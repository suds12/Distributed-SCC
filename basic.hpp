
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



class Basic
{
public:
	int np; //number of partitions
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
	int border_matrix[10][10];
	int out_matrix[10][10];
	int global_border_matrix[30][10];
	int global_out_matrix[30][10];
	vector<pair<int, unordered_set<int>>> global_border_vector; //
	vector<int> global_scc;


	int iteration;
	int nrows; //Number of SCCs
	int ncols;  //Size of biggest SCC(only borders)

	void alloc_2d_init(int rows, int cols);

	Basic()
	{
		//Definitely not the optimal way of doing it. Should work on improving this
		int nrows = 10;  //num of local SCC. Set it to appropriate vale
		int ncols = 10;  //Max size of borders of SCC
		// int** border_matrix = new int*[nrows];
		// int** out_matrix = new int*[nrows];
		// for(int i = 0; i < nrows; ++i)
		// {
		//     border_matrix[i] = new int[ncols];
		//     out_matrix[i] = new int[ncols];
		// }

		// memset(arr, 0, (10*20*) * (sizeof *arr));
		memset(border_matrix, -1, sizeof(int)*nrows*ncols);
		memset(out_matrix, -1, sizeof(int)*nrows*ncols);


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


