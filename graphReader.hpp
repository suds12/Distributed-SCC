//
// Created by norris on 5/25/20.
//

#ifndef DISTRIBUTED_SCC_GRAPHREADER_HPP
#define DISTRIBUTED_SCC_GRAPHREADER_HPP

#include <unordered_set>
#include <vector>
#include <string>

//#include "basic.hpp"
#include "metis.h"

using namespace std;

class GraphReader {
public:
    GraphReader() : buffer_size(100){}
    GraphReader(int buf) : buffer_size(buf){}
    void read(string filename);

    int get_num_vert() { return this->nvertices; }
    vector<idx_t>& get_adj_ind() { return this->adj_ind; }
    vector<idx_t>& get_adj_vert() { return this->adj_vert; }
private:
//    void dump_binary(string filename);

private:
    const int buffer_size = 100; // reserve memory in advance
    idx_t nvertices = 0;
    unordered_set<int> vertices;
    vector<vector<int>> edges;

    // For Metis
    vector<idx_t> adj_ind; // Indices of starting points in adjacent array
    vector<idx_t> adj_vert; // Adjacent vertices in consecutive index order
    vector<idx_t> vert_weights;  // Weights of vertices
};


#endif //DISTRIBUTED_SCC_GRAPHREADER_HPP
