
class DeviceFuncs
{
public:
	int* bcast_size;
	int* external_counts;
	int* internal_counts;
	int* external_displacements;
	int* internal_displacements;
	int* external_data;
	int* internal_data;

	int total_meta_nodes; // total number of meta nodes across all process
	int* all_meta_nodes; //array of meta nodes
	int* all_meta_nodes_size; //stores number of external edges for respective meta nodes
	int* all_external; //stores all external edges in order of meta nodes
	int* all_internal; //stores all internal edges in node1 node2 format
	int size_amn[1];
	int size_e[1];
	int size_i[1];	

};


void main_1();
void cuda_h2d(int* src, int* dst, int size);
void cuda_d2h(int* src, int* dst, int size);


void deallocate_device_mem(int* ptr);
void cuda_sync();
//void unpack_bcast_gpu(Basic& basic, int world_rank, int world_size);;

