//
// Created by norris on 5/25/20.
//

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "graphReader.hpp"


void GraphReader::read(string filename, unordered_set<int>& vertices, vector<vector<int>>& edges) {

    std::ifstream file;
    int capacity = this->buffer;
    edges.reserve(capacity);
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(filename);
        std::string line;
        int edge_count = 0;
        while (std::getline(file, line)) {
            // line contains the current line
            std::vector<int> lineData;
            std::stringstream lineStream(line);

            int value; int i=0;
            // Read an integer at a time from the line
            while (lineStream >> value) {
                // Add the integers from a line to a 1D array (vector)
                lineData.push_back(value);
                if (i++ < 2) vertices.insert(value);
                // TODO: If there is a tag, it would be third, handle it here
            }
            // When all the integers have been read, add the 1D vector containing
            // the edge details (end vertices and optionally a flag to indicate add/delete)
            // into the edges vector (TODO that last one)
            edges[edge_count++] = lineData;
            basic.
            if (count >= capacity) {
                capacity += this->buffer;
                edges.reserve(capacity);
            }
        }
        file.close();
    }
    catch (std::ifstream::failure e) {
        std::cerr << "GraphReader::read: Exception opening/reading/closing file" << filename << endl;
    }
    edges.shrink_to_fit();
}