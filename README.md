# Distributed-SCC

## 1. Partitioning

Install Metis, either follow instructions at http://glaros.dtc.umn.edu/gkhome/metis/metis/download or install the package, e.g., on Ubuntu:
```
sudo apt install libmetis-dev
```
Check the  `makefile` to make sure the paths to the Metis include file and library are correct.

Next, build the partitioner:
```
make partition
```
with debugging output enabled:
```
make DEBUG=1 partition
```
Run the partition:
```./partition
Usage: ./partition inputgraph num_part
	inputgraph: edgelist graph input
	num_part: number of partitions
```

For example
```
./partition input/inputgraph 2
```
This will create two partitioning files, each containing lines that are pairs of partition ID and vertex number.

Note that both ASCII and binary edgelist  formats are supported (each edge is represented with three ints -- source vertex ID, target vertex ID, weight).
Multiple partitionings can be generated at once by specifying a list instead of a single number of partitions, e.g.,
```
./partition input/inputgraph.edgelist.bin 2,3
```
This will produce the files
```
inputgraph.edgelist.part_002_0.bin
inputgraph.edgelist.part_002_1.bin
inputgraph.edgelist.part_003_0.bin
inputgraph.edgelist.part_003_1.bin
inputgraph.edgelist.part_003_2.bin
``
where `part_002_1` indicates partition 1 of a 2-part graph partitioning and `part_003_2` indicates partition 2 of a 3-part graph partitioning.

## 2. Utilities

The convert programer takes an ASCII edgelist graph as input and produces the binary equivalent.
Build with `make convert`. To run:
```
./convert inputgraphfile
```
will produce inputgraphfile.edgelist.bin binary edgelist file.
