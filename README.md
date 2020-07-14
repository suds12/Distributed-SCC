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
