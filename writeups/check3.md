# Implementation Challenges

* How to implement the retransmission timer? We need to implement it using tick method since the tick method is our only access to the passage of time?
* The TCP sender may receive reordered messages from the TCP receiver because they network is unreliable.
