#include "reassembler.hh"
#include "debug.hh"

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{

  if (is_last_substring) {
    if (first_index == 0 && data.size() == 0) {
      output_.writer().close();
      return;
    }
    last_substring_index = first_index + data.size() - 1;
  } else if (data.size() == 0) {
    return;
  }

  uint64_t first_unaccepted_index = output_.reader().bytes_popped() + output_.writer().available_capacity();

  uint64_t last_index = first_index + data.size() - 1;

  if (last_index < first_unassemebled_index_) {
    return;
  }

  // Trim data
  if (last_index >= first_unaccepted_index) {
    data = data.substr(0, output_.writer().available_capacity());
    is_last_substring = false;
  }
  

  for (vector<Segment>::iterator it = segments_.begin(); it != segments_.end(); ++it) {
    uint64_t seg_last_index = it->first_index + it->data.size() - 1;

    if (last_index < it->first_index) {
      segments_.insert(it, Segment(first_index, data));
      break;
    }
    if (first_index > seg_last_index) {
      segments_.push_back(Segment(first_index, data));
      break;
    }

    if (first_index < it->first_index) {
      it->data = data.substr(0, it->first_index - first_index) + it->data;
      it->first_index = first_index;
    }

    if (last_index <= seg_last_index) {
      break;
    }

    first_index = seg_last_index + 1;
    data = data.substr(last_index + 1);
  }
  
  if (segments_.size() == 0) {
    segments_.push_back(Segment(first_index, data));
  } else {
    Segment last_seg = segments_.back();
    if (first_index > last_seg.first_index + last_seg.data.size() - 1) {
      segments_.push_back(Segment(first_index, data));
    }
  }

  uint i = 0;
  for (; i < segments_.size(); i++) {
    Segment seg = segments_.at(i);
    if (seg.first_index == first_unassemebled_index_) {
      output_.writer().push(seg.data);
      uint64_t seg_last_index = seg.first_index + seg.data.size() - 1;
      first_unassemebled_index_ = seg_last_index + 1;
      if (seg_last_index == last_substring_index) {
        output_.writer().close();
        break;
      }
    } else {
      break;
    }
  }

  if (i < segments_.size()) {
    segments_ = std::vector<Segment>(segments_.begin() + i, segments_.end());
  } else if (i >= segments_.size()) {
    segments_.clear();
  }
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
