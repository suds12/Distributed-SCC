#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <set>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/strong_components.hpp>
#include "reader.hpp"

#define chunk_height 10
#define chunk_width 10
#define num_partitions 3
#define root 0
#define global_modifier 9000





void perform_scc(char *argv[], Basic& basic, Graph& graph, int world_rank)   //Shared memory scc
{
	
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




}

void make_meta(char *argv[], Basic& basic, Graph& graph, int world_rank)
{
	for(int i=0;i<basic.l_scc.size();i++)
	{
		int border_count=0, out_count=0;
		for(auto itr=basic.l_scc[i].begin(); itr!=basic.l_scc[i].end();itr++)
		{
			//Add borders from both incoming and outgoing edges to border matrix. 
			if(basic.border_out_vertices.find(*itr) != basic.border_out_vertices.end())
			{
				basic.border_matrix[i][border_count]=*itr;
				border_count++;

				for(auto item : basic.border_out_vertices.at(*itr))
				{
					basic.out_matrix[i][out_count]=item;
					out_count++;
				}
			}
			if(basic.border_in_vertices.find(*itr) != basic.border_in_vertices.end())
			{
				basic.border_matrix[i][border_count]=*itr;
				border_count++;
			}	
			
		}
	}
}

void send_meta(char *argv[], Basic& basic, int world_rank)
{
	/*Each process needs to send its 2d array of columns= border vertices and row= each local SCC to the root process. Likewise another 2d array for out_matrix defined in the above function.
	This is technically of different shapes in each process depending on the number of border elements so I kept a fixed size array and padded it -1. The challenge
	here is that root process doesn't know in advance, how many processes are sending so doesn't know how long to wait.
	There are a few ways you could do this. I am currently doing #4
	1) The way mentioned in the stack overflow https://stackoverflow.com/questions/53592970/mpi-receiving-data-from-an-unknown-number-of-ranks
	2) Do an IRecv/ISend and then call a barrier once you know all processes that wanted to send, have, then Recv the right number of messages. This is a danger cause the MPI buffer might fill up if there are too many processes sending. Also might be a bottleneck cause of the barrier.
	3) Use one-sided communication (MPI 3 standard). Each process that wants to send would just have a space where it says “here is my stuff,” but you’d need a barrier at the end, and also extra memory for every process, since you don’t know which processes will call a put and so don’t want processes trampling over each other’s memory
	4) If you were going to, say, receive messages from rougly 1/2 the processes it would be better to use an MPI_Gather and just have some ranks send nothing.*/
	MPI_Gather(basic.border_matrix,  (chunk_width * chunk_height), MPI_INT,      /* everyone sends 2 ints from local */
           basic.global_border_matrix, (chunk_width * chunk_height), MPI_INT,      /* root receives 2 ints each proc into global */
           root, MPI_COMM_WORLD);   /* recv'ing process is root, all procs in MPI_COMM_WORLD participate */	

	MPI_Gather(basic.out_matrix,  (chunk_width * chunk_height), MPI_INT,      /* everyone sends 2 ints from local */
           basic.global_out_matrix, (chunk_width * chunk_height), MPI_INT,      /* root receives 2 ints each proc into global */
           root, MPI_COMM_WORLD);   /* recv'ing process is root, all procs in MPI_COMM_WORLD participate */	
                                   
}

void make_meta_graph(char *argv[], Basic& basic, MetaGraph& meta_graph, int world_rank)
{
	/*Convert from 2d array to hash map*/
	for(int i =0; i<num_partitions*chunk_height;i++)
	{
		int j=0;
		unordered_set<int> temp;
		while(basic.global_border_matrix[i][j] != -1)
		{		
			temp.insert(basic.global_border_matrix[i][j]);
			j++;
		}
		basic.global_border_vector.push_back({i,temp});
	}
	for(int i =0; i<num_partitions*chunk_height;i++)
	{
		int j=0;
		while(basic.global_out_matrix[i][j] != -1)
		{		
			for(auto row:basic.global_border_vector)
			{
				if(row.second.find(basic.global_out_matrix[i][j]) != row.second.end())
				{
					//cout<<basic.global_out_matrix[i][j]<<" found in SCC "<<row.first<<endl;
					cout<<i<<" -> "<<row.first<<endl;
					boost::add_vertex (i, meta_graph);
					basic.meta_nodes.insert(i);
					boost::add_vertex (row.first, meta_graph);
					basic.meta_nodes.insert(row.first);
					boost::add_edge (i, row.first, meta_graph);

				}
			}
			j++;
		}
	}
	
} 
void recompute_scc(Basic& basic, MetaGraph& meta_graph, int world_rank)
{
	basic.global_scc.reserve(chunk_height * num_partitions);

	size_t num_components = boost::strong_components (meta_graph, &basic.global_scc[0]);
	cout<<endl<<"::  "<<num_components;

	for (size_t i = 0; i < boost::num_vertices (meta_graph); ++i)
	{
		if(basic.meta_nodes.find(basic.global_scc[i]) != basic.meta_nodes.end())
		{
			basic.global_scc[i] += global_modifier;
		}
		cout << basic.global_scc[i] << " ";
	}

}

void create_result(Basic& basic, MetaGraph& meta_graph, int world_rank)
{
	//Create a vector with the global SCC IDs that could be scattered back to the respective tasks
	//This is definitely an unnecessary task and should think of a better way of creating it that doesn't involve iterating over the size of all local SCCs
	cout<<endl<<"result : ";
	for(int i=0;i<(chunk_height * num_partitions); i++)
	{
		if(basic.global_scc[i] >= global_modifier)
		{
			basic.global_result[i]=basic.global_scc[i];
		}
		else
			basic.global_result[i]=-1;

		cout<<basic.global_result[i]<<" ";
	}

}

void scatter_global(Basic& basic, MetaGraph& meta_graph, int world_rank)
{
	MPI_Scatter(basic.global_result,  (chunk_height), MPI_INT,       //everyone recieves chunk_height ints from result 
           basic.local_result, (chunk_height), MPI_INT,      
           root, MPI_COMM_WORLD); 
}

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
template<class Set1, class Set2> 
bool is_disjoint(const Set1 &set1, const Set2 &set2)
{
    if(set1.empty() || set2.empty()) return true;

    typename Set1::const_iterator 
        it1 = set1.begin(), 
        it1End = set1.end();
    typename Set2::const_iterator 
        it2 = set2.begin(), 
        it2End = set2.end();

    if(*it1 > *set2.rbegin() || *it2 > *set1.rbegin()) return true;

    while(it1 != it1End && it2 != it2End)
    {
        if(*it1 == *it2) return false;
        if(*it1 < *it2) { it1++; }
        else { it2++; }
    }

    return true;
}
