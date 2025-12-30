#include "wrapping_integers.hh"
#include "debug.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  return Wrap32 { zero_point } + static_cast<uint32_t>(n) ;
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  uint32_t ckpt_wrap = Wrap32::wrap(checkpoint, zero_point).raw_value_;
  uint32_t offset = raw_value_ - ckpt_wrap;
  uint64_t abs_seqno = checkpoint + offset;

  if (offset > (1u << 31) && abs_seqno >= (1ul << 32)) {
    abs_seqno -= (1ul << 32);
  }
  return abs_seqno;
}
