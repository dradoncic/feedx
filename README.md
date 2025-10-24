# FeedBridge

Feed abstracts away exchange-specific websocket quirks and message formats into a unified, high-performance data pipeline.

It’s built to handle thousands of messages per second with deterministic latency, using:

- Lock-free SPSC queues for concurrent message dispatch

- Shared-memory access for multi-process strategies

- Pluggable parsers and dispatchers for flexible data schemas

- Efficient serialization for low-overhead inter-process communication

FeedX is platform-agnostic, extensible, and optimized for both HFT-grade performance and clean modular design.
