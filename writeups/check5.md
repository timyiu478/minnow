# Network Interface

## Implementation Tip

The network interface shouldn’t store a datagram beyond the time when it would be willing to send another ARP request for the same IP address.

To implement this, you can store a timestamp alongside each entry in the datagram queue, and when the interface tries to resend the datagram after learning the Ethernet address from the ARP message(both request and reply), compare the current time to the stored timestamp. If the elapsed time exceeds a certain threshold (5 seconds in our config), consider the entry stale and do not resend this datagram.

## Test Result

Machine Stack:

* OS: Ubuntu 22.04.3 LTS
* CPU: 13th Gen Intel(R) Core(TM) i5-13600

```
tim@tim-virtual-machine ~/g/minnow-2025 (ch5)> cmake --build build --target check5
Test project /home/tim/git/minnow-2025/build
    Start  1: compile with bug-checkers
1/3 Test  #1: compile with bug-checkers ........   Passed    0.65 sec
    Start 35: net_interface
2/3 Test #35: net_interface ....................   Passed    0.17 sec
    Start 37: no_skip
3/3 Test #37: no_skip ..........................   Passed    0.02 sec

100% tests passed, 0 tests failed out of 3
```
