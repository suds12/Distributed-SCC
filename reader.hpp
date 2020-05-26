#ifndef DISTRIBUTED_SCC_READER_HPP
#define DISTRIBUTED_SCC_READER_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#include "basic.hpp"

using namespace std;

//This function is for memory mapping. Each reading function would call this with their respective files as arg1
const char* get_file_map_info(const char* fname, size_t& num_bytes, int world_rank);

//This function reads the partition id for each vertex from the partition file. The global_SCC disjoint set is also allocated here. Each
void read_partitions(char *argv[], Basic& basic, Graph& graph, int world_rank);

//Function for reading the graph. Each process reads only the edges it is allocated by looking up at the partition hash map
void read_graph(char *argv[], Basic &basic, Graph& graph, int world_rank);

void read_changes(char *argv[], Basic &basic, Graph& changes, Graph& graph, int world_rank);

void read_sccmap(char *argv[], Basic &basic, int world_rank);

void serialize_basic(Basic basic, int world_rank);

void display(Basic &basic, Graph &graph, int world_rank);

#endif
