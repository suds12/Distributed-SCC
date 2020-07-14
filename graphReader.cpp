//
// Created by norris on 5/25/20.
//

#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include "graphReader.hpp"


void GraphReader::read(string filename) {
    std::ifstream file;
    int capacity = this->buffer_size;
    adj_vert.reserve(capacity);
    vert_weights.reserve(capacity);
    idx_t maxvertex = 0;

    if ( access( filename.c_str(), F_OK ) == -1 )  {
        cerr << "Specified input file " << filename << " does not exist." << endl;
        exit(1);
    }
    //file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    int current_index = 0;
    try {
        file.open(filename);
        std::string line;
        int previous_vertex = 0;    // start indexing vertices at 0
        adj_ind.push_back(previous_vertex);
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            //cout << line << endl;
            // line contains the current line
            std::vector<int> lineData;
            std::stringstream lineStream(line);
            if (line[0] == '%') continue;

            int value; int i=0;
            // Read an integer at a time from the line
            idx_t e[3];
            while (lineStream >> value) {
                // Add the integers from a line to a 1D array (vector)
                lineData.push_back(value);
                if (i < 2) {
                    e[i] = value;
                    if (maxvertex < value) maxvertex = value;
                }
                // after the vertices, the remaining numbers are the weights
                if (i == 2) vert_weights.push_back(value);
                i++;
            }
            if (i < 2) 
                cerr << "ERROR: Invalid edge list line encountered (edge " << current_index << "):  " << line << endl; 
            adj_vert.push_back(e[1]);

            if (previous_vertex != e[0]) {
                // Starting edges for new vertex; assumes sorted edge lists!!
                adj_ind.push_back(current_index);
                previous_vertex = e[0]; 
            }
            current_index++;
            if (current_index >= capacity) {
                capacity += this->buffer_size;
                adj_vert.reserve(capacity);
                vert_weights.reserve(capacity);
            }
        }
        file.close();
    }
    catch (std::ifstream::failure e) {
        std::cerr << "GraphReader::read: Exception opening/reading/closing file" << filename << endl;
    }
    adj_ind.push_back(current_index);
    adj_vert.shrink_to_fit();
    this->nvertices = maxvertex + 1; // vertex indices start at 0
    //edges.shrink_to_fit();
#if DEBUG == 1
    cout << "Adjacency indices: " << endl; 
    for (auto it = adj_ind.begin(); it != adj_ind.end(); it++) 
        cout << *it << " ";
    cout << "\nAdjacent vertices in consecutive index order: " << endl; 
    for (int i = 0; i < adj_vert.size(); i++) cout << adj_vert[i] << " ";
    cout << endl;
    for (int i = 0; i < vert_weights.size(); i++) cout << vert_weights[i] << " ";
    cout << endl;
#endif
}
