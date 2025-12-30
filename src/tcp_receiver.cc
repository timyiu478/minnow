#include "tcp_receiver.hh"
#include "debug.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  if ( message.SYN ) {
    isn_ = message.seqno;
    isn_set_ = true;
  }

  if ( message.RST ) {
    reassembler_.reader().set_error();
  }
  
  if ( !isn_set_ || message.RST ) { return ; }

  uint64_t stream_index = message.seqno.unwrap( isn_, reassembler_.first_unassemebled_index() );

  if ( !message.SYN ) {
    stream_index -= 1;
  }

  reassembler_.insert( stream_index , message.payload, message.FIN );
}

TCPReceiverMessage TCPReceiver::send() const
{
  uint32_t fin = reassembler_.writer().is_closed() ? 1 : 0;
  uint64_t cap = reassembler_.writer().available_capacity();
  uint16_t window = (cap >= 65536) ? 65535 : static_cast<uint16_t>(cap);

  return TCPReceiverMessage{ 
    isn_set_ ? std::optional<Wrap32>(isn_.wrap( reassembler_.first_unassemebled_index(), isn_ ) + 1 + fin ) : std::nullopt,
    window, 
    reassembler_.writer().has_error()
  };
}
