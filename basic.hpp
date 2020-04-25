
/*This file consists of the Basic class definition. 
Basic holds all the variables needed for SCC and the object reference is passed around functions*/
#pragma once
#include <unordered_set>
#include <set>
#include <map> 
#include <unordered_map> 

#include <vector>
#include <boost/graph/adjacency_list.hpp>



class Basic
{
public:
	int np; //number of partitions
	map<int, int> partition_of_vertex; //Hashmap of partition id for each vertex
	map<int, int> init_scc_of_vertex; //Hashmap of initial scc id for each vertex. This is read from sccmap file.
	unordered_set<int> border_vertices;  //Hashset of border vertices. Each process maintains its own.
	map<int, vector<int>> border_out_vertices; //Hashmap that stores the vertices a specific border vertex has outgoing edges to
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
	int** detail; //2D array with each row holding the intersections of mirror vertices and a single SCC. The root vertex(some random vertex from that SCC) is stored on pos 0 of each row. We send this using MPI to process 0 for merging. 
	int iteration;
	int nrows; //Number of SCCs
	int ncols;  //Size of biggest SCC

	void alloc_2d_init(int rows, int cols);

};

void Basic::alloc_2d_init(int rows, int cols)
	{
	    int *data = (int *)malloc(rows*cols*sizeof(int));
	    int **array= (int **)malloc(rows*sizeof(int*));
	    for (int i=0; i<rows; i++)
	        array[i] = &(data[cols*i]);

	    detail=array;
	}

typedef
  boost::adjacency_list<
    boost::vecS            // edge list
  , boost::vecS            // vertex list
  , boost::directedS     // directedness
  , float                  // property associated with vertices
  >
Graph;

