#include "reassembler.hh"
#include "debug.hh"
#include <iostream>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{

  if ( is_last_substring ) {
    first_close_index_ = first_index + data.size();
    is_closed_ = true;
  }

  uint64_t first_unacceptable_index = first_unassemebled_index_ + output_.writer().available_capacity();

  if ( first_index >= first_unacceptable_index || ( is_closed_ && first_index > first_close_index_ ) ) {
    return;
  }
  
  // Segment index range: [first_index, end_index)
  uint64_t end_index = first_index + data.size();

  // Remove assembled data
  if ( end_index <= first_unassemebled_index_ ) {
    if ( is_closed_ ) {
      output_.writer().close();
    }
    return;
  }
  if ( first_index < first_unassemebled_index_ && data.size() > 0 ) {
    data = data.substr(first_unassemebled_index_ - first_index);
    first_index = first_unassemebled_index_;
    end_index = first_index + data.size();
  }
  
  bool is_handled = data.size() == 0 ? true : false ; // whether the new string is totally handled by the existing segment(s)
  list<Segment>::iterator it = segments_.begin();

  for ( ; it != segments_.end(); ++it ) {
    uint64_t seg_end_index = it->first_index + it->data.size();

    // the string is not overlap with this segment
    // and before this segment
    // and not able to merge
    if ( end_index < it->first_index ) {
      break;
    }

    // merge the new string's front part: [first_index, it->first_index)
    if ( first_index < it->first_index && end_index >= it->first_index ) {
      it->data = data.substr(0, it->first_index - first_index) + it->data;
      it->first_index = first_index;
    }

    // totally overlap with this segment after merge
    if ( it->first_index <= first_index && end_index <= seg_end_index ) {
      is_handled = true;
      break;
    }

    // std::cout << format("first index {}, data size {}, data {}\n", first_index, data.size(), data);
    // Trim the new string to [seg_end_index, end_index)
    data = data.substr(seg_end_index - first_index);
    first_index = seg_end_index;
  }

  if ( !is_handled ) {
    // Discard the bytes beyond the stream's available capacity
    // or discard the bytes start from first_close_index_
    if (is_closed_) {
      first_unacceptable_index = min(first_close_index_, first_unacceptable_index);
    }
    if (end_index > first_unacceptable_index) {
      data = data.substr(0, data.size() - (end_index - first_unacceptable_index));
    }
    if (data.size() > 0) {
      segments_.insert(it, Segment(first_index, data, false));
    }
  }

  // Push data
  for ( auto &seg : segments_ ) {
    if ( seg.first_index == first_unassemebled_index_ ) {
      output_.writer().push(seg.data);
      uint64_t seg_end_index = seg.first_index + seg.data.size();
      first_unassemebled_index_ = seg_end_index;
      seg.pushed = true;
      if ( is_closed_ && first_unassemebled_index_ == first_close_index_ ) {
        output_.writer().close();
        segments_.clear();
        break;
      }
    } else {
      break;
    }
  }

  // Clean up the pushed segments
  segments_.remove_if([](Segment seg){ return seg.pushed; });
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  uint64_t total = 0;
  for (const auto& seg : segments_) {
    total += seg.data.size();
  }
  return total;
}
