//
// Created by norris on 5/25/20.
//

#ifndef DISTRIBUTED_SCC_GRAPHREADER_HPP
#define DISTRIBUTED_SCC_GRAPHREADER_HPP

#include <vector>
#include <string>

#include "basic.hpp"
#include "partitioner.hpp"

using namespace std;

class GraphReader {
public:
    GraphReader(int buf) : buffer(buf){}
    void read(string filename);
    void partition(Partitioner& p);
private:
    void dump_binary(string filename);

private:
    int buffer = 100; // reserve memory in advance
    unordered_set<int> vertices;
    vector<vector<int>> edges;
};


#endif //DISTRIBUTED_SCC_GRAPHREADER_HPP
