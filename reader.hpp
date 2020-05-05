#pragma once
#include <iostream>
#include <fstream>
#include <vector>
//#include "basic.hpp"

#include <unordered_set>
#define chunk_height 10
#define chunk_width 10
#define num_partitions 3
#define root 0

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
	int vertex, temp=0, edge_count=0, v1,v2;
	

	ifstream file1 (argv[1]); if (!file1.is_open() ) { cout<<"INPUT ERROR:: Could not open file 1 "<<world_rank;}

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

					//store border vertices and store the vertices a specific border vertex has outgoing edges to. This is stored seperately and is only used for merging. Not included while performing local SCC.
					if(world_rank == basic.partition_of_vertex.at(v1))
					{
						if(basic.border_out_vertices.find(v1) == basic.border_out_vertices.end())//border vertex not yet added 
						{
							vector<int> borders;
							borders.push_back(v2);
							basic.border_out_vertices.insert({v1, borders});
						}
						else //border vertex already exists. Push oppopsite vertex to vector mapped with border vertex
						{
							basic.border_out_vertices[v1].push_back(v2);
						}
					}
					//Similarly, store borders of incoming edges in another hashmap
					if(world_rank == basic.partition_of_vertex.at(v2))
					{
						if(basic.border_in_vertices.find(v2) == basic.border_in_vertices.end())
						{
							vector<int> borders;
							borders.push_back(v1);
							basic.border_in_vertices.insert({v2, borders});
						}
						else //border vertex already exists. Push oppopsite vertex to vector mapped with border vertex
						{
							basic.border_in_vertices[v2].push_back(v1);
						}

					}

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

void read_changes(char *argv[], Basic &basic, Graph& changes, Graph& graph, int world_rank)
{
	int vertex, temp=0, edge_count=0, v1,v2;
	

	ifstream file3 (argv[3]); if (!file3.is_open() ) { cout<<"INPUT ERROR:: Could not open file 3 "<<world_rank;}

	while(file3 >> vertex) 
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
			if(basic.partition_of_vertex.at(v1) != basic.partition_of_vertex.at(v2))  //Edge across partitions
			{
				//Here we would not allocate that edge but just mark the vertices as border vertices at both the respective partitions.
				if(world_rank == basic.partition_of_vertex.at(v1) or world_rank == basic.partition_of_vertex.at(v2))
				{
					// vector<int> edge;
					// edge.push_back(v1);
					// edge.push_back(v2);
					// basic.allocated_changes.push_back(edge);

					// boost::add_edge (v1, v2, Change);  //Storing in boost ajacency list. This is a different structure from allocated_changes[][].

					//store border vertices and store the vertices a specific border vertex has outgoing edges to. This is stored seperately and is only used for merging. Not included while performing local SCC.
					if(world_rank == basic.partition_of_vertex.at(v1))
					{
						if(basic.border_out_vertices.find(v1) == basic.border_out_vertices.end())//border vertex not yet added 
						{
							vector<int> borders;
							borders.push_back(v2);
							basic.border_out_vertices.insert({v1, borders});
						}
						else //border vertex already exists. Push oppopsite vertex to vector mapped with border vertex
						{
							basic.border_out_vertices[v1].push_back(v2);
						}
					}
					//Similarly, store borders of incoming edges in another hashmap
					if(world_rank == basic.partition_of_vertex.at(v2))
					{
						if(basic.border_in_vertices.find(v2) == basic.border_in_vertices.end())
						{
							vector<int> borders;
							borders.push_back(v1);
							basic.border_in_vertices.insert({v2, borders});
						}
						else //border vertex already exists. Push oppopsite vertex to vector mapped with border vertex
						{
							basic.border_in_vertices[v2].push_back(v1);
						}

					}

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
					basic.allocated_changes.push_back(edge);

					boost::add_edge (v1, v2, changes);   //boost graph

					/*For now the changes are also added to the original graph as boost scc doesn't perform dynamic changes and it recomputes.
					When boost is replaced by the dynamic shared scc, I would feed the input as two seperate files for changes and input. 
					This line will be removed then*/  
					boost::add_edge (v1, v2, graph);  
					
				}

			}
			temp=2;
			continue;
		}
		if(temp == 2)//Only insertion for now. So assume this column is 1 and skip over. When deletion is introduced, we need to check this column first and act accordingly
		{
			temp=0;
			continue;
		}
	}
}

void read_sccmap(char *argv[], Basic &basic, int world_rank)
{
	int temp=0, vertex, X;
	ifstream file2 (argv[2]); if (!file2.is_open() ) { cout<<"INPUT ERROR:: Could not open file 2 ";}

	while(file2 >> X)
	{
		if(temp == 0) //Read vertex in column 1
		{
			vertex = X;
			temp=1;
			continue;
		}
		if(temp == 1) //Read its respective SCC in column 2 and store it in a hash map
		{
			basic.init_scc_of_vertex.insert({vertex, X}); 
			temp=0;
			continue;
		}
	}

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
	ofstream out_dump("dump/rel_" + std::to_string(world_rank) + ".txt");
	ofstream inter_dump("dump/int_" + std::to_string(world_rank) + ".txt");
	ofstream meta_dump("dump/mir_" + std::to_string(world_rank) + ".txt");
	ofstream l_scc_dump("dump/l_scc_" + std::to_string(world_rank) + ".txt");
	ofstream dump_bor("dump/global_matrix.txt");

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
	// 	for(auto it : border_out_vertices)
	// 	{
	// 		for(auto i : it.second)
	// 		{
	// 			mirror_dump<<basic.border_out_vertices[*it][*i]<<" ";
	// 		}
	// 	}
	// }
	

	//Display local SCC
	for (size_t i = 0; i < boost::num_vertices (graph); ++i)
	{
		if(basic.partition_of_vertex[i]==world_rank)
    		scc_dump << basic.local_scc[i] << " ";
	}
	//Display local merge details
	for(int it=0;it<basic.l_scc.size();it++)
	{
		l_scc_dump<<it<<" : ";
		for(auto itr=basic.l_scc[it].begin(); itr!=basic.l_scc[it].end();itr++)
			l_scc_dump<<*itr<<" ";
		l_scc_dump<<endl;
	}
	//Display border matrix
	for(int i =0; i<basic.l_scc.size();i++)
	{
		int j=0;
		meta_dump<<endl;
		while(basic.border_matrix[i][j] != -1)
		{
			meta_dump<<basic.border_matrix[i][j]<<" ";
			j++;
		}
	}

	//Display out matrix
	for(int i =0; i<basic.l_scc.size();i++)
	{
		int j=0;
		
		while(basic.out_matrix[i][j] != -1)
		{
			out_dump<<basic.out_matrix[i][j]<<" ";
			j++;
		}
		out_dump<<endl;
	}
	//display global border matrix
	if(world_rank == root)
	{
		for(int i=0;i<chunk_height * num_partitions;i++)
		{
			int j=0;
			while(basic.global_border_matrix[i][j] != -1)
			{
				dump_bor<<basic.global_border_matrix[i][j]<<" ";
				j++;
			}
			dump_bor<<endl;
		}
		
	}
	//Display relevant vertices
	// for(auto it=basic.relevant_vertices.begin(); it!=basic.relevant_vertices.end(); it++)
	// {
	// 	rel_dump<<*it<<" ";
	// }
	//Display intersection
	// for(auto it=basic.intersection_set.begin(); it!=basic.intersection_set.end(); it++)
	// {
	// 	inter_dump<<*it<<" ";
	// }

	
}




