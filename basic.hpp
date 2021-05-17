
/*This file consists of the Basic class definition. 
Basic holds all the variables needed for SCC and the object reference is passed around functions*/
#pragma once
#include <unordered_set>
#include <set>
#include <map> 
#include <unordered_map> 
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/functional/hash.hpp>
#define c_height 3
#define c_width 5



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
	
	map<int,vector<unordered_set<int>>> meta_in_out;
	int* meta_graph_vector; //bit vector with each index being a 1to1 combination of all metanodes and value denoting if there is an edge between. For N metanodes, the size is N^2
	unordered_set< pair <int,int>, boost::hash< pair< int, int > > > partial_meta_edge; //Hashset of edges between metanodes within same process
	int* partial_ME_vector; //stores meta edges within processes. 
	int partial_ME_size; 

	int* all_meta_nodes; //Stores all the meta nodes
	int* all_meta_nodes_size; //stores number of external edges for respective meta nodes
	int* all_external; //stores all external edges in order of meta nodes
	int* all_internal; //stores all internal edges in node1 node2 format
	int size_amn[1];
	int size_e[1];
	int size_i[1];

	map<int, pair<int,int>> edge_index; //hasmap that matches index from ME_vector to pair of meta nodes(meta edge)
	vector<int>meta_scc; //scc of full meta graph

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


