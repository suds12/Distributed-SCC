#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <set>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/strong_components.hpp>
#include <numeric>
#include <vector>
#include "reader.hpp"

#define chunk_height 3
#define chunk_width 5
#define num_partitions 3
#define root 0
#define global_modifier 9000


void perform_scc(char *argv[], Basic& basic, Graph& graph, int world_rank)   //Shared memory scc
{
	
	int p = 1;
	int nodes=11;

	//Replace this with function call for shared SCC (From Sriram). Tha input parameters would be the allocated graph and SCC mapping. 
	//input => basic.input_graph
	//num of edges = basic.edge_count
	//num of nodes = basic.nodes.size()
	

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

void init_coo(Basic& basic)
{
	//initial size is double the border count.
	basic.border_row = new int[basic.total_border_count*2];
	basic.border_col = new int[basic.total_border_count*2];
	basic.border_value = new int[basic.total_border_count*2];

	basic.out_row = new int[basic.total_border_count*2];
	basic.out_col = new int[basic.total_border_count*2];
	basic.out_value = new int[basic.total_border_count*2];

	
	basic.nnz_capacity = (basic.total_border_count * 2);
	cout<<"^^^"<<(basic.total_border_count * 2);
	basic.out_nnz_capacity = (basic.total_border_count*2);	
}


void make_meta(char *argv[], Basic& basic, Graph& graph, int world_rank)
{
	//ofstream fout("dump/resizing_" + std::to_string(world_rank));
	/*Here we create the meta vertices and store them as matrices in COO format. EAch row represents the local SCC Id and each column represents the border border vertices in that SCC. 
	Likewise we create another matrix for storing our vertices. Here each row represents the local SCC id and each column represents the border vertex from another SCC to which this local SCC connects to.
	We need both these information to make a meta graph*/
	vector<int> bc;
	int row_offset = 0;
	basic.index=0, basic.out_index=0;
	for(int i=0;i<basic.l_scc.size();i++)
	{
		int col_offset=0;
		for(auto itr=basic.l_scc[i].begin(); itr!=basic.l_scc[i].end();itr++)
		{
			//Add borders from both incoming and outgoing edges to border matrix. 
			if(basic.border_out_vertices.find(*itr) != basic.border_out_vertices.end())
			{
				//fout<<basic.index<<" "<<basic.nnz_capacity<<endl;
				// if(basic.index >= basic.nnz_capacity)
				// {
				// 	cout<<"REsizing from "<<world_rank<<endl;
				// 	//Resize the coo arrays by 1000. Need to empirically identify a better resizing size.
				// 	int *temp, *temp1, *temp2;
				// 	temp = (int*) realloc(basic.border_row, 1000 * sizeof(int));
				// 	temp1 = (int*) realloc(basic.border_col, 1000 * sizeof(int));
				// 	temp2 = (int*) realloc(basic.border_value, 1000 * sizeof(int));

				// 	if(temp!=NULL && temp1!=NULL & temp2!= NULL)
				// 	{
				// 		basic.border_row = temp;
				// 		basic.border_col = temp1;
				// 		basic.border_value = temp2;
				// 	}
				// 	basic.nnz_capacity += 1000;

				// }
				basic.border_row[basic.index]=row_offset;
				basic.border_col[basic.index]=col_offset;
				basic.border_value[basic.index]=*itr;
				basic.index++;

				//basic.border_matrix[i][border_count]=*itr;
				//border_count++;


				for(auto item : basic.border_out_vertices.at(*itr))
				{
					// if(basic.out_index >= basic.out_nnz_capacity)
					// {
					// 	//fout<<basic.index<<" "<<basic.nnz_capacity<<endl;
					// 	//cout<<"Resizing from "<<world_rank<<endl;
					// 	//Resize the coo arrays by 1000
					// 	int *temp3, *temp4, *temp5;
					// 	temp3 = (int*) realloc(basic.out_row, 1000 * sizeof(int));
					// 	temp4 = (int*) realloc(basic.out_col, 1000 * sizeof(int));
					// 	temp5 = (int*) realloc(basic.out_value, 1000 * sizeof(int));

					// 	if(temp3!=NULL && temp4!=NULL & temp5!= NULL)
					// 	{
					// 		basic.out_row = temp3;
					// 		basic.out_col = temp4;
					// 		basic.out_value = temp5;
					// 	}
					// 	basic.out_nnz_capacity +=1000;
					// }
					basic.out_row[basic.out_index]=row_offset;
					basic.out_col[basic.out_index]=col_offset;
					basic.out_value[basic.out_index]=item;
					basic.out_index++;

					//basic.out_matrix[i][out_count]=item;
					//out_count++;
				}
			}
			if(basic.border_in_vertices.find(*itr) != basic.border_in_vertices.end())
			{
				// if(basic.index >= basic.nnz_capacity)
				// {
				// 	//fout<<basic.index<<" "<<basic.nnz_capacity<<endl;
				// 	//cout<<"REsizing from "<<world_rank<<endl;
				// 	//Resize the coo arrays by 1000
				// 	int *temp, *temp1, *temp2;
				// 	temp = (int*) realloc(basic.border_row, 1000 * sizeof(int));
				// 	temp1 = (int*) realloc(basic.border_col, 1000 * sizeof(int));
				// 	temp2 = (int*) realloc(basic.border_value, 1000 * sizeof(int));

				// 	if(temp!=NULL && temp1!=NULL & temp2!= NULL)
				// 	{
				// 		basic.border_row = temp;
				// 		basic.border_col = temp1;
				// 		basic.border_value = temp2;
				// 	}
				// }
				basic.border_row[basic.index]=row_offset;
				basic.border_col[basic.index]=col_offset;
				basic.border_value[basic.index]=*itr;
				basic.index++;

				//basic.border_matrix[i][border_count]=*itr;
				//border_count++;
			}	

			col_offset++;
			
		}
		//Don't need these. From the old implementation but it breaks when I remove them. So need to check where it is being used and make appropriate changes.

		row_offset++;
	}
}

void prepare_to_send(Basic& basic, int world_rank)
{
	int task_modifier = 1000 * world_rank;
	//allocate space for the array to comprise all 2 arrays of COO representation for both border and out matrix.
	basic.border_combined = new int [basic.index * 2]; 
	basic.out_combined = new int [basic.out_index * 2];
	
	
	//Copying elements from 2 arrays to border_combined in  such a away that each element of border_combined is followed by each element of out_combined .
	//I am not sending the col array as it doesn't matter which col the border vertices belong to. So only 2 arrays form the COO are sent.
	//This is parallel safe so should be parallelized using openmp. I wonder if we should actually copy. Placing the respective pointers in 2 successive locations would technically form a combined array but won't be contigious and that would be a problem while doing MPI_gather. 
	//Need to confirm with Boyana. Not copying over the array would be a huge oprimization.
	int row = 0, col = 0, val = 0;
	for(int i=0; i<basic.index*2; i+=2)
	{
		basic.border_combined[i] = basic.border_row[row++] + task_modifier;
		//basic.border_combined[i+1] = basic.border_col[col++];
		basic.border_combined[i+1] = basic.border_value[val++];
	}
	//Same for out_matrix
	int row_ = 0, col_ = 0, val_ = 0;
	for(int i=0; i<basic.out_index*2; i+=2)
	{
		basic.out_combined[i] = basic.out_row[row_++] + task_modifier;
		//basic.out_combined[i+1] = basic.out_col[col_++];
		basic.out_combined[i+1] = basic.out_value[val_++];
	}

	// ofstream fout("dump/bor_" + std::to_string(world_rank));
	// for(int i=0; i<basic.out_index;i++)
	// {
	// 	fout<<basic.out_row[i]<<" "<<basic.out_col[i]<<" "<<basic.out_value[i]<<endl;
	// 	//fout<<basic.border_combined[i]<<" ";
	// }

	// ofstream fout("dump/combined_" + std::to_string(world_rank));
	// for(int i=0; i<basic.out_index;i++)
	// {
	// 	fout<<basic.out_row[i]<<" "<<basic.out_col[i]<<" "<<basic.out_value[i]<<endl;
	// 	//fout<<basic.border_combined[i]<<" ";
	// }

	// ofstream fout("dump/combined_" + std::to_string(world_rank));
	// for(int i=0; i<basic.out_index*2;i++)
	// {
	// 	//fout<<basic.out_row[i]<<" "<<basic.out_col[i]<<" "<<basic.out_value[i]<<endl;
	// 	fout<<basic.out_combined[i]<<" ";
	// }
	// ofstream fout1("dump/b_combined_" + std::to_string(world_rank));
	// for(int i=0; i<basic.index*2;i++)
	// {
	// 	//fout<<basic.out_row[i]<<" "<<basic.out_col[i]<<" "<<basic.out_value[i]<<endl;
	// 	fout1<<basic.border_combined[i]<<" ";
	// }
}

void send_meta(char *argv[], Basic& basic, int world_rank, int world_size)
{
	/*Each process needs to send its two 1d arrays, one for border_combined and another for out_combined defined in the above function.
	The challenge here is that root process doesn't know in advance how many processes are sending so doesn't know how long to wait.
	There are a few ways you could do this. I am currently doing #4
	1) The way mentioned in the stack overflow https://stackoverflow.com/questions/53592970/mpi-receiving-data-from-an-unknown-number-of-ranks
	2) Do an IRecv/ISend and then call a barrier once you know all processes that wanted to send, have, then Recv the right number of messages. This is a danger cause the MPI buffer might fill up if there are too many processes sending. Also might be a bottleneck cause of the barrier.
	3) Use one-sided communication (MPI 3 standard). Each process that wants to send would just have a space where it says “here is my stuff,” but you’d need a barrier at the end, and also extra memory for every process, since you don’t know which processes will call a put and so don’t want processes trampling over each other’s memory
	4) If you were going to, say, receive messages from rougly 1/2 the processes it would be better to use an MPI_Gather and just have some ranks send nothing.*/

	//---send border combined------
	int* counts = new int[world_size];
	int size_to_send = basic.index*2;

	// Each process tells the root how many elements it holds
	MPI_Gather(&size_to_send, 1, MPI_INT, counts, 1, MPI_INT, root, MPI_COMM_WORLD);
	// Displacements in the receive buffer for MPI_GATHERV
	int *disps = new int[world_size];
	// Displacement for the first chunk of data - 0
	for (int i = 0; i < world_size; i++)
	   disps[i] = (i > 0) ? (disps[i-1] + counts[i-1]) : 0;
	// Place to hold the gathered data
	// Allocate at root only
	if (world_rank == 0)
	  // disps[size-1]+counts[size-1] == total number of elements
	  basic.global_border_combined = new int[disps[world_size-1] + counts[world_size-1]];
	// Collect everything into the root
	MPI_Gatherv(basic.border_combined, size_to_send, MPI_INT, basic.global_border_combined, counts, disps, MPI_INT, root, MPI_COMM_WORLD);


	basic.sizeof_borders=accumulate(counts , counts+world_size , basic.sizeof_borders);
	//cout<<"&&"<<basic.sizeof_borders;
        if (DEBUG) {
	    ofstream fout("dump/global_combined" );

	    if (world_rank == 0) {
	 	for(int i=0;i<basic.sizeof_borders;i++)
	 		fout<<basic.global_border_combined[i]<<" ";
	    }
        }

	//----send out combined ------------------------

	int* counts1 = new int[world_size];
	int size_to_send1 = basic.out_index*2;
	// Each process tells the root how many elements it holds
	MPI_Gather(&size_to_send1, 1, MPI_INT, counts1, 1, MPI_INT, root, MPI_COMM_WORLD);
	// Displacements in the receive buffer for MPI_GATHERV
	int *disps1 = new int[world_size];
	// Displacement for the first chunk of data - 0
	for (int i = 0; i < world_size; i++)
	   disps1[i] = (i > 0) ? (disps1[i-1] + counts1[i-1]) : 0;
	// Place to hold the gathered data
	// Allocate at root only
	if (world_rank == 0)
	  // disps[size-1]+counts[size-1] == total number of elements
	  basic.global_out_combined = new int[disps1[world_size-1] + counts1[world_size-1]];
	// Collect everything into the root
	MPI_Gatherv(basic.out_combined, size_to_send1, MPI_INT, basic.global_out_combined, counts1, disps1, MPI_INT, root, MPI_COMM_WORLD);

	basic.sizeof_outs=accumulate(counts1 , counts1+world_size , basic.sizeof_outs);
	// ofstream fout1("dump/global_combined");
	// if (world_rank == 0)
	// {
	// 	for(int i=0;i<total_borders1;i++)
	// 		fout1<<basic.global_out_combined[i]<<" ";
	// }
                                   
}

void update_borders(char *argv[], Basic& basic, int world_rank, int world_size)
{
    /* 
        This is an alternative to send_meta. Instead of collecting data through the root, all processes
        exchange their border information.  At the end, all tasks have identical global border information.

        This is accomplished through a two-step process:
         - First, all the sizes are gathered to determine the global border size. 
         - Next, each task sends its border info to all other tasks.
    */

    //---send border combined------
    int* counts = new int[world_size];
    int size_to_send = basic.index*2;

    // Each process tells the everyone else how many elements it holds
    MPI_Allgather(&size_to_send, 1, MPI_INT, counts, 1, MPI_INT, MPI_COMM_WORLD);


    // Displacements in the receive buffer for MPI_GATHERV
    int *disps = new int[world_size];

    // Displacement for the first chunk of data - 0
    for (int i = 0; i < world_size; i++)
        disps[i] = (i > 0) ? (disps[i-1] + counts[i-1]) : 0;
        for(int i=0; i< world_size; i++) {
            MPI_Barrier(MPI_COMM_WORLD);
            if (world_rank == i) {
                cout << world_rank << " counts : " << counts[0] << " " << counts[1] << " " << counts[2]<< endl; 
                cout << world_rank << " disps : " << disps[0] << " " << disps[1] << " " << disps[2]<< endl; 
        }}

    // Place to hold the gathered data, replicated in all tasks!
    basic.sizeof_borders=accumulate(counts , counts+world_size , basic.sizeof_borders);
    basic.global_border_combined = new int[basic.sizeof_borders];

    // Collect everything, at the end, all tasks have identical basic.global_border_combined arrays
    MPI_Allgatherv(basic.out_combined, size_to_send, MPI_INT, basic.global_border_combined, counts, disps, MPI_INT, MPI_COMM_WORLD);


    if (DEBUG) {
	cout<<"Global border size " << world_rank << ": " << basic.sizeof_borders << endl;
        for(int i=0; i< world_size; i++) {
            MPI_Barrier(MPI_COMM_WORLD);
            if (world_rank == i) {
	        ofstream fout("dump/global_combined",  std::ofstream::out | std::ofstream::app);
                fout << "Local Border " << world_rank << ": ";
                for(int i = 0; i < size_to_send; i++) fout << basic.out_combined[i] << " ";
                fout << endl << "COMBINED BORDERS " << world_rank << ": ";
                for(int i=0;i<basic.sizeof_borders;i++) fout << basic.global_border_combined[i] << " ";
                fout << endl;
            }
         }
     }
}

void make_meta_graph(char *argv[], Basic& basic, MetaGraph& meta_graph, int world_rank)
{
	/*Here we connect the meta vertices(local SCCs) to form a meta graph based on the border vertices from each partition and out_vertex to which each border vertex connects to.*/

	cout<<"converting to hashmap"<<endl;
	//Store border_combined in hasmap. key=border vertex and value= respective meta vertex(local SCC ID) 
	for(int i=0; i<basic.sizeof_borders; i+=2)
	{
		basic.global_border_map.insert({basic.global_border_combined[i+1], basic.global_border_combined[i]}); //key=border vertex, val=local scc_id(made unique by adding with task_modifier)
	}	
	cout<<"creating meta graph"<<endl;
	//For each out_vertex check which meta vertex it belongs to and make the connection between the 2 meta vertices.
	for(int i=0; i<basic.sizeof_outs; i+=2)
	{
		//cout<<basic.global_out_combined[i]<<" -> "<<basic.global_border_map[basic.global_out_combined[i +1]]<<endl;
		//boost::add_vertex(basic.global_out_combined[i], meta_graph);
		basic.meta_nodes.insert(basic.global_out_combined[i]);
		//boost::add_vertex(basic.global_border_map[basic.global_out_combined[i +1]], meta_graph);
		basic.meta_nodes.insert(basic.global_border_map[basic.global_out_combined[i +1]]);
		boost::add_edge (basic.global_out_combined[i], basic.global_border_map[basic.global_out_combined[i +1]], meta_graph);
	}
	
	
} 
void recompute_scc(Basic& basic, MetaGraph& meta_graph, int world_rank)
{
	/*Here we recompute the SCC for the meta graph. This should also be optimzed to perform for discontinues vertices. The meta vertex ID would be in order of large numbers 
	to make sure the global SCC ID is distinguishable from the local SCC ID. But current boost implementation fills in the gaps and calculates SCC for evey index within that range. 
	This wouldn't affect the overall result but would be musch slower than just calculating SCC for the vertices present. Doing that is a little more complecated with boost but I will figure that out*/
	basic.global_scc.reserve(boost::num_vertices (meta_graph));

	size_t num_components = boost::strong_components (meta_graph, &basic.global_scc[0]);
	//cout<<endl<<"::  "<<num_components;

	// for (size_t i = 0; i < boost::num_vertices (meta_graph); ++i)
	// {
	// 	if(basic.meta_nodes.find(basic.global_scc[i]) != basic.meta_nodes.end())
	// 	{
	// 		cout << i<<" = "<<basic.global_scc[i] << endl;;
	// 	}
		

	// }

}

void create_result(Basic& basic, MetaGraph& meta_graph, int world_rank)
{
	//Create a vector with the global SCC IDs that could be scattered back to the respective tasks
	//This is definitely an unnecessary task and should think of a better way of creating it that doesn't involve iterating over the size of all local SCCs
	basic.global_result = new int [1000000];
	int count=0;
	// cout<<endl<<"result : ";
	// cout<< boost::num_vertices (meta_graph);
	for (size_t i = 0; i < boost::num_vertices (meta_graph); ++i)
	{
		if(basic.meta_nodes.find(basic.global_scc[i]) != basic.meta_nodes.end())
		{
			//cout<<basic.global_scc[i];
			basic.global_result[i]=basic.global_scc[i];
			count++;
		}
		else
		{
			basic.global_result[i]=-1;
			count++;
		}

		//cout<<basic.global_result[i]<<" ";
	}
	cout<<endl<<count;
	

}

void scatter_global(Basic& basic, MetaGraph& meta_graph, int world_rank)
{
	basic.local_result = new int [1000000];
	MPI_Scatter(basic.global_result,  (chunk_height), MPI_INT,       //everyone recieves chunk_height ints from result 
           basic.local_result, (chunk_height), MPI_INT,      
           root, MPI_COMM_WORLD); 

	cout<<endl<<"done";
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
