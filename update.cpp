#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include<set>
#include "reader.hpp"
#include "main_code.cpp"





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
	int count=0, parent;
	for(int i=0;i<graph.local_scc.size();i++)
	{
	  count=0;
	  cout <<"Set "<< i<<" :";
	  for(set<int> :: iterator it = graph.local_scc[i].begin(); it != graph.local_scc[i].end();++it)
	  {
	    if(count==0)
	    {
	      parent=*it;
	      ds.make_set(*it);
	      
	    }
	    else
	    {
	      ds.make_set(*it);
	      ds.union_set(parent,*it);
	    }
	     if(world_rank==2)
	     	cout<<" " <<*it;

	     count++;
	  }
	  cout <<"\n";

	}

}


void disjoint_union(Basic& graph, int world_rank)
{
	if(world_rank==2)
	{	
		for(set<int> :: iterator it = graph.local_scc[0].begin(); it != graph.local_scc[0].end();++it)
		{
			cout<<*it;
		}
	}

}
