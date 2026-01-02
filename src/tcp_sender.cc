#include "tcp_sender.hh"
#include "debug.hh"
#include "parser.hh"
#include "tcp_config.hh"
#include <algorithm>

using namespace std;

// How many sequence numbers are outstanding?
uint64_t TCPSender::sequence_numbers_in_flight() const
{
  uint64_t total = 0;
  for ( const auto &msg : outstanding_ ) {
    total += msg.sequence_length();
  }
  return total;
}

// How many consecutive retransmissions have happened?
uint64_t TCPSender::consecutive_retransmissions() const
{
  return retransmission_count_;
}

void TCPSender::push( const TransmitFunction& transmit )
{
  // Pretend like the window size is one
  uint16_t window = window_size_ > 0 ? window_size_ : 1 ;

  string_view bytes = input_.reader().peek();

  // No space available in the window
  // or no bytes to send (with no SYN and FIN)
  // or the stream is closed and the last message is outgoing/acknowledged
  if ( (syn_ && !input_.writer().is_closed() && bytes.size() == 0) || sequence_numbers_in_flight() >= window || fin_ ) { return; }

  // Prepare the payload and pop the payload_size from the input_ stream
  uint16_t available_window = window - sequence_numbers_in_flight();
  uint16_t payload_size = min(static_cast<uint16_t>(bytes.size()), available_window);
  payload_size = min(payload_size, static_cast<uint16_t>(TCPConfig::MAX_PAYLOAD_SIZE));
  string payload(bytes.substr( 0, payload_size ));
  input_.reader().pop(payload_size);

  bool fin = input_.writer().is_closed();
  bool rst = input_.has_error() || rst_;
  bool syn = syn_ ? false : true;

  if ( !syn_ ) {
    syn_ = true;
  }

  if ( fin ) {
    fin_ = true;
  }

  Wrap32 seqno = last_ack_;

  if ( outstanding_.size() > 0 ) {
    seqno = outstanding_.back().seqno + outstanding_.back().sequence_length();
  }

  TCPSenderMessage msg = TCPSenderMessage{
    seqno,
    syn,
    payload,
    fin,
    rst
  };

  debug("bytes size: {}", bytes.size());

  transmit(msg);

  // Add msg to outstanding_
  outstanding_.push_back(msg);

  // Start timer
  if ( !timer_.is_started() ) {
    timer_.start( now_ + RTO_ms_ );
  }
}

TCPSenderMessage TCPSender::make_empty_message() const
{
  if ( outstanding_.size() == 0 ) {
    return TCPSenderMessage{last_ack_, {}, {}, {}};
  }

  return TCPSenderMessage{outstanding_.back().seqno + outstanding_.back().sequence_length(), {}, {}, {}};
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
  // Update window size
  window_size_ = msg.window_size;

  // Look through its collection of outstanding segments and remove any that have now been fully acknowledged
  bool acked = false; // whether the segment(s) are fully acknowledged
  if ( msg.ackno.has_value() ) {
    uint64_t ackno = msg.ackno.value().unwrap(isn_, input_.reader().bytes_popped());

    // Wrong ack
    if ( outstanding_.size() == 0 ) {
      debug( "wrong ack: no outstanding msg");
      return;
    }
    uint64_t expect_largest_ack = outstanding_.back().seqno.unwrap(isn_, input_.reader().bytes_popped()) + static_cast<uint64_t>(outstanding_.back().sequence_length());
    if ( ackno > expect_largest_ack ) {
      debug( "wrong ack: expect_ack is {}, ackno is {}", expect_largest_ack, ackno);
      return;
    }
    
    std::list<TCPSenderMessage>::iterator it = outstanding_.begin();

    for ( ; it != outstanding_.end(); ++it ) {
     uint64_t expect_ack = it->seqno.unwrap(isn_, input_.reader().bytes_popped()) + static_cast<uint64_t>(it->sequence_length());

     debug( "ackno: {}, expect_ack: {}", ackno, expect_ack );

     if ( expect_ack > ackno ) { break; }
    }

    if ( it != outstanding_.begin() ) {
      outstanding_.erase(outstanding_.begin(), it);
      acked = true;
    }

    last_ack_ = msg.ackno.value();
  }

  // Timer doesn't restart without ACK of new data
  if ( !acked ) {
    return;
  }

  // Handle RST
  if ( msg.RST ) {
    input_.set_error();
  }

  // Reset
  retransmission_count_= 0;
  RTO_ms_ = initial_RTO_ms_;

  if ( outstanding_.size() == 0) {
    timer_.stop();
  } else {
    timer_.start( now_ + RTO_ms_ );
  }
}

void TCPSender::tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit )
{
  now_ += ms_since_last_tick;
  
  if ( !timer_.is_started() ) {
    debug("tick: do nothing since timer is not started.");
    return;
  }
  if ( !timer_.is_expired( now_ ) ) {
    debug("tick: do nothing since timer is not expired. now is {}", now_ );
    return;
  }
  if ( window_size_ == 0 || !syn_ ) {
    return;
  }
  if ( outstanding_.size() == 0 ) {
    debug("outstanding_.size() should > 0 if the timer is expired");
    return;
  }

  retransmission_count_ += 1;
  RTO_ms_ *= 2;

  // Give up the connection
  if ( retransmission_count_ > TCPConfig::MAX_RETX_ATTEMPTS ) {
    rst_ = true;
    outstanding_.front().RST = true;
  }

  timer_.start(now_ + RTO_ms_);

  transmit(outstanding_.front());
}
