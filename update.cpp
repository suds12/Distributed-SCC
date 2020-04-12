#include <iostream>
#include <string>
#include <sstream>
#include<set>
#include "reader.hpp"
#include "main_code.cpp"



class Update:public Reader
{
public:
	void perform_scc(char *argv[], int world_rank);
	void write_to_files(int world_rank);
};

void Update::perform_scc(char *argv[], int world_rank)   //Shared memory scc
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
		local_scc.push_back(single_component);
	}
	if(world_rank==1)
	{
		set<int> single_component;
		single_component.insert({4,5,6});
		local_scc.push_back(single_component);
	}

}



