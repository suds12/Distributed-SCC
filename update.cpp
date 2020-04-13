#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <set>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include "reader.hpp"
#include "main_code.cpp"


typedef
  boost::adjacency_list<
    boost::vecS            // edge list
  , boost::vecS            // vertex list
  , boost::undirectedS     // directedness
  , float                  // property associated with vertices
  >
Graph;


void perform_scc(char *argv[], Basic& graph, int world_rank)   //Shared memory scc
{
	char file1[100]="input/distributed/inputgraph";
	char file2[100]="input/distributed/sccmap";
	char file3[100]="input/distributed/change";

	// char file1[100]="input/inputgraph";
	// char file2[100]="input/sccinput";
	// char file3[100]="input/changes";
	int p = 1;
	int nodes=11;

	//scc(file1,file2,file3,p,nodes,argv);
	if(world_rank==0)
	{
		set<int> single_component;
		single_component.insert({1,2,3});
		graph.local_scc.push_back(single_component);
	}
	if(world_rank==1)
	{
		set<int> single_component;
		single_component.insert({4,5,6});
		graph.local_scc.push_back(single_component);
	}
	if(world_rank==2)
	{
		set<int> single_component;
		single_component.insert({7,8,6});
		graph.local_scc.push_back(single_component);
	}
	//Store in sets into boost disjoint sets 
	// int count=0, parent;
	// for(int i=0;i<graph.local_scc.size();i++)
	// {
	//   count=0;
	//   cout <<"Set "<< i<<" :";
	//   for(set<int> :: iterator it = graph.local_scc[i].begin(); it != graph.local_scc[i].end();++it)
	//   {
	//     if(count==0)
	//     {
	//       parent=*it;
	//       ds.make_set(*it);
	      
	//     }
	//     else
	//     {
	//       ds.make_set(*it);
	//       ds.union_set(parent,*it);
	//     }
	     
	//      cout<<" " <<*it;

	//      count++;
	//   }
	//   cout <<"\n";

	// }

	//Boost SCC
	Graph c (4);
	boost::add_edge (1, 5, c);  
	boost::add_edge (5, 2, c);
	boost::add_edge (2, 4, c);
	boost::add_edge (4, 1, c);

	boost::add_edge (0, 6, c);  
	boost::add_edge (6, 3, c);
	boost::add_edge (3, 7, c);
	boost::add_edge (7, 0, c);

	boost::add_edge (8, 9, c);  
	boost::add_edge (9, 10, c);
	boost::add_edge (10, 8, c);
	

	vector<int> component (boost::num_vertices (c));
	size_t num_components = boost::connected_components (c, &component[0]);

	for (size_t i = 0; i < boost::num_vertices (c); ++i)
    	cout << component[i] << " ";

}


void disjoint_union(Basic& graph, int world_rank)
{


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
