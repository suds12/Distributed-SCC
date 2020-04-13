#pragma once
#include <iostream>
#include <fstream>
#include <vector>
//#include "basic.hpp"

#include <unordered_set>
using namespace std;


void read_partitions(char *argv[], Basic& basic, Graph& graph)
{
	int vertex=0, part;
	unordered_set<int> :: iterator itr;

	ifstream file4 (argv[6]); if (!file4.is_open() ) { cout<<"INPUT ERROR:: Could not open file\n";}

	
	while(file4 >> part)
	{
		basic.partition_of_vertex.insert({vertex, part});
		vertex++;
	}

}

void read_graph(char *argv[], Basic &basic, Graph& graph, int world_rank)
{
	int vertex, temp=0, edge_count=0, v1,v2;
	

	ifstream file1 (argv[1]); if (!file1.is_open() ) { cout<<"INPUT ERROR:: Could not open file\n";}

	while(file1 >> vertex)
	{
		if(temp == 0 )    // Reading vertex1 of edge
		{
			v1=vertex;
			temp++;
			continue;
		}
		if(temp == 1)	// Reading vertex2 of edge
		{
			v2=vertex;
			//cout<<v1<<" "<<v2<<" "<<partition_of_vertex.at(v1)<<" "<<partition_of_vertex.at(v2)<<"\n";

			if(basic.partition_of_vertex.at(v1) != basic.partition_of_vertex.at(v2))  //Edge across partitions
			{
				//cout<<world_rank<<endl;
				//Here we allocate an edge to the two processes that holds the vertices
				if(world_rank == basic.partition_of_vertex.at(v1) or world_rank == basic.partition_of_vertex.at(v2))
				{
					vector<int> edge;
					edge.push_back(v1);
					edge.push_back(v2);
					basic.allocated_graph.push_back(edge);

					boost::add_edge (v1, v2, graph);  //boost graph

					//mark mirror nodes
					if(world_rank == basic.partition_of_vertex.at(v1))
						basic.mirror_vertices.insert(v2);
					else if(world_rank == basic.partition_of_vertex.at(v2))
						basic.mirror_vertices.insert(v1);

				}
									
			}
			else//Edge within the same partition. 
			{

				//Here we allocate an edge only to the process that holds both the vertices
				if(world_rank == basic.partition_of_vertex.at(v1))
				{

					vector<int> edge;
					edge.push_back(v1);
					edge.push_back(v2);
					basic.allocated_graph.push_back(edge);

					boost::add_edge (v1, v2, graph);   //boost graph
				}

			}
			temp=2;
			continue;
		}
		if(temp == 2)		//Skip over the weight column
		{
			temp=0;
			continue;
		}

	}

	local_size=boost::num_vertices (graph);
	basic.local_scc.reserve(local_size);

	

}

void display(Basic &basic, Graph &graph, int world_rank)
{
	// for(int i=0;i<np;i++)
	// {
	// 	// cout<<"\n"<<i<<" : ";
	// 	// for(unordered_set<int> :: iterator it = mirror_vertices[i].begin(); it != mirror_vertices[i].end();++it)
	// 	// {
	// 	// 	cout<<*it;
	// 	// }
	// }
	vector< vector<int> >::iterator row;
	vector<int>::iterator col;
	//unordered_set<int>::iterator it;

	//Display allocated graph for specific partition
	// if(world_rank==0)
	// {
	// 	for(row=basic.allocated_graph.begin(); row<basic.allocated_graph.end(); row++)
	// 	{
	// 		cout<<endl;
	// 		for(col = row->begin(); col != row->end(); col++)
	// 		{
	// 			cout<<*col<<" ";
	// 		}
	// 	}
	// }

	//Display mirrors for specific partition
	// if(world_rank==0)
	// {
	// 	for(auto it=basic.mirror_vertices.begin(); it!=basic.mirror_vertices.end(); it++)
	// 		cout<<*it<<" ";

	// }

	//Display local SCC
	for (size_t i = 0; i < boost::num_vertices (graph); ++i)
    	cout << basic.local_scc[i] << " ";
	
}




