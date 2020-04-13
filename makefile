mpi_base=/usr/local/packages/mpich/3.2/gcc-5
boost=/home/users/ssriniv2/packages/boost_1_72_0
shared_scc=/home/users/ssriniv2/SCC/SharedSCC
CC=$(mpi_base)/bin/mpic++
run=$(mpi_base)/bin/mpirun
mpi_include=$(mpi_base)/include
mpi_lib=$(mpi_base)/lib



all: main.cpp merge.hpp 
	$(CC) -o main -g -w main.cpp global.hpp -I$(boost) -I$(mpi_include) -I$(shared_scc) -L$(mpi_lib) -lmpich -std=c++11
# run:
# 	$(run) -np 3 ./main input/distributed/inputgraph input/distributed/sccmap input/distributed/changes 11 1 input/distributed/partition 3

run1:
	$(run) -np 1 ./main input/inputgraph input/sccinput input/changes 5 1 input/distributed/partition 3	

sp:
	$(run) -np 1 ./main input/distributed/inputgraph input/distributed/sccmap input/distributed/changes 11 1 input/distributed/partition1 1
