#pragma once

#include "byte_stream.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"
#include "retransmission_timer.hh"

#include <functional>
#include <list>

class TCPSender
{
public:
  /* Construct TCP sender with given default Retransmission Timeout and possible ISN */
  TCPSender( ByteStream&& input, Wrap32 isn, uint64_t initial_RTO_ms )
    : input_( std::move( input ) ), isn_( isn ), last_ack_( isn ), initial_RTO_ms_( initial_RTO_ms ), RTO_ms_( initial_RTO_ms )
  {}

  /* Generate an empty TCPSenderMessage */
  TCPSenderMessage make_empty_message() const;

  /* Receive and process a TCPReceiverMessage from the peer's receiver */
  void receive( const TCPReceiverMessage& msg );

  /* Type of the `transmit` function that the push and tick methods can use to send messages */
  using TransmitFunction = std::function<void( const TCPSenderMessage& )>;

  /* Push bytes from the outbound stream */
  void push( const TransmitFunction& transmit );

  /* Time has passed by the given # of milliseconds since the last time the tick() method was called */
  void tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit );

  // Accessors
  uint64_t sequence_numbers_in_flight() const;  // How many sequence numbers are outstanding?
  uint64_t consecutive_retransmissions() const; // How many consecutive retransmissions have happened?
  const Writer& writer() const { return input_.writer(); }
  const Reader& reader() const { return input_.reader(); }
  Writer& writer() { return input_.writer(); }

private:
  Reader& reader() { return input_.reader(); }

  ByteStream input_;
  Wrap32 isn_;
  Wrap32 last_ack_;
  uint64_t initial_RTO_ms_;
  uint64_t RTO_ms_;
  uint64_t last_tick_ms_ {};
  uint16_t window_size_ { 1 };
  uint16_t retransmission_count_ { 0 }; // the count of “consecutive retransmissions”
  std::list<TCPSenderMessage> outstanding_ {}; // list of outstanding segments
  RetransmissionTimer timer_ {};
  bool syn_ {}; // whether if the connection is initialized
  bool fin_ {}; // whether TCPSenderMessage with FIN flag is sent at least once
};
