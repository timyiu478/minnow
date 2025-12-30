# TCP Receiver

## Unwrap seqno

| # | Absolute relation                          | raw_value_ vs ckpt_wrap     | offset (unsigned)       | offset > 2³¹ ? | Subtract 2³² ? | Final unwrap result vs checkpoint | Typical real-world situation                        | Correct? |
|---|--------------------------------------------|-----------------------------|-------------------------|----------------|----------------|------------------------------------|-----------------------------------------------------|----------|
| 1 | seq > checkpoint (ahead, no wrap cross)    | raw_value_ > ckpt_wrap      | small (0 … 2³¹−1)       | No             | No             | > checkpoint                       | Normal in-order arrival                             | Yes      |
| 2 | seq > checkpoint (ahead, wrap cross)       | raw_value_ < ckpt_wrap      | small                   | No             | No             | > checkpoint                       | Segment arrives just after wrap point               | Yes      |
| 3 | seq < checkpoint (behind, same cycle)      | raw_value_ < ckpt_wrap      | large (> 2³¹)           | Yes            | Yes            | < checkpoint                       | Delayed / out-of-order / retransmitted packet       | Yes      |
| 4 | seq == checkpoint                          | raw_value_ == ckpt_wrap     | 0                       | No             | No             | == checkpoint                      | Perfect alignment (common at start of segment)      | Yes      |
| 5 | seq >> checkpoint (big jump forward)       | usually raw < ckpt_wrap     | large                   | Yes            | Yes            | much smaller than checkpoint       | Usually invalid / attack / bug                      | No (picks "closer") |
| 6 | seq << checkpoint (very old packet)        | raw > ckpt_wrap             | large                   | Yes            | Yes            | much larger than checkpoint        | Usually invalid / very old duplicate                | No (picks "closer") |

Co-pilot: Grok 4.1
