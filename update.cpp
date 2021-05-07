#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <set>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/strong_components.hpp>
#include <vector>
#include "reader.hpp"

#define chunk_height 3
#define chunk_width 5
#define num_partitions 3
#define root 0
#define global_modifier 100
#define mailbox_displacement 100





void perform_scc(char *argv[], Basic& basic, Graph& graph, int world_rank)   //Shared memory scc
{
	/*
	Here we perform SCCs locall at each process and some extra bookeeping 
	*/
	int p = 1;
	int nodes=11;

	//Replace this with function call for shared SCC (From Sriram). Tha input parameters would be the allocated graph and SCC mapping. 
	size_t num_components = boost::strong_components (graph, &basic.local_scc[0]); //output to local_scc

	//Additional conversions. Don't time
	int temp=0;
	unordered_set<int> empty;
	for(int i=0;i<num_components;i++)
	{
		basic.temp_scc.push_back(empty);
	}
	for(int i=0;i<boost::num_vertices (graph);i++)
	{
		//cout<<"**"<<basic.relevant_vertices.count(i)<<" ";
		if(basic.partition_of_vertex[i]==world_rank)
		{
			basic.temp_scc[basic.local_scc[i]].insert(i);
		}
		
	}
	for(int i=0;i<basic.temp_scc.size();i++)
	{
		if(!basic.temp_scc[i].empty())
			basic.l_scc.push_back(basic.temp_scc[i]);
	}
	//-----------------
	//Store local scc in hash table. key =vertex id ; value = local scc id. Also create borders_of_scc(described in basic.hpp)
	for(int i=0;i<boost::num_vertices (graph);i++) 
	{
		if(basic.partition_of_vertex[i]==world_rank)
		{
			basic.local_scc_map.insert({i,basic.local_scc[i]});
			basic.meta_nodes.insert((world_rank * global_modifier) + basic.local_scc[i]);   //Store all global SCC IDs in a set for future use

			if(basic.border_in_vertices.find(i) != basic.border_in_vertices.end())
			{
				for(auto temp : basic.border_in_vertices.at(i))
				{
					basic.borders_in_of_scc[(world_rank * global_modifier) + basic.local_scc_map[i]].insert(temp);    //Inserting incoming border vertices into a hashset allocated for each global SCC.
				}
				
			}

			if(basic.border_out_vertices.find(i) != basic.border_out_vertices.end())  
			{
				for(auto temp : basic.border_out_vertices.at(i))
				{
					basic.borders_out_of_scc[(world_rank * global_modifier) + basic.local_scc_map[i]].insert(temp);    //Inserting outgoing border vertices into a hashset allocated for each global SCC.
				}
				
			}

		}
	}
	// if(world_rank == 2)
	// {
	// 	cout<<"----------";
	// 	for(auto itr : basic.local_scc_map)
	// 	{
	// 		cout<<itr.first<<" : "<<itr.second<<endl;
	// 	}
	// }
}

int* arr_resize(int* arr, int oldsize, int newsize)
{
	int *temp_arr = new int[newsize];
	memcpy( temp_arr, arr, oldsize * sizeof(int) );
	arr=temp_arr;
	delete[] temp_arr;

	return arr;
}

void create_partial_meta_graph(Basic& basic, int world_rank)
{
	//The aim here is to create meta edges without redundant edges between meta nodes and store them in an array format for communication. 
	//This could either be done by putting the edges(pair of meta nodes) in a hashset and them store them in an array by traversing the hashet or directly store them in an array by checking redundancies with IF statement. 
	//I don't know which is faster so need to analyse further. Going with the former for now.
	pair<int,int> edge;
	int index = 0;
	for(auto i : basic.allocated_graph)
	{
		edge.first = basic.local_scc[i[0]] + (world_rank * global_modifier);
		edge.second = basic.local_scc[i[1]] + (world_rank * global_modifier);
		basic.partial_meta_edge.insert(edge);
	}

	basic.partial_ME_vector = (int *)malloc(basic.partial_meta_edge.size() * 2 * sizeof(int));
	for(auto j : basic.partial_meta_edge)
	{
		basic.partial_ME_vector[index] = j.first;
		index++;
		basic.partial_ME_vector[index] = j.second;
		index++;
	}
	basic.partial_ME_size = index;

	
}



void prepare_to_send(Basic& basic, int world_rank)
{
	//We need to send the border vertices to the respected partitions of its connections
	basic.probe_to_send = arr_resize(basic.probe_to_send, 0, 100);
	basic.probe_to_send[0] = 1; //1 at index 0 indicates a probe message. REst of message starts index 1
	int index=1;
	for(auto itr : basic.border_out_vertices)
	{
		for(auto i : itr.second)
		{
			int local_scc_of_vertex = basic.local_scc_map[itr.first];
			int global_scc_val = (world_rank * global_modifier) + local_scc_of_vertex; 
			basic.probe_to_send[index] = global_scc_val;
			index++;
			basic.probe_to_send[index] = i;
			basic.target_list.insert(basic.partition_of_vertex[i]); 
			index++;
		}
		
	}
	basic.probe_to_send[index] = -1;
	index++;
	basic.size_of_probe = index;
	

	//test
	if(world_rank == 1)
	{
		// for(auto i : basic.target_list)
		// 	cout<<i<<" - ";
	}

}

void bcast_meta_nodes(Basic& basic, int world_rank, int world_size)
{
	/*
	Here everyone broadcasts their meta nodes and outgoing interprocess edges using allgatherv. 
	Individual messages(probes) are of the form:
	index[0] = meta node Id
	index[1] = number of incoming edges to meta node
	index[2] = number of outgoing edges from meta node
	index[3] to index[n] = vertex IDs of incoming edges followed by outgiong edges where n = index[1] + index[2]
	This repeats itself for all metanodes bringing total size of message to M*(n+3) where M = number of meta nodes
	*/

	int index=0;
	int* probe_meta_node;
	int* rbuf_size;  
	int* rbuf_data; 
	int* rbuf_internal;
	int probe_size[2];
	int* probe_counts;
	int* internal_counts;
	int* probe_displacements;
	int* internal_displacements;

	//probe_meta_node = arr_resize(probe_meta_node, 0, 100);
	probe_meta_node = (int *)malloc(100000 * sizeof(int));

	
	for(auto temp : basic.meta_nodes)
	{
		probe_meta_node[index] = temp;
		index++;
		probe_meta_node[index] = basic.borders_in_of_scc[temp].size();
		index++;
		probe_meta_node[index] = basic.borders_out_of_scc[temp].size();
		index++;

		if(basic.borders_in_of_scc[temp].size() != 0)
		{
			for(auto itr : basic.borders_in_of_scc[temp])
			{
				probe_meta_node[index] = itr;
				index++;
			}
		}
		
		if(basic.borders_out_of_scc[temp].size() != 0)
		{
			for(auto itr : basic.borders_out_of_scc[temp])
			{
				probe_meta_node[index] = itr;
				index++;
			}
		}
		

	}

	rbuf_size = (int *)malloc(world_size * 2 * sizeof(int));  //Buffer to hold sizes of both external and internal edges
	probe_size[0] = index;
	probe_size[1] = basic.partial_ME_size;
	
	MPI_Allgather( probe_size, 2, MPI_INT, rbuf_size, 2, MPI_INT, MPI_COMM_WORLD);   // Sending the size of each probe message to all processors. We need this to calculate displacements when using allgatherv

	
	probe_counts = (int *)malloc(world_size * sizeof(int));
	internal_counts = (int *)malloc(world_size * sizeof(int));
	probe_displacements = (int *)malloc(world_size*sizeof(int));
	internal_displacements = (int *)malloc(world_size*sizeof(int));

	int disp = 0;
	int i_disp = 0;
	int j=0;

	
	
	for(int i=0; i<world_size*2; i++)
	{
		probe_displacements[j] = disp;
		disp += rbuf_size[i];
		probe_counts[j] = rbuf_size[i];
		i++;
		internal_displacements[j] = i_disp;
		i_disp += rbuf_size[i];
		internal_counts[j] = rbuf_size[i];
		j++;
	}

	
	rbuf_data = (int *)malloc(disp*sizeof(int));
	MPI_Allgatherv(probe_meta_node, index, MPI_INT, rbuf_data, probe_counts, probe_displacements, MPI_INT, MPI_COMM_WORLD);

	basic.all_probe = rbuf_data;
	basic.displacement = disp;

	

	rbuf_internal = (int *)malloc(i_disp*sizeof(int));
	MPI_Allgatherv(basic.partial_ME_vector, basic.partial_ME_size, MPI_INT, rbuf_internal, internal_counts, internal_displacements, MPI_INT, MPI_COMM_WORLD);
	basic.all_internal = rbuf_internal;
	basic.internal_size = i_disp;

	// if(world_rank == 0)
	// {
	// 	for(int i=0; i<i_disp; i++)
	// 	{
	// 		cout<<basic.all_internal[i]<<" ";
	// 	}
	// }

}

void unpack_bcast(Basic& basic, int world_rank, int world_size)
{
	/*
	Here we unpack the broadcast message and store them in respective hash tables.
	*/
	int index = 0;
	int first;
	int iptr = 0, jptr=0;
	int insize = 0; int outsize=0;
	int instart, outstart;

	

	//This can be done parallely by using 2 seperate pointers for traversal
	while(index < basic.displacement)
	{
		unordered_set<int> invertices;
		unordered_set<int> outvertices;
		vector<unordered_set<int>> second;

		first = basic.all_probe[index];   //Storing meta node as key 
		index++;
		insize = basic.all_probe[index];
		index++;
		outsize = basic.all_probe[index];
		index++;

		instart = index;
		while(index < (instart + insize))   // Traversing and storing invertices as first column of value vector
		{
			if(basic.partition_of_vertex[basic.all_probe[index]] == world_rank) //Check if the vertex belongs to this partition
			{
				invertices.insert({basic.local_scc_map[basic.all_probe[index]] + (world_rank * global_modifier) });
			}
			index++;
		}
		second.push_back(invertices);

		outstart = index;
		while(index < (outstart + outsize))	//Traversing and storing outvertices as second column of value vector
		{
			if(basic.partition_of_vertex[basic.all_probe[index]] == world_rank) //Check if the vertex belongs to this partition
			{
				outvertices.insert({basic.local_scc_map[basic.all_probe[index]] + (world_rank * global_modifier) });
			}
			index++;
		}
		second.push_back(outvertices);		
		basic.meta_in_out.insert({first,second});   //Push key and value vector into hashmap at end of each iteration
	}
}


void create_meta_graph_vector(Basic& basic, int world_rank, int world_size)
{
	//Unfortunately this is a pretty expensive function. Need to improve this. Can be parallelized
	//Here we create bit vector where the indices are every 1to1 combination of all meta nodes and values being 0 or 1 depending on the presence of an edge between them.
	//The array is created by traversing hashtables so it is N^2 for N meta nodes
	int index = 0;
	pair<int,int> temp;
	basic.meta_graph_vector = (int *)malloc(basic.meta_in_out.size() * basic.meta_in_out.size() * sizeof(int));

	for(auto i : basic.meta_in_out)
	{
		for(auto j : basic.meta_in_out)
		{
			if(i.second[1].find(j.first) != i.second[1].end())
			{
				basic.meta_graph_vector[index] = 1;
				temp.first = i.first;
				temp.second = j.first;
				basic.edge_index.insert({index,temp});
			}
			else
			{
				basic.meta_graph_vector[index] = 0;
				temp.first = i.first;
				temp.second = j.first;
				basic.edge_index.insert({index,temp});
			}
			index++;
		}
	}

	
}

void reduce_meta_graph(Basic& basic, int world_rank, int world_size)
{
	//Here we reduce the meta graph bit vector to fill in the missing meta edges from every process. At the end of all_reduce, every process maintains the same copy of of the vector.
	int* rbuf;
	int buf_size = basic.meta_in_out.size() * basic.meta_in_out.size();
	rbuf = (int *)malloc(basic.meta_in_out.size() * basic.meta_in_out.size() * sizeof(int));

	MPI_Allreduce(basic.meta_graph_vector, rbuf, buf_size, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
	basic.full_ME_vector = rbuf;
	basic.full_ME_vector_size = buf_size;

	// if(world_rank == 2)
	// {
	// 	cout<<endl;
	// 	for(int i = 0; i<buf_size; i++)
	// 	{
	// 		cout<<rbuf[i]<<" ";
	// 	}
	// }
	// cout<<endl;
}

void create_full_meta_graph(Basic& basic, MetaGraph& meta_graph, int world_rank, int world_size)
{
	//Reading edges into a boost graph. This should be replaced when the shared scc code is ready.
	for(int i=0;i<basic.full_ME_vector_size;i++)
	{
		if(basic.full_ME_vector[i] == 1)
		{
			boost::add_edge (basic.edge_index[i].first, basic.edge_index[i].second, meta_graph);
		}
	}
	int itr = 0;
	int node1, node2;
	while(itr<basic.internal_size)
	{
		node1 = basic.all_internal[itr];
		itr++;
		node2 = basic.all_internal[itr];
		itr++;

		boost::add_edge (node1, node2, meta_graph);

	}
}

void reperform_scc(Basic& basic, MetaGraph& meta_graph, int world_rank, int world_size)
{
	int graph_size = boost::num_vertices (meta_graph);
	basic.meta_scc.reserve(graph_size);
	size_t num_components = boost::strong_components (meta_graph, &basic.meta_scc[0]);

	// if(world_rank ==0)
	// {
	// 	cout<<endl;
	// 	for(int i=0;i<graph_size;i++)
	// 	{
	// 		if(basic.meta_in_out.find(i) != basic.meta_in_out.end())
	// 		{
	// 			cout<<i<<" : "<<basic.meta_scc[i]<<endl;
	// 		}
	// 	}
	// }

}

void send_probe(Basic& basic, int world_rank, int world_size)
{
	// mailbox = new int[basic.size_of_probe];
	// mailbox = basic.probe_to_send;

	// if(world_rank == 0)
 // 	{
	//  	cout<<"mail from p0 : ";
	//  	for(int i=0; i<basic.size_of_probe; i++)
	//  	{
	//  		cout<<basic.probe_to_send[i]<<" ";
	//  	}
	//  	cout<<endl;
	//  }

	int *mailbox; MPI_Win win;   //Window called mailbox created for 1 sided communication

	/* create private memory */
 	MPI_Alloc_mem(world_size * mailbox_displacement * sizeof(int), MPI_INFO_NULL, &mailbox);
 	
 	//cout<<"mail from p1 : ";
 	for(int i=0; i<basic.size_of_probe; i++)
 	{
 		mailbox[(world_rank * mailbox_displacement) + i] = basic.probe_to_send[i];
 		//cout<<basic.probe_to_send[i]<<" ";
 		//cout<<mailbox[(world_rank * mailbox_displacement) + i]<<" ";
 	}
 	//cout<<endl;


	/* locally declare memory as remotely accessible */
	MPI_Win_create(mailbox, basic.size_of_probe*sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

 	/* No local operations prior to this epoch, so give an assertion */
	MPI_Win_fence(0,win);
	

	for(auto target : basic.target_list)
	{
		//MPI_Put(mailbox, basic.size_of_probe, MPI_INT, target, sizeof(int), basic.size_of_probe, MPI_INT, win);
		MPI_Put(mailbox + (world_rank * mailbox_displacement), basic.size_of_probe, MPI_INT, target, (world_rank * mailbox_displacement), basic.size_of_probe, MPI_INT, win);
	}

	//Complete the epoch - this will block until MPI_Get is complete 
	MPI_Win_fence(0,win);
	// if(world_rank == 1)
	// {
	// 	cout<<"size "<<basic.size_of_probe<<" mail : ";
	// 	int i=0;
	// 	while(mailbox[i] != -1)
	// 	{
	// 		//cout<<basic.probe_to_send[i]<<" ";
	// 		cout<<mailbox[i]<<" ";
	// 		i++;
	// 	}
			
			
	// }

	//All done with the window - tell MPI there are no more epochs */
	//MPI_Win_fence(MPI_MODE_NOSUCCEED,win);

	 MPI_Win_free(&win);
    MPI_Free_mem(mailbox);  

	cout<<"doneeee";

	
    

	

 	// if(world_rank == 2)
 	// {
 	// 	cout<<" mailbox : ";
 	// 	//for(auto i : mailbox)
 	// 		cout<<mailbox[1]<<" ";
 	// }
}


// void init_meta(Basic& basic)
// {
// 	//for(int i=0;i<basic.l_scc.size();i++)
// 	basic.border_matrix.resize(basic.l_scc.size());
// }

// void make_meta(char *argv[], Basic& basic, Graph& graph, int world_rank)
// {
	
// 	vector<int> bc;
// 	for(int i=0;i<basic.l_scc.size();i++)
// 	{
// 		int border_count=0, out_count=0;
// 		for(auto itr=basic.l_scc[i].begin(); itr!=basic.l_scc[i].end();itr++)
// 		{
// 			//Add borders from both incoming and outgoing edges to border matrix. 
// 			if(basic.border_out_vertices.find(*itr) != basic.border_out_vertices.end())
// 			{
// 				basic.border_matrix[i].push_back(*itr);
// 				border_count++;

// 				for(auto item : basic.border_out_vertices.at(*itr))
// 				{
// 					basic.out_matrix[i][out_count]=item;
// 					out_count++;
// 				}
// 			}
// 			if(basic.border_in_vertices.find(*itr) != basic.border_in_vertices.end())
// 			{
// 				basic.border_matrix[i].push_back(*itr);
// 				border_count++;
// 			}	
			
// 		}
// 		bc.push_back(border_count);
// 	}
// 	int global_num_scc=0;
// 	int local_num_scc=basic.l_scc.size();
// 	MPI_Allreduce(&local_num_scc, &global_num_scc, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
// 	if(world_rank==0)
// 	{
// 	 cout<<" "<<global_num_scc;
// 	}
// 	int global_max_width = 0;
// 	int local_max_width = *max_element(bc.begin(), bc.end());
// 	MPI_Allreduce(&local_max_width, &global_max_width, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
// 	if(world_rank==0)
// 	{
// 	 cout<<" **"<<global_max_width;
// 	}
// 	basic.height = global_num_scc;
// 	basic.width = global_max_width;
// }
// void padding_meta(Basic& basic)
// {
// 	basic.border_matrix.resize(basic.height);
// 	basic.global_border_matrix.resize(basic.height * num_partitions, vector<int>(basic.width));
// 	for(int i=0;i<basic.height;i++)
// 	{
// 		for(int j=basic.border_matrix[i].size();j<basic.width;j++)
// 		{
// 			basic.border_matrix[i].push_back(-1);
// 		}
// 	}
// 	// for(int i=0;i<basic.height * num_partitions;i++)
// 	// {
// 	// 	for(int j=0;j<basic.width;j++)
// 	// 	{
// 	// 		basic.global_border_matrix[i][j]=-1;
// 	// 	}
// 	// }

	
	
// 	ofstream fout("dump/bor_" + std::to_string(world_rank));
// 	for(int itr=0;itr<basic.border_matrix.size();itr++)
// 	{
// 		for(int i=0;i<basic.border_matrix[itr].size();i++)
// 		{
// 			fout<<basic.border_matrix[itr][i]<<" ";
// 		}

// 		fout<<endl;
// 	}
// 	ofstream fout1("dump/glob_bor_" + std::to_string(world_rank));
// 	for(int itr=0;itr<basic.global_border_matrix.size();itr++)
// 	{
// 		for(int i=0;i<basic.global_border_matrix[itr].size();i++)
// 		{
// 			fout1<<basic.global_border_matrix[itr][i]<<" ";
// 		}

// 		fout1<<endl;
// 	}
// 	//basic.border_matrix.resize(basic.height, vector<int>(basic.width, -1));
// 	//basic.global_border_matrix.resize((basic.height * num_partitions), vector<int>(basic.width, -1));
	
// }

// void send_meta(char *argv[], Basic& basic, int world_rank)
// {

// 	Each process needs to send its 2d array of columns= border vertices and row= each local SCC to the root process. Likewise another 2d array for out_matrix defined in the above function.
// 	This is technically of different shapes in each process depending on the number of border elements so I kept a fixed size array and padded it -1. The challenge
// 	here is that root process doesn't know in advance, how many processes are sending so doesn't know how long to wait.
// 	There are a few ways you could do this. I am currently doing #4
// 	1) The way mentioned in the stack overflow https://stackoverflow.com/questions/53592970/mpi-receiving-data-from-an-unknown-number-of-ranks
// 	2) Do an IRecv/ISend and then call a barrier once you know all processes that wanted to send, have, then Recv the right number of messages. This is a danger cause the MPI buffer might fill up if there are too many processes sending. Also might be a bottleneck cause of the barrier.
// 	3) Use one-sided communication (MPI 3 standard). Each process that wants to send would just have a space where it says “here is my stuff,” but you’d need a barrier at the end, and also extra memory for every process, since you don’t know which processes will call a put and so don’t want processes trampling over each other’s memory
// 	4) If you were going to, say, receive messages from rougly 1/2 the processes it would be better to use an MPI_Gather and just have some ranks send nothing.
// 	MPI_Gather(basic.border_matrix.data(),  (basic.height * basic.width), MPI_INT,      /* everyone sends 2 ints from local */
//            basic.global_border_matrix.data(), (basic.height * basic.width), MPI_INT,      /* root receives 2 ints each proc into global */
//            root, MPI_COMM_WORLD);   /* recv'ing process is root, all procs in MPI_COMM_WORLD participate */	

// 	MPI_Gather(basic.out_matrix,  (chunk_width * chunk_height), MPI_INT,      /* everyone sends 2 ints from local */
//            basic.global_out_matrix, (chunk_width * chunk_height), MPI_INT,      /* root receives 2 ints each proc into global */
//            root, MPI_COMM_WORLD);   /* recv'ing process is root, all procs in MPI_COMM_WORLD participate */	
                                   
// }

// void update_global_table(Basic& basic, MetaGraph& meta_graph, int world_rank)
// {
//     basic.global_scc.reserve(boost::num_vertices (meta_graph));

//     size_t num_components = boost::strong_components (meta_graph, &basic.global_scc[0]);
//     //cout<<endl<<"::  "<<num_components;

//     for (size_t i = 0; i < boost::num_vertices (meta_graph); ++i)
//     {
//         if(basic.meta_nodes.find(basic.global_scc[i]) != basic.meta_nodes.end())
//         {
//             basic.global_scc[i] += global_modifier;
//         }
//         cout << basic.global_scc[i] << " ";

//     }

// }

// void make_meta_graph(char *argv[], Basic& basic, MetaGraph& meta_graph, int world_rank)
// {
// 	/*Convert from 2d array to hash map*/
// 	// for(int row=0; row<basic.global_border_matrix.size();row++)
// 	// {
// 	// 	for(int i=0; i<basic.global_border_matrix[row].size();i++)
// 	// 	{
// 	// 		cout<<basic.global_border_matrix[row][i]<<" ";
// 	// 	}
// 	// 	cout<<endl;
// 	// }

// 	cout<<"done";
// 	for(int i =0; i<num_partitions*chunk_height;i++)
// 	{
// 		int j=0;
// 		unordered_set<int> temp;
// 		while(basic.global_border_matrix[i][j] != -1)
// 		{		
// 			temp.insert(basic.global_border_matrix[i][j]);
// 			j++;
// 		}
// 		basic.global_border_vector.push_back({i,temp});
// 	}
// 	for(int i =0; i<num_partitions*chunk_height;i++)
// 	{
// 		int j=0;
// 		while(basic.global_out_matrix[i][j] != -1)
// 		{		
// 			for(auto row:basic.global_border_vector)
// 			{
// 				if(row.second.find(basic.global_out_matrix[i][j]) != row.second.end())
// 				{
// 					//cout<<basic.global_out_matrix[i][j]<<" found in SCC "<<row.first<<endl;
// 					cout<<i<<" -> "<<row.first<<endl;
// 					boost::add_vertex (i, meta_graph);
// 					basic.meta_nodes.insert(i);
// 					boost::add_vertex (row.first, meta_graph);
// 					basic.meta_nodes.insert(row.first);
// 					boost::add_edge (i, row.first, meta_graph);

// 				}
// 			}
// 			j++;
// 		}
// 	}
	
// } 
// void recompute_scc(Basic& basic, MetaGraph& meta_graph, int world_rank)
// {
// 	basic.global_scc.reserve(boost::num_vertices (meta_graph));

// 	size_t num_components = boost::strong_components (meta_graph, &basic.global_scc[0]);
// 	//cout<<endl<<"::  "<<num_components;

// 	for (size_t i = 0; i < boost::num_vertices (meta_graph); ++i)
// 	{
// 		if(basic.meta_nodes.find(basic.global_scc[i]) != basic.meta_nodes.end())
// 		{
// 			basic.global_scc[i] += global_modifier;
// 		}
// 		cout << basic.global_scc[i] << " ";

// 	}

// }

// void create_result(Basic& basic, MetaGraph& meta_graph, int world_rank)
// {
// 	//Create a vector with the global SCC IDs that could be scattered back to the respective tasks
// 	//This is definitely an unnecessary task and should think of a better way of creating it that doesn't involve iterating over the size of all local SCCs
// 	int count=0;
// 	cout<<endl<<"result : ";
// 	for (size_t i = 0; i < boost::num_vertices (meta_graph); ++i)
// 	{
// 		if(basic.global_scc[i] >= global_modifier)
// 		{
// 			basic.global_result[i]=basic.global_scc[i];
// 			count++;
// 		}
// 		else
// 		{
// 			basic.global_result[i]=-1;
// 			count++;
// 		}

// 		cout<<basic.global_result[i]<<" ";
// 	}
// 	cout<<endl<<count;
	

// }

// void scatter_global(Basic& basic, MetaGraph& meta_graph, int world_rank)
// {
// 	MPI_Scatter(basic.global_result,  (chunk_height), MPI_INT,       //everyone recieves chunk_height ints from result 
//            basic.local_result, (chunk_height), MPI_INT,      
//            root, MPI_COMM_WORLD); 

// 	cout<<"done";
// }

// void disjoint_union(Basic& basic, int world_rank)
// {
// 	if (world_rank==1)
// 	{
// 		int root,temp=0,count=0;
// 		ofstream l_scc_dump("dump/l_scc_" + std::to_string(world_rank) + ".txt");

		
// 		basic.alloc_2d_init(basic.nrows,basic.ncols);
// 		//Find intersection of new border with each SCC
// 		for(int it=0;it<basic.l_scc.size();it++)
// 		{	
// 			root= *basic.l_scc[it].begin();   //Some random element chosen from the set. Used as parent of the set when merging and sent along with inrtersections
// 			//basic.merge_detail[it].push_back(root); //First element of the row vector is root followed by intersections.
// 			temp=0;	
// 			for (auto element = basic.border_vertices.begin(); element != basic.border_vertices.end();element++) 
// 			{
// 			  if (basic.l_scc[it].find(*element) != basic.l_scc[it].end()) 
// 			  {
// 			    //basic.intersection_set.push_back(*element);
// 			    basic.detail[it][temp]= *element;
			 
// 			    count++;//For bookeeping
// 			    temp++;
// 			  }
// 			}
// 		}
// 	}

// 	int buffer2[2];
// 	//MPI_Request request;
	
// 	//Remove this from timing as I would implicitly be storing 2d vectors as a flattened array. Those are much better for MPI communication
	
// 	// for(int i=0;i<basic.nrows;i++)
// 	// {
// 	// 	for(int j=0;j<basic.ncols;j++)
// 	// 		cout<<basic.detail[i][j]<<" ";
// 	// }

// 	if(world_rank==1)
// 	{
// 		for(int i=0;i<basic.nrows;i++)
// 		{
// 			cout<<endl;
// 			for(int j=0;j<basic.ncols;j++)
// 				cout<<basic.detail[i][j]<<" ";
// 		}
// 	}
// 		//MPI_Send(basic.detail, count, MPI_INT, 0, 123, MPI_COMM_WORLD);
// 		//MPI_Wait(&request, &status);
	
// }



//template func to test if two sets are disjoint

