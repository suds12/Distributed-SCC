
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

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#define height 3
#define width 5
#define np 3

#define DELETE_1DARRAY(x) if (x!=nullptr) delete[] x

class Basic
{
public:
	vector<int> input_graph;
	// friend class boost::serialization::access;
 //    template<class Archive>
 //    void serialize(Archive & ar, const unsigned int version)
 //    {
 //        // Simply list all the fields to be serialized/deserialized.
 //        ar & input_graph;
 //    }

	int edge_count=0;
	unordered_set<int> nodes;
	map<int, int> partition_of_vertex; //Hashmap of partition id for each vertex
	map<int, int> init_scc_of_vertex; //Hashmap of initial scc id for each vertex. This is read from sccmap file.
	unordered_set<int> border_vertices;  //Hashset of border vertices. Each process maintains its own.
	map<int, vector<int>> border_out_vertices; //Hashmap that stores the vertices a specific border vertex has outgoing edges to. Each int is paired with a vector of connecting vertices
	map<int, vector<int>> border_in_vertices; // Hashmap that stores the vertices a specific border vertex has incoming edges from
	vector<vector<int>> allocated_graph; //Edge list of partitioned graph. Each process maintains its own
	vector<vector<int>> allocated_changes; //Edge list of changes. Each process maintains its own
	//vector<set<int>> local_scc;
	vector<int>local_scc;  //Vector of component id. Each index corresponds to global vertex id. Each process maintains its own. Hence we need to remove vertices indices it does not own
	vector<unordered_set<int>>temp_scc;
	vector<unordered_set<int>>l_scc;
	//unordered_set<int> relevant_vertices; //Hash set of vertices and mirror vertices. Each process maintains its own. Used for removing irrelevent vertices from local_scc
	vector<int> intersection_set; 
	vector<vector<int>>merge_detail; 
	unordered_map<int, int> parent_scc; //Used for creating SCC on disjoint sets using union find
	// int **border_matrix;
	// int **out_matrix;
	//int border_matrix[height][width]; //2d matrix of number of local SCC with each local SCC containing its respective border vertices. Needed for forming the meta graph
    //int out_matrix[height][width];  ////2d matrix of number of local SCC with each local SCC containing the vertices from other partitions that connects to the respective border vertex
    //COO representation of border_matrix
    int *border_row=nullptr;
    int *border_col=nullptr;
    int *border_value=nullptr;
    int *border_combined=nullptr;
    int nnz_capacity=0;
    int index=0;
    //COO representation of out_matrix
    int *out_row=nullptr;
    int *out_col=nullptr;
    int *out_value=nullptr;
    int *out_combined=nullptr;
    int out_nnz_capacity=0;
    int out_index=0;

    int *global_out_combined=nullptr; //Combined array of all out COOs at the root
    int *global_border_combined=nullptr; //Combined array of all border COOs at the root
    int sizeof_borders=0; //Total num of nodes in global_border_combined. Maintained ar root
    int sizeof_outs=0; //Total num of nodes in global_out_combined. maintained at root

    int global_border_matrix[height * np][width]; //Stored only in the root. Contains border matrices from all partitions stacked on top of each other.
    int global_out_matrix[height * np][width]; //Similarly for out_matrix
    map<int,int>global_border_map; //key=border vertex, val=local scc_id(made unique by adding with task_modifier)
    vector<int> global_scc; //Stores the scc result for meta graph.  
    int *global_result; //array format for global_scc to send back to every. Can probably be removed as this is unnecessary conversion
    int *local_result;
    int total_border_count;

	unordered_set<int> meta_nodes; //A set maintained in root process that holds the list of meta nodes


	int iteration;
	int nrows; //Number of SCCs
	int ncols;  //Size of biggest SCC(only borders)

	void alloc_2d_init(int rows, int cols);

	Basic()
	{
		//Definitely not the optimal way of doing it. Should work on improving this
        int nrows = height;  //num of local SCC. Set it to appropriate vale
        int ncols = width;  //Max size of borders of SCC
        edge_count = 0;
		// int** border_matrix = new int*[nrows];
		// int** out_matrix = new int*[nrows];
		// for(int i = 0; i < nrows; ++i)
		// {
		//     border_matrix[i] = new int[ncols];
		//     out_matrix[i] = new int[ncols];
		// }

		// memset(arr, 0, (10*20*) * (sizeof *arr));
		sizeof_borders = 0;
		sizeof_outs = 0;
		total_border_count = 0;
		//memset(border_matrix, -1, sizeof(int)*nrows*ncols);
		//memset(out_matrix, -1, sizeof(int)*nrows*ncols);
	}
	~Basic()
	{
		DELETE_1DARRAY(border_row);
		DELETE_1DARRAY(border_col);
		DELETE_1DARRAY(border_value);
		DELETE_1DARRAY(border_combined);
		DELETE_1DARRAY(global_border_combined);

		DELETE_1DARRAY(out_row);
		DELETE_1DARRAY(out_col);
		DELETE_1DARRAY(out_value);
		DELETE_1DARRAY(out_combined);
		DELETE_1DARRAY(global_out_combined);

		// This causes a crash with signal 6
		//DELETE_1DARRAY(global_result);
		DELETE_1DARRAY(local_result);

	}



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


