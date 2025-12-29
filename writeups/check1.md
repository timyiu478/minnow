# Reassembler

## The `insert` function



## Test Results

Machine Stack:

* OS: Ubuntu 22.04.3 LTS
* CPU: 13th Gen Intel(R) Core(TM) i5-13600


```console
      Start  1: compile with bug-checkers
 1/18 Test  #1: compile with bug-checkers ........   Passed    6.52 sec
      Start  3: byte_stream_basics
 2/18 Test  #3: byte_stream_basics ...............   Passed    0.03 sec
      Start  4: byte_stream_capacity
 3/18 Test  #4: byte_stream_capacity .............   Passed    0.03 sec
      Start  5: byte_stream_one_write
 4/18 Test  #5: byte_stream_one_write ............   Passed    0.03 sec
      Start  6: byte_stream_two_writes
 5/18 Test  #6: byte_stream_two_writes ...........   Passed    0.03 sec
      Start  7: byte_stream_many_writes
 6/18 Test  #7: byte_stream_many_writes ..........   Passed    0.15 sec
      Start  8: byte_stream_stress_test
 7/18 Test  #8: byte_stream_stress_test ..........   Passed    0.07 sec
      Start  9: reassembler_single
 8/18 Test  #9: reassembler_single ...............   Passed    0.02 sec
      Start 10: reassembler_cap
 9/18 Test #10: reassembler_cap ..................   Passed    0.03 sec
      Start 11: reassembler_seq
10/18 Test #11: reassembler_seq ..................   Passed    0.04 sec
      Start 12: reassembler_dup
11/18 Test #12: reassembler_dup ..................   Passed    0.09 sec
      Start 13: reassembler_holes
12/18 Test #13: reassembler_holes ................   Passed    0.03 sec
      Start 14: reassembler_overlapping
13/18 Test #14: reassembler_overlapping ..........   Passed    0.03 sec
      Start 15: reassembler_win
14/18 Test #15: reassembler_win ..................   Passed    1.57 sec
      Start 37: no_skip
15/18 Test #37: no_skip ..........................   Passed    0.02 sec
      Start 38: compile with optimization
        ByteStream throughput (pop length 4096):  2.01 Gbit/s
        ByteStream throughput (pop length 128):   1.99 Gbit/s
        ByteStream throughput (pop length 32):    1.66 Gbit/s
        Reassembler throughput (no overlap):  31.15 Gbit/s
        Reassembler throughput (10x overlap):  6.94 Gbit/s
```
