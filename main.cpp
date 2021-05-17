#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include <map>
#include "merge.hpp"
#include "basic.hpp"
//#include "main_code.cpp"
#include "update.cpp"
#include "reader.hpp"
//#include "kamesh/scc_main.cpp"


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

#if DEBUG
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    printf("PID %d on %s ready for attach\n", getpid(), hostname);
    fflush(stdout);
#endif


  
    Basic basic;
    Graph graph;
    Graph changes;
    MetaGraph meta_graph;
    DeviceFuncs device;

    cout<<"reading partition from rank "<<world_rank<<endl;
    read_partitions(argv,basic,graph);

    cout<<"reading graph from rank "<<world_rank<<endl;
    read_graph(argv,basic,graph,world_rank);

    cout<<"reading changes from rank "<<world_rank<<endl;
    read_changes(argv,basic,changes,graph,world_rank);

    // cout<<"reading sccmap from rank "<<world_rank<<endl;
    // read_sccmap(argv,basic,world_rank);

    cout<<"Performing initial SCC from rank "<<world_rank<<endl;
    perform_scc(argv,basic,graph,world_rank);

    
    cout<<"Creating partial meta graph from rank "<<world_rank<<endl;
    create_partial_meta_graph(basic, world_rank);

    cout<<"Broadcasting meta nodes from rank "<<world_rank<<endl;
    bcast_meta_nodes(device, basic, world_rank, world_size);

    //-------GPU-------------

    //unpack_bcast_gpu(device, basic, world_rank, world_size);

    //------------------
    
    // cout<<"unpacking bcast from rank "<<world_rank<<endl;
    // unpack_bcast(basic, world_rank, world_size);

    // cout<<"Creating meta graph vector from rank "<<world_rank<<endl;
    // create_meta_graph_vector(basic, world_rank, world_size);

    // cout<<"Reducing meta graph vector from rank "<<world_rank<<endl;
    // reduce_meta_graph(basic, world_rank, world_size);

    // cout<<"Creating full meta graph vector from rank "<<world_rank<<endl;
    // create_full_meta_graph(basic, meta_graph, world_rank, world_size);

    // cout<<"Reperforming SCC from rank "<<world_rank<<endl;
    // reperform_scc(basic, meta_graph, world_rank, world_size);

    
    display(basic,graph,world_rank);


  	MPI_Finalize();
	return 0;
}
