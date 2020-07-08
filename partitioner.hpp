//
// Created by norris on 5/25/20.
//

#ifndef DISTRIBUTED_SCC_PARTITIONER_HPP
#define DISTRIBUTED_SCC_PARTITIONER_HPP

class Partitioner {
public:

    Partitioner(int npartitions) : num_partitions(npartitions) {}
    void partition(int num_partitions, const unordered_set<int>& vertices,
                   const vector<vector<int>>& edges, vector<vector<int>>& result);
private:
    int num_partitions;
};

#endif //DISTRIBUTED_SCC_PARTITIONER_HPP
