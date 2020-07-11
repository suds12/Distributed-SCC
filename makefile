# These are Sudharshan-only settings, which can be changed in the environment for correct locations elsewhere
mpi_base?=/usr/local/packages/mpich/3.2/gcc-5
shared_scc?=/home/users/ssriniv2/SCC/SharedSCC
DEBUG?=1
CPPFLAGS=-g -std=c++14 -g3 -DDEBUG=$(DEBUG)
OBJECTS=main.o graphReader.o update.o reader.o utils.o

# Boost settings
BOOST_ROOT?=/packages/boost/1_73_0
CPPFLAGS+=-I$(BOOST_ROOT) -I$(BOOST_ROOT)/include
LDFLAGS+=-L$(BOOST_ROOT)/lib -Wl,-rpath,$(BOOST_ROOT)/lib
LIBS=-lboost_serialization -lboost_program_options

# KaHIP/ParHIP -- must be built with CMake, see README.md
KAHIP_CXX=mpicxx # this has to be openmpi, the same one as used to build KaHIP
KAHIP_CXXFLAGS=-I./KaHIP_v2.12/parallel/parallel_src/lib \
    -I./KaHIP_v2.12/parallel/modified_kahip/lib \
    -I./KaHIP_v2.12/parallel/modified_kahip/lib/tools \
    -I./KaHIP_v2.12/parallel/modified_kahip/deploy 
KAHIP_LIB=./KaHIP_v2.12/build/parallel/modified_kahip/liblibmodified_kahip_interface.a
KAHIP_BUILD=./KaHIP_v2.12/build/parallel/parallel_src/CMakeFiles/libparallel.dir/lib
KAHIP_IO_OBJ=$(KAHIP_BUILD)/io/parallel_graph_io.cpp.o \
    $(KAHIP_BUILD)/data_structure/parallel_graph_access.cpp.o \
    $(KAHIP_BUILD)/data_structure/balance_management.cpp.o \
    $(KAHIP_BUILD)/data_structure/balance_management_coarsening.cpp.o \
    $(KAHIP_BUILD)/data_structure/balance_management_refinement.cpp.o

# To enable PETSc logging, set the PETSC_DIR and PETSC_ARCH environment variables.
# For example, on arya, you can use the command to compile and run the small example: 
#    make PETSC_DIR=/shared/soft/petsc-git PETSC_ARCH=arch-linux2-c-opt run 
# For deugging use:
#    make PETSC_DIR=/shared/soft/petsc-git PETSC_ARCH=arch-linux2-c-opt run
ifdef PETSC_DIR
   include ${PETSC_DIR}/lib/petsc/conf/variables
   mpi_base=$(PETSC_DIR)/$(PETSC_ARCH)
   CPPFLAGS+= -I$(PETSC_DIR)/include -DHAVE_PETSC
   LIBS+=-lpetsc $(PETSC_EXTERNAL_LIB_BASIC)
   RUNOPTS+=-log_view
endif


CC=$(mpi_base)/bin/mpic++
run=$(mpi_base)/bin/mpirun

all: main

%.o : %.cpp
	$(CC) $(CPPFLAGS) -c $<

main: $(OBJECTS) $(WILDCARD *.hpp)
	$(CC) $(LDFLAGS) -o $@  $(OBJECTS) $(LDFLAGS) $(LIBS)

# Convert an edgelist graph to a binary format for use with partitioner
converter: converter.cpp $(KAHIP_IO_OBJ) $(KAHIP_LIB)
	$(KAHIP_CXX) $(KAHIP_CXXFLAGS) $(LDFLAGS) -o converter.exe  $^

run: main
	$(run) $(MPIRUNOPTS) -np 3 ./main input/distributed/g2/input_test input/distributed/g2/sccmap_test input/distributed/g2/change_test 2 1 input/distributed/g2/partition 3 $(RUNOPTS)

run1:
	$(run) $(MPIRUNOPTS) -np 1 ./main input/inputgraph input/sccinput input/changes 5 1 input/distributed/partition 3 $(RUNOPTS)

sp:
	$(run) $(MPIRUNOPTS) -np 1 ./main input/distributed/inputgraph input/distributed/sccmap input/distributed/change 11 1 input/distributed/partition1 1 $(RUNOPTS)

orkut: main
	$(run) $(MPIRUNOPTS) -np 8 ./main ../orkut/input.txt ../orkut/sccmap ../orkut/changes_orkut 2 1 ../orkut/partition_8.txt 3 $(RUNOPTS)

facebook: main
	$(run) $(MPIRUNOPTS) -np 3 ./main input/facebook/facebook_combined.txt input/facebook/scc_map input/facebook/changes 2 1 input/facebook/partition_facebook.txt 3 $(RUNOPTS)

clean:
	$(RM) *.o main

clean_dump:
	rm dump/*
