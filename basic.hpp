
/*This file consists of the Basic class definition. 
Basic holds all the variables needed for SCC and the object reference is passed around functions*/
#pragma once
#include <unordered_set>
#include <set>
#include <map> 
#include <unordered_map> 
#include <utility> 
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/functional/hash.hpp>
#define c_height 3
#define c_width 5
#define np 3


class Basic
{
public:
	map<int, int> partition_of_vertex; //Hashmap of partition id for each vertex
	map<int, int> init_scc_of_vertex; //Hashmap of initial scc id for each vertex. This is read from sccmap file.
	unordered_set<int> border_vertices;  //Hashset of border vertices. Each process maintains its own.
	unordered_set<int> meta_nodes;  //Hashset of global SCC IDs. Each process maintains its own.
	map<int, vector<int>> border_out_vertices; //Hashmap that stores the vertices a specific border vertex has outgoing edges to. Each int is paired with a vector of connecting vertices
	map<int, vector<int>> border_in_vertices; // Hashmap that stores the vertices a specific border vertex has incoming edges from
	vector<int> allocated_vertices; //vector of vertices at each partition. Order matters for mapping vertex to scc
	vector<vector<int>> allocated_graph; //Edge list of partitioned graph. Each process maintains its own
	//vector<vector<int>> allocated_changes; //Edge list of changes. Each process maintains its own
	//vector<set<int>> local_scc;
	vector<int>local_scc;  //Vector of component id. Each index corresponds to global vertex id. Each process maintains its own. Hence we need to remove vertices indices it does not own
	vector<unordered_set<int>>temp_scc;
	vector<unordered_set<int>>l_scc;
	map<int,int> local_scc_map; //Hashmap of vertex ID to local SCC ID
	unordered_set<int> target_list; //Hash set of all reciever PIDs. EAch process maintains a list PIDs that has a connection with it through border vertices.
	map<int, unordered_set<int>>borders_in_of_scc;  //Hashmap of SCC ID to set of all incoming border vertices in that SCC 
	map<int, unordered_set<int>>borders_out_of_scc;  //Hashmap of SCC ID to set of all outgoing border vertices in that SCC
	int *probe_to_send; 
	int size_of_probe;
	int *all_probe; //array that stores all individual probes after allgatherv.
	int displacement; //size of the entire probe message after gather
	map<int,vector<unordered_set<int>>> meta_in_out;
	int* meta_graph_vector; //bit vector with each index being a 1to1 combination of all metanodes and value denoting if there is an edge between. For N metanodes, the size is N^2
	unordered_set<pair<int,int>, boost::hash<pair<int, int>> > partial_meta_edge; //Hashset of edges between metanodes within same process
	int* partial_ME_vector; //stores meta edges within processes. 
	int partial_ME_size; 
	int *all_internal; //array that stores all individual internal meta edges after allgatherv
	int internal_size; //size of all internal
	int *full_ME_vector; //Stores all meta edges after reduction
	int full_ME_vector_size;
	map<int, pair<int,int>> edge_index; //hasmap that matches index from ME_vector to pair of meta nodes(meta edge)
	vector<int>meta_scc; //scc of full meta graph


	//unordered_set<int> relevant_vertices; //Hash set of vertices and mirror vertices. Each process maintains its own. Used for removing irrelevent vertices from local_scc
	//vector<vector<int>>merge_detail; 
	//unordered_map<int, int> parent_scc; //Used for creating SCC on disjoint sets using union find
	
	// int **border_matrix;
	// int **out_matrix;
	//vector<vector<int>> border_matrix; //2d matrix of number of local SCC with each local SCC containing its respective border vertices. Needed for forming the meta graph
    //int out_matrix[c_height][c_width];  ////2d matrix of number of local SCC with each local SCC containing the vertices from other partitions that connects to the respective border vertex
    //vector<vector<int>> global_border_matrix;
    //int global_border_matrix[c_height * np][c_width]; //Stored only in the root. Contains border matrices from all partitions stacked on top of each other.
 //    int global_out_matrix[c_height * np][c_width]; //Similarly for out_matrix
 //    vector<pair<int, unordered_set<int>>> global_border_vector; //
 //    vector<int> global_scc;
 //    int global_result[1000];
 //    int local_result[c_height * np];
 //    int height;
 //    int width;


	// unordered_set<int> meta_nodes; //A set maintained in root process that holds the list of meta nodes


	// int iteration;
	// int nrows; //Number of SCCs
	// int ncols;  //Size of biggest SCC(only borders)

	// void alloc_2d_init(int rows, int cols);
	// void resize_2d_array(int **array, int height, int width);

	// Basic()
	// {
	// 	//Definitely not the optimal way of doing it. Should work on improving this
 //        int nrows = height;  //num of local SCC. Set it to appropriate vale
 //        int ncols = width;  //Max size of borders of SCC
	// 	// int** border_matrix = new int*[nrows];
	// 	// int** out_matrix = new int*[nrows];
	// 	// for(int i = 0; i < nrows; ++i)
	// 	// {
	// 	//     border_matrix[i] = new int[ncols];
	// 	//     out_matrix[i] = new int[ncols];
	// 	// }

	// 	// memset(arr, 0, (10*20*) * (sizeof *arr));
	// 	//memset(border_matrix, -1, sizeof(int)*nrows*ncols);
	// 	//memset(out_matrix, -1, sizeof(int)*nrows*ncols);


	// }



};




typedef
  boost::adjacency_list<
    boost::vecS            // edge list
  , boost::vecS            // vertex list
  , boost::directedS     // directedness
  , float                  // property associated with vertices
  >
Graph;

typedef
  boost::adjacency_list<
    boost::vecS            // edge list
  , boost::vecS            // vertex list
  , boost::directedS     // directedness
  , float                  // property associated with vertices
  >
MetaGraph;


