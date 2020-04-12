#pragma once
#include <unordered_set>
#include <set>
#include <map> 
#include <vector>


class Basic
{
public:
	int np; //number of partitions
	map<int, int> partition_of_vertex; //Hashmap of partition id for each vertex
	unordered_set<int> mirror_vertices;  //vector of sets. Each row of vector is a set of mirror vertices for that partition
	vector<vector<int>> allocated_graph;
	vector<set<int>> local_scc;
	

};