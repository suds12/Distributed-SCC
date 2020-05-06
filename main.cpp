#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include <map>
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
    Graph changes (11);
    MetaGraph meta_graph;
    read_partitions(argv,basic,graph);
    read_graph(argv,basic,graph,world_rank);
    read_changes(argv,basic,changes,graph,world_rank);
    read_sccmap(argv,basic,world_rank);
    perform_scc(argv,basic,graph,world_rank);
    make_meta(argv,basic,graph,world_rank);
    send_meta(argv,basic,world_rank);
    if(world_rank==0)
    {
        make_meta_graph(argv,basic,meta_graph,world_rank);
        recompute_scc(basic,meta_graph,world_rank);
        create_result(basic,meta_graph,world_rank);
    }
    scatter_global(basic,meta_graph,world_rank);



    display(basic,graph,world_rank);
  	MPI_Finalize();
	return 0;
}