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
	unordered_set<int> mirror_vertices;  //vector of sets. Each row of vector is a set of mirror vertices for that partition
	vector<vector<int>> allocated_graph;
	//vector<set<int>> local_scc;
	vector<int>local_scc;
	vector<unordered_set<int>>temp_scc;
	vector<unordered_set<int>>l_scc;
	unordered_set<int> relevant_vertices;
	vector<int> intersection_set;
	vector<vector<int>>merge_detail;
	unordered_map<int, int> parent_scc; 
	int** detail;
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
  , boost::undirectedS     // directedness
  , float                  // property associated with vertices
  >
Graph;

