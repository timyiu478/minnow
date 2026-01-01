#include "retransmission_timer.hh"


void RetransmissionTimer::start( uint64_t expire_time ) {
  expire_time_ = expire_time;
}

bool RetransmissionTimer::is_expired( uint64_t now ) {
  if ( !started_ ) return false;

  return now > expire_time_;
}

bool RetransmissionTimer::is_started() {
  return started_;
}

void RetransmissionTimer::stop() {
  started_ = false;
}
