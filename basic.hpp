#pragma once
#include <unordered_set>
#include <set>
#include <map> 
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
	vector<unordered_set<int>>l_scc;
	unordered_set<int> relevant_vertices;
	vector<int> intersection_set;


};

typedef
  boost::adjacency_list<
    boost::vecS            // edge list
  , boost::vecS            // vertex list
  , boost::undirectedS     // directedness
  , float                  // property associated with vertices
  >
Graph;

