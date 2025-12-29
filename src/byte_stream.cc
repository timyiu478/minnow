#include "byte_stream.hh"
#include "debug.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) { buffer_.resize(capacity_); }

void Writer::push( string data )
{
  if ( closed_ && not data.empty() ) {
    throw runtime_error( "Writer:: push() called on closed stream" );
  }

  const uint64_t can_push = available_capacity();
  const uint64_t to_push = min( can_push, static_cast<uint64_t>( data.size() ) );

  for ( uint64_t i = 0; i < to_push; ++i ) {
    buffer_[( bytes_pushed_ + i ) % capacity_] = data[i];
  }

  bytes_pushed_ += to_push;
}

void Writer::close()
{
  closed_ = true;
}

bool Writer::is_closed() const
{
  return closed_;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - ( bytes_pushed_ - bytes_popped_ );
}

uint64_t Writer::bytes_pushed() const
{
  return bytes_pushed_;
}

string_view Reader::peek() const
{
  // Only return contiguous data from current position to end
  const uint64_t start_pos = bytes_popped_ % capacity_;
  const uint64_t bytes_in_buffer = bytes_pushed_ - bytes_popped_;
  
  if ( bytes_in_buffer == 0 ) {
    return string_view();  // Empty view
  }
  
  // Return only the contiguous portion before wraparound
  const uint64_t available = min( bytes_in_buffer, capacity_ - start_pos );

  return string_view( buffer_.data() + start_pos, available );
}

void Reader::pop( uint64_t len )
{
  if ( len > bytes_buffered() ) {
    throw runtime_error( "Reader::pop() called with len greater than buffered size" );
  }

  bytes_popped_ += len;
}

bool Reader::is_finished() const
{
  return closed_ && bytes_popped_ == bytes_pushed_;
}

uint64_t Reader::bytes_buffered() const
{
  return bytes_pushed_ - bytes_popped_;
}

uint64_t Reader:: bytes_popped() const
{
  return bytes_popped_;
}
