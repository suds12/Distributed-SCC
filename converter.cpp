#include <cstddef> /* NULL */
#include <metis.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "graphReader.hpp"

// Convert ASCII edgelist graph to binary edgelist with triplets for each edge (v1, v2, weight) -- all ints
// To run:  ./convert filename

int main(int argc, char *argv[]){

    if (argc < 2) {
        cerr << "Usage: ./convert inputgraph" << endl;
        exit(1);
    }
    string filename = argv[1];
 
    cout << "Converting " << filename << " into binary format...\n";

    idx_t nWeights  = 1; // only one weight value per vertex

    GraphReader reader(1000);   // buffer size is 1000
    cout << "Reading graph...\n";
    reader.read(filename);      
    reader.writeBinaryEdgelist(filename+".edgelist.bin");
    cout << "Binary graph: " << filename+".edgelist.bin" << endl;
    return 0;
}
