#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include <map>
#include <chrono> 
#include <ctime>

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
     std::chrono::time_point<std::chrono::system_clock> start, end;
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
    Graph graph;
    Graph changes;
    MetaGraph meta_graph;
    cout<<"reading partition from rank "<<world_rank<<endl;
    read_partitions(argv,basic,graph);

    cout<<"reading graph from rank "<<world_rank<<endl;
    read_graph(argv,basic,graph,world_rank);

    cout<<"reading changes from rank "<<world_rank<<endl;
    read_changes(argv,basic,changes,graph,world_rank);

    //serialize_basic(basic);

    // cout<<"reading sccmap from rank "<<world_rank<<endl;
    // read_sccmap(argv,basic,world_rank);

    //This barrier is kept so timing can start accurately. 
    //It shouldn't affect the overall execution but small tasks will progress until mpi_gather and wait there while bigger tasks are still reading.
    cout<<"Rank "<<world_rank<<" waiting for other tasks to finish reading"<<endl;
    MPI_Barrier(MPI_COMM_WORLD);

    //start timer
    start = std::chrono::system_clock::now();

    cout<<"Performing initial SCC from rank "<<world_rank<<endl;
    perform_scc(argv,basic,graph,world_rank);

    
    

    cout<<"Initialisizng COO from rank "<<world_rank<<" for "<<basic.total_border_count<<" border vertices"<<endl;
    init_coo(basic);

    cout<<"make meta vertex from rank "<<world_rank<<endl;
    make_meta(argv,basic,graph,world_rank);

    cout<<"preparing to send from rank "<<world_rank<<endl;
    prepare_to_send(basic,world_rank);

    cout<<"send meta vertex from rank "<<world_rank<<endl;
    send_meta(argv,basic,world_rank,world_size);
    if(world_rank==0)
    {
        cout<<"make meta graph from rank "<<world_rank<<endl;
        make_meta_graph(argv,basic,meta_graph,world_rank);

        cout<<"recompute SCC from rank "<<world_rank<<endl;
        recompute_scc(basic,meta_graph,world_rank);

        cout<<"creating result from rank "<<world_rank<<endl;
        create_result(basic,meta_graph,world_rank);

    }
    cout<<"scatter results from rank "<<world_rank<<endl;
    scatter_global(basic,meta_graph,world_rank);

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    if(world_rank ==0)
    {
        std::cout << "finished computation at " << std::ctime(&end_time) 
                  << "elapsed time: " << elapsed_seconds.count() << "s\n";
    }



    // display(basic,graph,world_rank);
  	MPI_Finalize();
	return 0;
}