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

#if DEBUG
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    printf("PID %d on %s ready for attach\n", getpid(), hostname);
    fflush(stdout);
#endif

    //shared_scc(argc, argv);
    //Reader r1;
  

    Basic basic;
    Graph graph;
    Graph changes;
    MetaGraph meta_graph;

    cout<<"reading partition from rank "<<world_rank<<endl;
    read_partitions(argv,basic,graph);

    cout<<"reading graph from rank "<<world_rank<<endl;
    read_graph(argv,basic,graph,world_rank);

    cout<<"reading changes from rank "<<world_rank<<endl;
    read_changes(argv,basic,changes,graph,world_rank);

    cout<<"reading sccmap from rank "<<world_rank<<endl;
    read_sccmap(argv,basic,world_rank);

    cout<<"Performing initial SCC from rank "<<world_rank<<endl;
    perform_scc(argv,basic,graph,world_rank);

    cout<<"Initializing meta vertices from rank "<<world_rank<<endl;
    init_meta(basic);

    cout<<"make meta vertex from rank "<<world_rank<<endl;
    make_meta(argv,basic,graph,world_rank);

    cout<<"Padding meta vertices from rank "<<world_rank<<endl;
    padding_meta(basic);

    cout<<"send meta vertex from rank "<<world_rank<<endl;
    send_meta(argv,basic,world_rank);
    if(world_rank==0)
    {
        // cout<<"making meta graph from rank "<<world_rank<<endl;
        // make_meta_graph(argv,basic,meta_graph,world_rank);

        //display(basic,graph,world_rank);

        // cout<<"Recomputing SCC from rank "<<world_rank<<endl;
        // recompute_scc(basic,meta_graph,world_rank);

        // cout<<"Creating results from rank "<<world_rank<<endl;
        // create_result(basic,meta_graph,world_rank);

    }
    //scatter_global(basic,meta_graph,world_rank);



    //display(basic,graph,world_rank);
  	MPI_Finalize();
	return 0;
}
