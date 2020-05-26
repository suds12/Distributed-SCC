#ifndef DISTRIBUTED_SCC_UPDATE_HPP
#define DISTRIBUTED_SCC_UPDATE_HPP


void perform_scc(char *argv[], Basic& basic, Graph& graph, int world_rank);   //Shared memory scc
void init_coo(Basic& basic);
void make_meta(char *argv[], Basic& basic, Graph& graph, int world_rank);
void prepare_to_send(Basic& basic, int world_rank);
void send_meta(char *argv[], Basic& basic, int world_rank, int world_size);
void update_arrays(int local_size, int world_rank, int world_size, int *local_array, int **global_array, int *global_size,bool to_root=false,  string msg="");
void update_meta_graph(char *argv[], Basic& basic, MetaGraph& meta_graph, int world_rank, int world_size);
void make_meta_graph(char *argv[], Basic& basic, MetaGraph& meta_graph, int world_rank);
void recompute_scc(Basic& basic, MetaGraph& meta_graph, int world_rank);
void create_result(Basic& basic, MetaGraph& meta_graph, int world_rank);
void scatter_global(Basic& basic, MetaGraph& meta_graph, int world_rank);

#endif