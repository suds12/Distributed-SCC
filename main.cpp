#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include "merge.hpp"
#include "basic.hpp"
//#include "main_code.cpp"
#include "update.cpp"
#include "reader.hpp"

using namespace std;
//Global variables
vector<set <int> > sccSets;
int world_rank, world_size, local_size;


int main(int argc, char *argv[])
{
    
    //-----------------------------------
	// Initialize the MPI environment
    MPI_Init(NULL, NULL);
    // Get the number of processes
    //int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    // Get the rank of the process
    //int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    //shared_scc(argc, argv);
    //Reader r1;
    Basic basic;
    Graph graph (11);
    read_partitions(argv,basic,graph);
    // Update u1
    // u1.read_partitions(argv);
    read_graph(argv,basic,graph,world_rank);
    perform_scc(argv,basic,graph,world_rank);
    //disjoint_union(basic,graph,world_rank);

    
    

    // Merge m1;
    // m1.perform_scc(argv,world_rank);

    display(basic,graph,world_rank);
  	MPI_Finalize();
	return 0;
}