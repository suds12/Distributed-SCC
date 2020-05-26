//
// Created by norris on 5/25/20.
//

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "graphReader.hpp"


void GraphReader::read(string filename, vector<vector<int>>& edges) {

    std::ifstream file;
    int capacity = this->buffer;
    edges.reserve(capacity);
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(filename);
        std::string line;
        int count = 0;
        while (std::getline(file, line)) {
            // line contains the current line
            std::vector<int> lineData;
            std::stringstream lineStream(line);

            int value;
            // Read an integer at a time from the line
            while (lineStream >> value) {
                // Add the integers from a line to a 1D array (vector)
                lineData.push_back(value);
            }
            // When all the integers have been read, add the 1D vector containing
            // the edge details (end vertices and optionally a flag to indicate add/delete)
            // into the edges vector.
            edges[count++] = lineData;
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