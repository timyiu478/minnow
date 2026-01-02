# Test Result

Machine Stack:

* OS: Ubuntu 22.04.3 LTS
* CPU: 13th Gen Intel(R) Core(TM) i5-13600

```
Test project /home/tim/git/minnow-2025/build
      Start  1: compile with bug-checkers
 1/37 Test  #1: compile with bug-checkers ........   Passed    7.88 sec
      Start  3: byte_stream_basics
 2/37 Test  #3: byte_stream_basics ...............   Passed    0.03 sec
      Start  4: byte_stream_capacity
 3/37 Test  #4: byte_stream_capacity .............   Passed    0.03 sec
      Start  5: byte_stream_one_write
 4/37 Test  #5: byte_stream_one_write ............   Passed    0.03 sec
      Start  6: byte_stream_two_writes
 5/37 Test  #6: byte_stream_two_writes ...........   Passed    0.03 sec
      Start  7: byte_stream_many_writes
 6/37 Test  #7: byte_stream_many_writes ..........   Passed    0.18 sec
      Start  8: byte_stream_stress_test
 7/37 Test  #8: byte_stream_stress_test ..........   Passed    0.08 sec
      Start  9: reassembler_single
 8/37 Test  #9: reassembler_single ...............   Passed    0.03 sec
      Start 10: reassembler_cap
 9/37 Test #10: reassembler_cap ..................   Passed    0.03 sec
      Start 11: reassembler_seq
10/37 Test #11: reassembler_seq ..................   Passed    0.05 sec
      Start 12: reassembler_dup
11/37 Test #12: reassembler_dup ..................   Passed    0.10 sec
      Start 13: reassembler_holes
12/37 Test #13: reassembler_holes ................   Passed    0.03 sec
      Start 14: reassembler_overlapping
13/37 Test #14: reassembler_overlapping ..........   Passed    0.03 sec
      Start 15: reassembler_win
14/37 Test #15: reassembler_win ..................   Passed    1.57 sec
      Start 16: wrapping_integers_cmp
15/37 Test #16: wrapping_integers_cmp ............   Passed    0.02 sec
      Start 17: wrapping_integers_wrap
16/37 Test #17: wrapping_integers_wrap ...........   Passed    0.01 sec
      Start 18: wrapping_integers_unwrap
17/37 Test #18: wrapping_integers_unwrap .........   Passed    0.02 sec
      Start 19: wrapping_integers_roundtrip
18/37 Test #19: wrapping_integers_roundtrip ......   Passed    0.67 sec
      Start 20: wrapping_integers_extra
19/37 Test #20: wrapping_integers_extra ..........   Passed    0.59 sec
      Start 21: recv_connect
20/37 Test #21: recv_connect .....................   Passed    0.04 sec
      Start 22: recv_transmit
21/37 Test #22: recv_transmit ....................   Passed    0.82 sec
      Start 23: recv_window
22/37 Test #23: recv_window ......................   Passed    0.03 sec
      Start 24: recv_reorder
23/37 Test #24: recv_reorder .....................   Passed    0.03 sec
      Start 25: recv_reorder_more
24/37 Test #25: recv_reorder_more ................   Passed    3.86 sec
      Start 26: recv_close
25/37 Test #26: recv_close .......................   Passed    0.03 sec
      Start 27: recv_special
26/37 Test #27: recv_special .....................   Passed    0.07 sec
      Start 28: send_connect
27/37 Test #28: send_connect .....................   Passed    0.04 sec
      Start 29: send_transmit
28/37 Test #29: send_transmit ....................   Passed    2.16 sec
      Start 30: send_window
29/37 Test #30: send_window ......................   Passed    0.63 sec
      Start 31: send_ack
30/37 Test #31: send_ack .........................   Passed    0.03 sec
      Start 32: send_close
31/37 Test #32: send_close .......................   Passed    0.04 sec
      Start 33: send_retx
32/37 Test #33: send_retx ........................   Passed    0.04 sec
      Start 34: send_extra
33/37 Test #34: send_extra .......................   Passed    0.11 sec
      Start 37: no_skip
34/37 Test #37: no_skip ..........................   Passed    0.02 sec
      Start 38: compile with optimization
35/37 Test #38: compile with optimization ........   Passed    2.37 sec
      Start 39: byte_stream_speed_test
36/37 Test #39: byte_stream_speed_test ...........   Passed    0.40 sec
      Start 40: reassembler_speed_test
37/37 Test #40: reassembler_speed_test ...........   Passed    0.47 sec

100% tests passed, 0 tests failed out of 37

        ByteStream throughput (pop length 4096):  2.23 Gbit/s
        ByteStream throughput (pop length 128):   2.21 Gbit/s
        ByteStream throughput (pop length 32):    2.06 Gbit/s
        Reassembler throughput (no overlap):  31.27 Gbit/s
        Reassembler throughput (10x overlap):  6.23 Gbit/s
```

---

# Implementation Challenges

* How to implement the retransmission timer? We need to implement it using tick method since the tick method is our only access to the passage of time?
* The TCP sender may receive reordered messages from the TCP receiver because they network is unreliable.

