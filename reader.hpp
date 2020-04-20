#pragma once
#include <iostream>
#include <fstream>
#include <vector>
//#include "basic.hpp"

#include <unordered_set>

using namespace std;

//This function reads the partition id for each vertex from the partition file. The global_SCC disjoint set is also allocated here. Each
void read_partitions(char *argv[], Basic& basic, Graph& graph)
{
	int vertex=0, part;
	unordered_set<int> :: iterator itr;

	ifstream file4 (argv[6]); if (!file4.is_open() ) { cout<<"INPUT ERROR:: Could not open file 4\n";}

	
	while(file4 >> part)
	{
		//Create initial disjoint set only on root process
		if(world_rank==0)
			global_scc.make_set(vertex); 

		basic.partition_of_vertex.insert({vertex, part});  //Use hashmaps to store which partition each vertex belongs to
		vertex++;
	}

}

//Function for reading the graph. Each process reads only the edges it is allocated by looking up at the partition hash map
void read_graph(char *argv[], Basic &basic, Graph& graph, int world_rank)
{
	ofstream grap("orkut_ce" + std::to_string(world_rank) + ".txt");	
	int vertex, temp=0, edge_count=0, v1,v2;
	

	ifstream file1 (argv[1]); if (!file1.is_open() ) { cout<<"INPUT ERROR:: Could not open file 1 "<<world_rank;}

	while(file1 >> vertex) 
	{
		if(temp == 0 )    // Reading vertex1 of edge
		{
			v1=vertex;
			grap<<v1<<" ";
			temp++;
			continue;
		}
		if(temp == 1)	// Reading vertex2 of edge
		{
			v2=vertex;
			grap<<v2<<endl;
			if(basic.partition_of_vertex.at(v1) != basic.partition_of_vertex.at(v2))  //Edge across partitions
			{
				//Here we allocate an edge to the two processes that holds the vertices
				if(world_rank == basic.partition_of_vertex.at(v1) or world_rank == basic.partition_of_vertex.at(v2))
				{
					vector<int> edge;
					edge.push_back(v1);
					edge.push_back(v2);
					basic.allocated_graph.push_back(edge);

					boost::add_edge (v1, v2, graph);  //Storing in boost ajacency list. This is a different structure from allocated_graph[][].
					//Additional bookeeping. Don't time
					basic.relevant_vertices.insert(v1);
					basic.relevant_vertices.insert(v2);


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
					//Additional booking. Don't time
					basic.relevant_vertices.insert(v1);
					basic.relevant_vertices.insert(v2);

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

void merge_ds(char *argv[], Basic &basic, Graph& graph, int world_rank)
{
	int vertex, temp=0, id, X;
	basic.iteration=0;
	ifstream file2 (argv[2]); if (!file2.is_open() ) { cout<<"INPUT ERROR:: Could not open file 2\n";}

	if(world_rank == 0)
	{
		if(basic.iteration==0)
		{
			while(file2 >> X)
			{
				if(temp==0) //Reading vertex
				{
					vertex=X;
					temp++;
					continue;
				}
				if(temp==1) //Reading SCC map
				{
					if(basic.parent_scc.find(X) == basic.parent_scc.end())
					{
						basic.parent_scc.insert(pair<int,int>(X,vertex));
					}
					else
					{
						//cout<<basic.parent_scc.at(X)<<" "<<vertex<<endl;
						global_scc.union_set(basic.parent_scc.at(X), vertex);
					}

					temp=0;
					continue;
				}
			
			}
		}

		
		//int *details;
		int MAX_NUMBERS=100;
		int* details = new int[MAX_NUMBERS];
		//MPI_Request request;
		//MPI_Status status;

		// for(i=0;i<argv[7];i++)
		// {
			//MPI_Irecv(details, MAX_NUMBERS, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
			//MPI_Wait(&request, &status);
			// for(int i=0;i<20;i++)
			// {
			// 	cout<<"*"<<i;
			// }
		// //The union operation for disjoint sets
			// int parent;
			// for(int i=0;i<max_rows;i++)
			// {
				// 	parent=details[i][0];
				// 	for(int j=0;j<max_cols;j++)
				// 	{
				// 		global_scc.union_set(parent,details[i][j]);
				// 	}
			// }
		//}


	}

}

void display(Basic &basic, Graph &graph, int world_rank)
{
	ofstream scc_dump("dump/file_no_" + std::to_string(world_rank) + ".txt");
	ofstream rel_dump("dump/rel_" + std::to_string(world_rank) + ".txt");
	ofstream inter_dump("dump/int_" + std::to_string(world_rank) + ".txt");
	ofstream mirror_dump("dump/mir_" + std::to_string(world_rank) + ".txt");
	//ofstream l_scc_dump("dump/l_scc_" + std::to_string(world_rank) + ".txt");

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
	if(world_rank==0)
	{
		for(auto it=basic.mirror_vertices.begin(); it!=basic.mirror_vertices.end(); it++)
		{
			mirror_dump<<*it<<" ";
		}
	}

	


	//Display local SCC
	// for (size_t i = 0; i < boost::num_vertices (graph); ++i)
 //    	cout << basic.local_scc[i] << " ";
	//Display local merge details
	// for(int it=0;it<basic.l_scc.size();it++)
	// {
	// 	l_scc_dump<<it<<" : ";
	// 	for(auto itr=basic.l_scc[it].begin(); itr!=basic.l_scc[it].end();itr++)
	// 		l_scc_dump<<*itr<<" ";
	// 	l_scc_dump<<endl;
	// }
	//Display local merge details
	for(int it=0;it<basic.merge_detail.size();it++)
	{
		scc_dump<<it<<" : ";
		for(auto itr=basic.merge_detail[it].begin(); itr!=basic.merge_detail[it].end();itr++)
			scc_dump<<*itr<<" ";
		scc_dump<<endl;
	}
	//Display relevant vertices
	for(auto it=basic.relevant_vertices.begin(); it!=basic.relevant_vertices.end(); it++)
	{
		rel_dump<<*it<<" ";
	}
	//Display intersection
	// for(auto it=basic.intersection_set.begin(); it!=basic.intersection_set.end(); it++)
	// {
	// 	inter_dump<<*it<<" ";
	// }

	
}




