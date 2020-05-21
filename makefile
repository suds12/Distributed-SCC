mpi_base?=/usr/local/packages/mpich/3.2/gcc-5
boost?=/home/users/ssriniv2/packages/boost_1_72_0
shared_scc=/home/users/ssriniv2/SCC/SharedSCC
CFLAGS?=-g3 -I$(boost) -I$(boost)/include -std=c++11
LIBS = /home/users/ssriniv2/packages/boost_1_72_0/stage/lib/libboost_serialization.a


# Comment out just the PETSC_DIR line below to disable PETSc
PETSC_DIR?=/shared/soft/petsc-git
PETSC_ARCH?=arch-linux2-c-debug
ifdef PETSC_DIR
   include ${PETSC_DIR}/lib/petsc/conf/variables
   mpi_base=$(PETSC_DIR)/$(PETSC_ARCH)
   CFLAGS+=-I$(PETSC_DIR)/include -I$(PETSC_DIR)/$(PETSC_ARCH)/include -DHAVE_PETSC
   LDFLAGS+=-L$(PETSC_DIR)/$(PETSC_ARCH)/lib 
   LIBS+=-lpetsc $(PETSC_EXTERNAL_LIB_BASIC)
   RUNOPTS=-log_view
endif

CC=$(mpi_base)/bin/mpic++
run=$(mpi_base)/bin/mpirun

%.o : %.cpp
	$(CC) $(CFLAGS) -c -w $< 

all: main.o 
	$(CC) $(CFLAGS) -o main  $^ $(LDFLAGS) $(LIBS)

run:
	$(run) -np 3 ./main input/distributed/g2/input_test input/distributed/g2/sccmap_test input/distributed/g2/change_test 2 1 input/distributed/g2/partition 3 $(RUNOPTS)

run1:
	$(run) -np 1 ./main input/inputgraph input/sccinput input/changes 5 1 input/distributed/partition 3 $(RUNOPTS)

sp:
	$(run) -np 1 ./main input/distributed/inputgraph input/distributed/sccmap input/distributed/change 11 1 input/distributed/partition1 1 $(RUNOPTS)

orkut:
	$(run) -np 8 ./main ../orkut/input.txt ../orkut/sccmap ../orkut/changes_orkut 2 1 ../orkut/partition_8.txt 3 $(RUNOPTS)

facebook:
	$(run) -np 3 ./main input/facebook/facebook_combined.txt input/facebook/scc_map input/facebook/changes 2 1 input/facebook/partition_facebook.txt 3 $(RUNOPTS)
clean:
	$(RM) *.o main

clean dump:
	rm dump/*
