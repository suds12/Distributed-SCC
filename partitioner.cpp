//
// Created by Boyana Norris 5/25/2020.
//

#ifndef __PARTITIONER_HPP
#define __PARTITIONER_HPP


#include <iostream>
#include <string>
#include <vector>

#include "kahiplib/interface/kaHIP_interface.h"
#include "partitioner.hpp"

using namespace std;

Paritioner::partition(int num_partitions, // number of partitions
            const unordered_set<int>& vertices,
            const vector<vector<int>>& edges,
            vector<vector<int>>& result){

    int num_vertices = vertices.size();
    assert(num_vertices == result.size());

    vector<int> edge_count;

    double imbalance = 0.065;

    int* result = new int(num_vertices);
    int cut = 0;
    kaffpa(&num_vertices, nullptr, edge_count.data(), nullptr,
            edges.data(), &num_partitions, &imbalance,
            false, 0, FAST, &cut, result);

    for(int i =0 ; i< num_vertices; i++){
        fout << result[i] << endl;
    }
}

