#pragma once
#include <iostream>
#include <fstream>
#include <vector>
//#include "basic.hpp"

#include <unordered_set>
using namespace std;


void read_partitions(char *argv[], Basic& graph)
{
	int vertex=0, part;
	unordered_set<int> :: iterator itr;

	ifstream file4 (argv[6]); if (!file4.is_open() ) { cout<<"INPUT ERROR:: Could not open file\n";}

	
	while(file4 >> part)
	{
		graph.partition_of_vertex.insert({vertex, part});
		vertex++;
	}

}

void read_graph(char *argv[], Basic &graph, int world_rank)
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

			if(graph.partition_of_vertex.at(v1) != graph.partition_of_vertex.at(v2))  //Edge across partitions
			{
				//cout<<world_rank<<endl;
				//Here we allocate an edge to the two processes that holds the vertices
				if(world_rank == graph.partition_of_vertex.at(v1) or world_rank == graph.partition_of_vertex.at(v2))
				{
					vector<int> edge;
					edge.push_back(v1);
					edge.push_back(v2);
					graph.allocated_graph.push_back(edge);

					//mark mirror nodes
					if(world_rank == graph.partition_of_vertex.at(v1))
						graph.mirror_vertices.insert(v2);
					else if(world_rank == graph.partition_of_vertex.at(v2))
						graph.mirror_vertices.insert(v1);

				}
									
			}
			else//Edge within the same partition. 
			{

				//Here we allocate an edge only to the process that holds both the vertices
				if(world_rank == graph.partition_of_vertex.at(v1))
				{

					vector<int> edge;
					edge.push_back(v1);
					edge.push_back(v2);
					graph.allocated_graph.push_back(edge);
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

	

}

void display(Basic &graph, int world_rank)
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
	// if(world_rank==2)
	// {
	// 	for(row=allocated_graph.begin(); row<allocated_graph.end(); row++)
	// 	{
	// 		cout<<endl;
	// 		for(col = row->begin(); col != row->end(); col++)
	// 		{
	// 			cout<<*col<<" ";
	// 		}
	// 	}
	// }

	//Display mirrors for specific partition
	if(world_rank==0)
	{
		for(auto it=graph.mirror_vertices.begin(); it!=graph.mirror_vertices.end(); it++)
			cout<<*it<<" ";

	}
	
}




