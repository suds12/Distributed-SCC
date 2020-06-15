//
// Created by norris on 5/25/20.
//

#ifndef DISTRIBUTED_SCC_GRAPHREADER_HPP
#define DISTRIBUTED_SCC_GRAPHREADER_HPP

#include <vector>
#include <string>

#include "basic.hpp"

using namespace std;

class GraphReader {
public:
    GraphReader(int buf) : buffer(buf){}
    void read(string filename, Basic basic&, vector< vector<int> >& edges);
    //void partition(Partitioner& p);
private:
    void dump_binary(string filename);

private:
    int buffer = 100; // reserve memory in advance
};


#endif //DISTRIBUTED_SCC_GRAPHREADER_HPP
