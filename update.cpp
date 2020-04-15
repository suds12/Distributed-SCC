#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <set>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include "reader.hpp"
#include "main_code.cpp"





void perform_scc(char *argv[], Basic& basic, Graph& graph, int world_rank)   //Shared memory scc
{
	
	int p = 1;
	int nodes=11;

	//Replace this with function call for shared SCC (From Sriram). Tha input parameters would be the allocated graph and SCC mapping. Output is a vector of sets
	size_t num_components = boost::connected_components (graph, &basic.local_scc[0]); //output to local_scc

	// for (size_t i = 0; i < boost::num_vertices (graph); ++i)
 //    	cout << component[i] << " ";

	
	//Additional conversions. Don't time
	int temp=0;
	unordered_set<int> empty;
	for(int i=0;i<num_components;i++)
	{
		basic.temp_scc.push_back(empty);
	}
	// basic.l_scc[basic.local_scc[2]].insert(4);
	// basic.l_scc[basic.local_scc[2]].insert(9);
	// //cout<<basic.local_scc[8];
	// for(auto itr=basic.l_scc[1].begin();itr!=basic.l_scc[1].end();itr++)
	// 	cout<<*itr;

	for(int i=0;i<boost::num_vertices (graph);i++)
	{
		//cout<<"**"<<basic.relevant_vertices.count(i)<<" ";
		if(basic.relevant_vertices.count(i)!=0)
		{
			basic.temp_scc[basic.local_scc[i]].insert(i);
		//cout<<"&&"<<basic.local_scc[i];
		}
		
	}
	int cols=0;
	for(int i=0;i<basic.temp_scc.size();i++)
	{
		if(!basic.temp_scc[i].empty())
		{
			basic.l_scc.push_back(basic.temp_scc[i]);
			if(basic.temp_scc[i].size()>cols)
				cols=basic.temp_scc[i].size();
		}
	}


	
	basic.ncols=cols;
	basic.nrows=basic.temp_scc.size();
	
	// for(int it=0;it<basic.l_scc.size();it++)
	// {
	// 	cout<<it<<" : ";
	// 	for(auto itr=basic.l_scc[it].begin(); itr!=basic.l_scc[it].end();itr++)
	// 		cout<<*itr<<" ";
	// 	cout<<endl;
	// }



}




void disjoint_union(Basic& basic, int world_rank)
{
	if (world_rank==1)
	{
		int root,temp=0,count=0;
		ofstream l_scc_dump("dump/l_scc_" + std::to_string(world_rank) + ".txt");

		
		basic.alloc_2d_init(basic.nrows,basic.ncols);
		//Find intersection of new mirrors with each SCC
		for(int it=0;it<basic.l_scc.size();it++)
		{	
			root= *basic.l_scc[it].begin();   //Some random element chosen from the set. Used as parent of the set when merging and sent along with inrtersections
			//basic.merge_detail[it].push_back(root); //First element of the row vector is root followed by intersections.
			temp=0;	
			for (auto element = basic.mirror_vertices.begin(); element != basic.mirror_vertices.end();element++) 
			{
			  if (basic.l_scc[it].find(*element) != basic.l_scc[it].end()) 
			  {
			    //basic.intersection_set.push_back(*element);
			    basic.detail[it][temp]= *element;
			 
			    count++;//For bookeeping
			    temp++;
			  }
			}
		}
	}

	int buffer2[2];
	//MPI_Request request;
	
	//Remove this from timing as I would implicitly be storing 2d vectors as a flattened array. Those are much better for MPI communication
	
	// for(int i=0;i<basic.nrows;i++)
	// {
	// 	for(int j=0;j<basic.ncols;j++)
	// 		cout<<basic.detail[i][j]<<" ";
	// }

	if(world_rank==1)
	{
		for(int i=0;i<basic.nrows;i++)
		{
			cout<<endl;
			for(int j=0;j<basic.ncols;j++)
				cout<<basic.detail[i][j]<<" ";
		}
	}
		//MPI_Send(basic.detail, count, MPI_INT, 0, 123, MPI_COMM_WORLD);
		//MPI_Wait(&request, &status);
	
}



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
