
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
	int* meta_nodes; //array of meta nodes
	
	

};


void main_1();
void cuda_h2d(int* src, int* dst, int size);
void create_MN_vector(int* meta_nodes, int size);

void deallocate_device_mem(int* ptr);
void cuda_sync();
//void unpack_bcast_gpu(Basic& basic, int world_rank, int world_size);;

