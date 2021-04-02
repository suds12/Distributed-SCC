#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>
#define chunk_height 3
#define chunk_width 5
#define num_partitions 3
#define root 0

using namespace std;

//This function is for memory mapping. Each reading function would call this with their respective files as arg1
const char* get_file_map_info(const char* fname, size_t& num_bytes, int& world_rank){
    int fd = open(fname, O_RDONLY); // 19
    if(fd == -1){
        if (world_rank == 0){
            cerr<< "error opening the file" << endl;
        }
        return fname; // if there is an error, just returning the filename back
    }
    struct stat sb; // 20
    if(fstat(fd, &sb) == -1){
        if (world_rank == 0){
            cerr << " error in fstat " << endl ;
        }
        return fname;
    }
    num_bytes = sb.st_size;
    const char * addr = static_cast<const char*> (mmap(NULL, num_bytes,PROT_READ, MAP_PRIVATE, fd, 0u ));

    //addr[0], addr[1] give the first and 2nd character in file
    if(addr == MAP_FAILED){
        if(world_rank == 0){
            cerr << "mmap failed";
        }
        return fname;
    }

    return addr;
}

//This function reads the partition id for each vertex from the partition file. The global_SCC disjoint set is also allocated here. Each
void read_partitions(char *argv[], Basic& basic, Graph& graph)
{
	char letter = '\0'; // 47
    int char_count = 0; // 48 keeps track of number of characters in each file
    int lineno=0;
	size_t num_bytes_partition = 0; // 14
	char* buffer = new char[64]();
    const char *partition_pointer  = get_file_map_info(argv[6], num_bytes_partition, world_rank);

    for (int i = 0; i < num_bytes_partition; i++)
    {
    	letter = partition_pointer[i];
    	char_count++;
    	if (letter == '\n')
    	{
            for (int j = 0; j < char_count -1; j++)
            {  
                buffer[j] = partition_pointer[i - (char_count -1) + j];
            }
            buffer[char_count - 1 ] = '\0';
            char_count = 0;
            int pid=atoi(buffer);
            basic.partition_of_vertex.insert({lineno, pid});
            if(world_rank == pid)
            {
            	basic.allocated_vertices.push_back(lineno);
            }
            lineno++;
        }
    }
}

//Function for reading the graph. Each process reads only the edges it is allocated by looking up at the partition hash map
void read_graph(char *argv[], Basic &basic, Graph& graph, int world_rank)
{
	int node1,node2;
	char letter = '\0'; 
    int char_count = 0; 
    int lineno=0;
	size_t num_bytes_input = 0; 
	char* buffer = new char[64]();
	char* token = nullptr; 
    const char *input_pointer  = get_file_map_info(argv[1], num_bytes_input, world_rank);

    for (int i = 0; i < num_bytes_input; i++)
    {
    	letter = input_pointer[i];
    	char_count++;
    	if (letter == '\n')
    	{
            for (int j = 0; j < char_count -1; j++)
            {  
                buffer[j] = input_pointer[i - (char_count -1) + j];
            }
            buffer[char_count - 1 ] = '\0';
            char_count = 0;
            token = strtok(buffer, "\t");
            node1=atoi(token);
            token = strtok(NULL, "\t");
            node2=atoi(token);

            //logic for allocating edges based on partition
            if(basic.partition_of_vertex.at(node1) != basic.partition_of_vertex.at(node2)) //Edge across partition
            {
            	//Here we allocate an edge to the two processes that holds the vertices
				if(world_rank == basic.partition_of_vertex.at(node1) or world_rank == basic.partition_of_vertex.at(node2))
				{
					boost::add_edge (node1, node2, graph);  //Storing in boost ajacency list.
					//store border vertices and store the vertices a specific border vertex has outgoing edges to. This is stored seperately and is only used for merging. Not included while performing local SCC.
					if(world_rank == basic.partition_of_vertex.at(node1))
					{
						if(basic.border_out_vertices.find(node1) == basic.border_out_vertices.end())//border vertex not yet added 
						{
							vector<int> borders;
							borders.push_back(node2);
							basic.border_out_vertices.insert({node1, borders});
						}
						else //border vertex already exists. Push oppopsite vertex to vector mapped with border vertex
						{
							basic.border_out_vertices[node1].push_back(node2);
						}
					}
					//Similarly, store borders of incoming edges in another hashmap
					if(world_rank == basic.partition_of_vertex.at(node2))
					{
						if(basic.border_in_vertices.find(node2) == basic.border_in_vertices.end())
						{
							vector<int> borders;
							borders.push_back(node1);
							basic.border_in_vertices.insert({node2, borders});
						}
						else //border vertex already exists. Push oppopsite vertex to vector mapped with border vertex
						{
							basic.border_in_vertices[node2].push_back(node1);
						}

					}
				}
            }
            else//Edge within the same partition.
            {
            	//Here we allocate an edge only to the process that holds both the vertices
				if(world_rank == basic.partition_of_vertex.at(node1))
				{
					boost::add_edge (node1, node2, graph);   //boost graph
				}
            }
            lineno++;
        }
    }
	local_size=boost::num_vertices (graph);
	basic.local_scc.reserve(local_size);
}

void read_changes(char *argv[], Basic &basic, Graph& changes, Graph& graph, int world_rank)
{
	int node1,node2;
	char letter = '\0'; 
    int char_count = 0; 
    int lineno=0;
	size_t num_bytes_changes = 0; 
	char* buffer = new char[64]();
	char* token = nullptr; 
    const char *changes_pointer  = get_file_map_info(argv[3], num_bytes_changes, world_rank);

    for (int i = 0; i < num_bytes_changes; i++)
    {
    	letter = changes_pointer[i];
    	char_count++;
    	if (letter == '\n')
    	{
            for (int j = 0; j < char_count -1; j++)
            {  
                buffer[j] = changes_pointer[i - (char_count -1) + j];
            }
            buffer[char_count - 1 ] = '\0';
            char_count = 0;
            token = strtok(buffer, "  ");
            node1=atoi(token);
            token = strtok(NULL, "  ");
            node2=atoi(token);

            //logic for allocating edges based on partition
            if(basic.partition_of_vertex.at(node1) != basic.partition_of_vertex.at(node2)) //Edge across partition
            {
            	//Here we allocate an edge to the two processes that holds the vertices
				if(world_rank == basic.partition_of_vertex.at(node1) or world_rank == basic.partition_of_vertex.at(node2))
				{
					//boost::add_edge (node1, node2, graph);  //Storing in boost ajacency list.
					//store border vertices and store the vertices a specific border vertex has outgoing edges to. This is stored seperately and is only used for merging. Not included while performing local SCC.
					if(world_rank == basic.partition_of_vertex.at(node1))
					{
						if(basic.border_out_vertices.find(node1) == basic.border_out_vertices.end())//border vertex not yet added 
						{
							vector<int> borders;
							borders.push_back(node2);
							basic.border_out_vertices.insert({node1, borders});
						}
						else //border vertex already exists. Push oppopsite vertex to vector mapped with border vertex
						{
							basic.border_out_vertices[node1].push_back(node2);
						}
					}
					//Similarly, store borders of incoming edges in another hashmap
					if(world_rank == basic.partition_of_vertex.at(node2))
					{
						if(basic.border_in_vertices.find(node2) == basic.border_in_vertices.end())
						{
							vector<int> borders;
							borders.push_back(node1);
							basic.border_in_vertices.insert({node2, borders});
						}
						else //border vertex already exists. Push oppopsite vertex to vector mapped with border vertex
						{
							basic.border_in_vertices[node2].push_back(node1);
						}

					}
				}
            }
            else//Edge within the same partition.
            {
            	//Here we allocate an edge only to the process that holds both the vertices
				if(world_rank == basic.partition_of_vertex.at(node1))
				{
					boost::add_edge (node1, node2, changes);   
					boost::add_edge (node1, node2, graph); //Adding it also to input graph for now to recompute changes. Should remove it when the shared SCC is ready.

				}
            }
            lineno++;
        }
    }
}

void read_sccmap(char *argv[], Basic &basic, int world_rank)
{
	char letter = '\0'; // 47
    int char_count = 0; // 48 keeps track of number of characters in each file
    int lineno=0;
	size_t num_bytes_sccmap = 0; // 14
	char* buffer = new char[64]();
    const char *sccmap_pointer  = get_file_map_info(argv[2], num_bytes_sccmap, world_rank);

    for (int i = 0; i < num_bytes_sccmap; i++)
    {
    	letter = sccmap_pointer[i];
    	char_count++;
    	if (letter == '\n')
    	{
            for (int j = 0; j < char_count -1; j++)
            {  
                buffer[j] = sccmap_pointer[i - (char_count -1) + j];
            }
            buffer[char_count - 1 ] = '\0';
            char_count = 0;
            int map=atoi(buffer);
            basic.init_scc_of_vertex.insert({lineno, map});
            lineno++;
        }
    }
}


void display(Basic &basic, Graph &graph, int world_rank)
{
	ofstream vertex_dump("dump/ver_" + std::to_string(world_rank) + ".txt");

	ofstream scc_dump("dump/file_no_" + std::to_string(world_rank) + ".txt");
	ofstream out_dump("dump/rel_" + std::to_string(world_rank) + ".txt");
	ofstream inter_dump("dump/int_" + std::to_string(world_rank) + ".txt");
	ofstream probe_dump("dump/probe_" + std::to_string(world_rank) + ".txt");
	ofstream l_scc_dump("dump/l_scc_" + std::to_string(world_rank) + ".txt");
	ofstream updated_result("dump/result" + std::to_string(world_rank) + ".txt");
	ofstream map_dump("dump/b_out_v" + std::to_string(world_rank) + ".txt");
	ofstream dump_bor("dump/global_matrix.txt");

	//display vertices in partition
	for(auto itr:basic.allocated_vertices)
	{
		vertex_dump<<itr<<endl;
	}
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
	// for(auto it : basic.border_out_vertices)
	// {
	// 	out_dump<<it.first<<" : ";
	// 	for(auto i : it.second)
	// 	{
	// 		out_dump<<i<<" ";
	// 	}
	// 	out_dump<<endl;
	// }
	// //display probe message
	// for(int i=0; i<basic.size_of_probe; i++)
	// {
	// 	probe_dump<<basic.probe_to_send[i]<<" ";
	// }
	//----------------------------
	
	

	//Display local SCC
	for (int i = 0; i < boost::num_vertices (graph); ++i)
	{
		if(basic.partition_of_vertex[i]==world_rank)
    		scc_dump << basic.local_scc[i] << " ";
	}
	// //Display local merge details
	// for(int it=0;it<basic.l_scc.size();it++)
	// {
	// 	l_scc_dump<<it<<" : ";
	// 	for(auto itr=basic.l_scc[it].begin(); itr!=basic.l_scc[it].end();itr++)
	// 		l_scc_dump<<*itr<<" ";
	// 	l_scc_dump<<endl;
	// }
	// //Display border matrix
	// for(int i =0; i<basic.l_scc.size();i++)
	// {
	// 	int j=0;
	// 	meta_dump<<endl;
	// 	while(basic.border_matrix[i][j] != -1)
	// 	{
	// 		meta_dump<<basic.border_matrix[i][j]<<" ";
	// 		j++;
	// 	}
	// }

	// //Display out matrix
	// for(int i =0; i<basic.l_scc.size();i++)
	// {
	// 	int j=0;
		
	// 	while(basic.out_matrix[i][j] != -1)
	// 	{
	// 		out_dump<<basic.out_matrix[i][j]<<" ";
	// 		j++;
	// 	}
	// 	out_dump<<endl;
	// }
	// //display global border matrix
	// if(world_rank == root)
	// {
	// 	for(int i=0;i<chunk_height * num_partitions;i++)
	// 	{
	// 		int j=0;
	// 		while(basic.global_border_matrix[i][j] != -1)
	// 		{
	// 			dump_bor<<basic.global_border_matrix[i][j]<<" ";
	// 			j++;
	// 		}
	// 		dump_bor<<endl;
	// 	}
		
	// }
	
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
    //Display border_in_vertices
	
	//Display border matrix
	

	
}




