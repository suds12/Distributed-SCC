mpi_base?=/usr/local/packages/mpich/3.2/gcc-5
boost?=/home/users/ssriniv2/packages/boost_1_72_0
shared_scc=/home/users/ssriniv2/SCC/SharedSCC
CC=$(mpi_base)/bin/mpic++
run=$(mpi_base)/bin/mpirun
CFLAGS?=-g3 -I$(boost) -I$(boost)/include -std=c++11

# ifdef PETSC_DIR
#   mpi_base=$(PETSC_DIR)/$(PETSC_ARCH)
#   CFLAGS+=-I$(PETSC_DIR)/include -I$(PETSC_DIR)/$(PETSC_ARCH)/include -DHAVE_PETSC
#   LDFLAGS+=-L$(PETSC_DIR)/$(PETSC_ARCH)/lib 
#   LIBS+=-lpetsc
# endif

%.o : %.cpp
	$(CC) $(CFLAGS) -c -w $< 

all: main.o 
	$(CC) $(CFLAGS) -o main  $^ $(LDFLAGS) $(LIBS)

run:
	$(run) -np 3 ./main input/distributed/g2/input_test input/distributed/g2/sccmap_test input/distributed/g2/change_test 2 1 input/distributed/g2/partition 3

run1:
	$(run) -np 1 ./main input/inputgraph input/sccinput input/changes 5 1 input/distributed/partition 3	

sp:
	$(run) -np 1 ./main input/distributed/inputgraph input/distributed/sccmap input/distributed/change 11 1 input/distributed/partition1 1

orkut:
	$(run) -np 3 ./main ../orkut/input.txt ../orkut/sccmap ../orkut/changes_orkut 2 1 ../orkut/partition.txt 3

facebook:
	$(run) -np 3 ./main input/facebook/facebook_combined.txt input/facebook/scc_map input/facebook/changes 2 1 input/facebook/partition_facebook.txt 3
clean:
	$(RM) *.o main
