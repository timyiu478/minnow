#include "byte_stream.hh"
#include "debug.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

// Push data to stream, but only as much as available capacity allows.
void Writer::push( string data )
{
  // make sure that the stream isn't closed
  if (closed_) {
    debug( "Unable to push data: the stream is closed" );
    return;
  }
  // make sure that the stream doesn’t exceed its storage capacit
  if (size_ + data.size() > capacity_) {
    debug( "Unable to push data: exceed byte stream capacity" );
    return;
  }
  // increase stream size
  size_ += static_cast<uint64_t>(data.size());

  // push data into stream
  stream_.push_back(data);
}

// Signal that the stream has reached its ending. Nothing more will be written.
void Writer::close()
{
  closed_ = true;
}

// Has the stream been closed?
bool Writer::is_closed() const
{
  return closed_;
}

// How many bytes can be pushed to the stream right now?
uint64_t Writer::available_capacity() const
{
  return capacity_ - size_;
}

// Total number of bytes cumulatively pushed to the stream
uint64_t Writer::bytes_pushed() const
{
  return size_;
}

// Peek at the next bytes in the buffer -- ideally as many as possible.
// It's not required to return a string_view of the *whole* buffer, but
// if the peeked string_view is only one byte at a time, it will probably force
// the caller to do a lot of extra work.
string_view Reader::peek() const
{
  popped_ += stream_.size();
  size_ -= stream_.size();

  std::string copyStream = stream_;

  stream_.clear();

  return copyStream;
}

// Remove `len` bytes from the buffer.
void Reader::pop( uint64_t len )
{
  popped_ += len;
  size_ -= len;

  stream_ = stream_.substr(len);
}

// Is the stream finished (closed and fully popped)?
bool Reader::is_finished() const
{
  return closed_ && size_ == 0;
}

// Number of bytes currently buffered (pushed and not popped)
uint64_t Reader::bytes_buffered() const
{
  return size_;
}

// Total number of bytes cumulatively popped from stream
uint64_t Reader::bytes_popped() const
{
  return popped_;
}
