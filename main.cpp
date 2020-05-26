#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include <map>
#include <chrono>

#include "utils.hpp"
#include "basic.hpp"
#include "update.hpp"
#include "reader.hpp"


static char help[] = "Computes distributed-memory parallel strongly connected components for dynamic graphs.\n\n";
int Basic::np = 1;

#ifdef HAVE_PETSC
// For profiling
PetscLogEvent event_read_input, event_local_scc, event_create_meta, event_init_coo, event_make_meta_par, event_make_meta_seq;
PetscLogStage stage_init, stage_update;
#endif

int main(int argc, char *argv[])
{
    int world_rank=0, world_size=1;
    boost::program_options::variables_map options;
    //process_options(argc, argv, options);
    std::chrono::time_point<std::chrono::system_clock> start, end;
    //-----------------------------------
	// Initialize the MPI environment
#ifdef HAVE_PETSC
    PetscInitialize(&argc,&argv,NULL,help);

    PetscLogStageRegister("Initialization", &stage_init); 
    PetscLogStageRegister("Update", &stage_update); 
    PetscLogEventRegister("Read input",0,&event_read_input); 
    PetscLogEventRegister("Initial SCC",0,&event_local_scc); 
    PetscLogEventRegister("Init COO",0,&event_init_coo);
    PetscLogEventRegister("Metagraph par",0,&event_make_meta_par);
    PetscLogEventRegister("Metagraph seq",0,&event_make_meta_seq);
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

    cout<<"Initializing..."<<endl;
  
    Basic basic(world_size,world_rank);
    Graph graph;
    Graph changes;
    MetaGraph meta_graph;

    log_stage_begin(stage_init);
    log_begin(event_read_input);

    cout<<"reading partition from rank "<<world_rank<<endl;
    read_partitions(argv,basic,graph,world_rank);

    cout<<"reading graph from rank "<<world_rank<<endl;
    read_graph(argv,basic,graph,world_rank);

    cout<<"reading changes from rank "<<world_rank<<endl;
    read_changes(argv,basic,changes,graph,world_rank);

    log_end(event_read_input);
    log_stage_end();

    //serialize_basic(basic, world_rank);

    // cout<<"reading sccmap from rank "<<world_rank<<endl;
    // read_sccmap(argv,basic,world_rank);

    //This barrier is kept so timing can start accurately. 
    //It shouldn't affect the overall execution but small tasks will progress until mpi_gather and wait there while bigger tasks are still reading.
    cout<<"Rank "<<world_rank<<" waiting for other tasks to finish reading"<<endl;
    MPI_Barrier(MPI_COMM_WORLD);

    //start timer
    cout<<"Performing initial SCC from rank "<<world_rank<<endl;
    start = std::chrono::system_clock::now();
    log_stage_begin(stage_update);

    log_begin(event_local_scc);

    cout <<"Trying Perfom_SCC"<<"\n";
    perform_scc(argv,basic,graph,world_rank);

    log_end(event_local_scc);

    if (DEBUG) cout<<"Initializing COO from rank "<<world_rank<<" for "<<basic.total_border_count<<" border vertices"<<endl;

    log_begin(event_init_coo);
    init_coo(basic);
    log_end(event_init_coo);

    if (DEBUG) cout<<"make meta vertex from rank "<<world_rank<<endl;  // should not have system calls in timed regions!
    log_begin(event_make_meta_par);
    make_meta(argv,basic,graph,world_rank);

    if (DEBUG) cout<<"preparing to send from rank "<<world_rank<<endl;
    prepare_to_send(basic,world_rank);

    if (DEBUG) cout<<"send meta vertex from rank "<<world_rank<<endl;
    //send_meta(argv,basic,world_rank,world_size);
    update_meta_graph(argv,basic,meta_graph,world_rank,world_size);
    log_end(event_make_meta_par);

    if(world_rank==0)
    {
        //if (DEBUG) cout<<"make meta graph from rank "<<world_rank<<endl;
        //make_meta_graph(argv,basic,meta_graph,world_rank);

        //if (DEBUG) cout<<"recompute SCC from rank "<<world_rank<<endl;
        //recompute_scc(basic,meta_graph,world_rank);

        if (DEBUG) cout<<"creating result from rank "<<world_rank<<endl;
        create_result(basic,meta_graph,world_rank);

    }
    if (DEBUG) cout<<"scatter results from rank "<<world_rank<<endl;


    scatter_global(basic,meta_graph,world_rank);

    log_end(event_make_meta_seq);
    log_stage_end();
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    if(world_rank ==0)
    {
        std::cout << "finished computation at " << std::ctime(&end_time) 
                  << "elapsed time: " << elapsed_seconds.count() << "s\n";
    }


    // display(basic,graph,world_rank);
    MPI_Barrier(MPI_COMM_WORLD);

#ifdef HAVE_PETSC
    PetscFinalize();
#else
    MPI_Finalize();
#endif
    return 0;
}
