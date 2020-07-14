# Distributed-SCC

## 1. Partitioning
We use the https://github.com/KaHIP/KaHIP partitioning framework to partition the graph among MPI tasks. We do this offline, by generated desired 
partitions for each graph. The input to KaHIP uses the Metis graph format, so a preliminary optional step is to convert the graph to that format.

We provide an edgelist to binary Metis format converted. To build, first build KaHIP, then build and use the converter. Details follow.

### 1.1 Building KaHIP
Prerequisites: OpenMPI, GCC compiler, argtable (on Ubuntu, you can install argtable with `sudo apt install libargtable2-dev`)
This repository includes a copy of KaHIP v. 2.12. To build using the CMake build:
```cd KaHIP_v2.12
mkdir build
cd build 
cmake ..
make
```
On a Mac, you may have to specify the C++ compiler to be GCC (Clang does not work), e.g., `cmake -DCMAKE_CXX_COMPILER=g++-mp-8` if you are using Mac Ports.

### 1.2 Building the converter
To build the edgelist to Metis graph converter, simply run `make converter` after building KaHIP 

To use the converter, simply give the input and output file names, e.g.,
```./converter.exe inputgraph inputgraph.metis```

### 1.3 Generating partitions
