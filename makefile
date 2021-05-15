mpi_base?=/usr/local/packages/mpich/3.2/gcc-5
boost?=/home/users/ssriniv2/packages/boost_1_72_0
shared_scc=/home/users/ssriniv2/SCC/SharedSCC
#CC=$(mpi_base)/bin/mpic++ 
#run=$(mpi_base)/bin/mpirun
CC=mpic++
run=mpirun
cuda_base=/storage/packages/hackathon-spack/padded-path-length-for-binaries/linux-rhel8-x86_64/gcc-8.3.1/cuda-11.2.0-i74bfb2ua5t6vse6wxbmbmfovzmmlmkr
stdgpu=/home/users/ssriniv2/packages/stdgpu/build


all: main.cpp 
	$(CC) -o main -g -w main.cpp -I$(boost)   -std=c++11

gpu: main.cpp
	$(CC) -o main -g -c -w main.cpp -I$(boost)   -std=c++11

cuda: stdgpu_set_example.cu
	nvcc -o set_ex -c stdgpu_set_example.cu -I$(stdgpu)/include -L$(stdgpu)/lib/ -lstdgpu 

combine:
	$(CC) main set_ex -o main_c -I$(stdgpu)/include -L$(cuda_base)/lib64 -lcudart -L$(stdgpu)/lib/ -lstdgpu 

debug: 
	$(run) -np 3 xterm -hold -e gdb -ex ./main input/distributed/g2/input_test input/distributed/g2/sccmap_test input/distributed/g2/change_test 2 1 input/distributed/g2/partition 3

run:
	$(run) -np 3 ./main input/distributed/g2/input_test input/distributed/g2/sccmap_test input/distributed/g2/change_test 2 1 input/distributed/g2/partition 3

run1:
	$(run) -np 1 ./main input/distributed/g2/input_test input/distributed/g2/sccmap_test input/distributed/g2/change_test 5 1 input/distributed/partition_1 1	

sp:
	$(run) -np 1 ./main input/distributed/inputgraph input/distributed/sccmap input/distributed/change 11 1 input/distributed/partition1 1

orkut:
	$(run) -np 3 ./main input/distributed/orkut/com-orkut.ungraph.txt input/distributed/orkut/sccmap input/distributed/orkut/changes_orkut 2 1 input/distributed/orkut/partition 3

fb:
	$(run) -np 4 ./main large_inputs/facebook/input_fb input/distributed/orkut/sccmap large_inputs/facebook/change_fb 2 1 large_inputs/facebook/facebook_partition_4 4

tau:
	$(run) -np 4 tau_exec ./main large_inputs/facebook/input_fb input/distributed/orkut/sccmap large_inputs/facebook/change_fb 2 1 large_inputs/facebook/facebook_partition_4 4

cuda_fb:
	 $(run) -np 4 ./main_c large_inputs/facebook/input_fb input/distributed/orkut/sccmap large_inputs/facebook/change_fb 2 1 large_inputs/facebook/facebook_partition_4 4
