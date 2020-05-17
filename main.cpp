#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include <map>
#include "merge.hpp"
#include "basic.hpp"
//#include "main_code.cpp"
#include "update.cpp"
#include "reader.hpp"

#ifdef HAVE_PETSC
// Debugging support 
#include <petscsys.h>
#endif

using namespace std;
//Global variables
vector<set <int> > sccSets;
int world_rank, world_size, local_size;



int main(int argc, char *argv[])
{
    
    //-----------------------------------
	// Initialize the MPI environment
#ifdef HAVE_PETSC
    PetscErrorCode ierr;
    PetscInitialize( &argc, &argv, 0, 0 );
    ierr = PetscPrintf( PETSC_COMM_WORLD, "Hello World\n" );
    CHKERRQ(ierr);
#else
    MPI_Init(NULL, NULL);
#endif
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
    cout<<"reading partition"<<endl;
    read_partitions(argv,basic,graph);
    cout<<"reading graph"<<endl;
    read_graph(argv,basic,graph,world_rank);
    cout<<"reading changes"<<endl;
    read_changes(argv,basic,changes,graph,world_rank);
    cout<<"reading sccmap"<<endl;
    read_sccmap(argv,basic,world_rank);
    cout<<"Performing initial SCC"<<endl;
    perform_scc(argv,basic,graph,world_rank);
    cout<<"make meta vertex"<<endl;
    make_meta(argv,basic,graph,world_rank);
    cout<<"send meta vertex"<<endl;
    send_meta(argv,basic,world_rank);
    if(world_rank==0)
    {
        make_meta_graph(argv,basic,meta_graph,world_rank);
        display(basic,graph,world_rank);
        recompute_scc(basic,meta_graph,world_rank);
        create_result(basic,meta_graph,world_rank);

    }
    //scatter_global(basic,meta_graph,world_rank);



    //display(basic,graph,world_rank);
#ifdef HAVE_PETSC
    PetscFinalize();
#else
    MPI_Finalize();
#endif
    return 0;
}
