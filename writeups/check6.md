## Test Result

Machine Stack:

* OS: Ubuntu 22.04.3 LTS
* CPU: 13th Gen Intel(R) Core(TM) i5-13600

```
tim@tim-virtual-machine ~/g/minnow-2025 (ch6) [2]> cmake --build build --target check6
Test project /home/tim/git/minnow-2025/build
    Start  1: compile with bug-checkers
1/4 Test  #1: compile with bug-checkers ........   Passed    8.10 sec
    Start 35: net_interface
2/4 Test #35: net_interface ....................   Passed    0.27 sec
    Start 36: router
3/4 Test #36: router ...........................   Passed    0.08 sec
    Start 37: no_skip
4/4 Test #37: no_skip ..........................   Passed    0.02 sec

100% tests passed, 0 tests failed out of 4

Total Test time (real) =   8.47 sec
Built target check6
```
