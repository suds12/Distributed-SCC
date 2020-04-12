#pragma once
#include <iostream>
#include <cstdlib>
#include <boost/pending/disjoint_sets.hpp>
#include <mpi.h>
#include <set> 
#include <iterator>
#include "global.hpp"
//#include "update.cpp"

using namespace std;

int world_size;   
int world_rank;

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

class Merge
{
	std::vector<int> scc;
	
public:
	vector<set <int> > sccSets1;
	//void display();
	void merge_step();
	void custom_function();
	void reduce();
	void disjoint_union(int world_rank);

	Merge()
	{
		int count=0, parent;
		//initialize vector of sets
		set<int> empty;
		for(int i=0;i<3;i++)
		{
			sccSets1.push_back(empty);
		}
		
		sccSets1[0].insert({1,2,4,5});
		sccSets1[1].insert({0,3,6,7,10});
		sccSets1[2].insert({8,9,10,7});
		for(int i=0;i<sccSets1.size();i++)
		{
			count=0;
			for(set<int> :: iterator it = sccSets1[i].begin(); it != sccSets1[i].end();++it)
			{
				 if(count==0)
			    {
			      parent=*it;
			      ds_test.make_set(*it);
			      
			    }
			    else
			    {
			      ds_test.make_set(*it);
			      ds_test.union_set(parent,*it);
			    }
			}
		}
		
		
	}
};

void Merge::reduce()
{
	// int world_size;
 //    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
 //    // Get the rank of the process
 //    int world_rank;
 //    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int sum = 0, global_sum = 0;
	for(int i=0; i<scc.size(); i++)
	{
		sum+=scc[i];
	}
	MPI_Reduce(&sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if(world_rank==0)
		cout<<"sum is "<<global_sum;
		
}

// void Merge::display()
// {
// 	// if(is_disjoint(sccSets[2],sccSets[1]))
// 	// 	cout<<"true\n";
// 	// else
// 	// 	cout<<"false\n";

// 	cout<<"Display func :\n";
// 	for(int i=0; i<sccSets.size();i++)
// 	{
// 		cout <<"Set "<< i<<" :";
// 		for(set<int> :: iterator it = sccSets[i].begin(); it != sccSets[i].end();++it)
// 		{
// 			cout<<*it;
// 		}
// 		cout <<"\n";
// 	}

// }

void Merge::disjoint_union(int world_rank)
{
	// if(world_rank==1)
	// {	
	// 	for(set<int> :: iterator it = local_scc[0].begin(); it != local_scc[0].end();++it)
	// 	{
	// 		cout<<*it;
	// 	}
	// }

}

	







