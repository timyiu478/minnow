#pragma once

#include <cstdint>

class RetransmissionTimer
{
public:
  explicit RetransmissionTimer() {}

  void start( uint64_t expire_time );

  bool is_started();

  bool is_expired( uint64_t now );

  void stop();

private:
  bool started_ {};
  uint64_t expire_time_ {};
};
