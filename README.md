# MaiterStore

**MaiterStore**: A Hot-aware, High-Performance Key-Value Store for Graph Processing

MaiterStore is a high-performance key-value storage system, which addresses the scalability challenge by using solid state drives (SSDs). It treats SSDs as an extension of memory and optimizes the data structures for fast query of the large graphs on SSDs. Furthermore, observing that hot-spot property and skewed power-law degree distribution are widely existed in real graphs, it adapt a ***hot-aware caching (HAC) policy*** to effectively manage the hot vertices (frequently accessed vertices). HAC can conduce to the substantial acceleration of the graph iterative execution.
