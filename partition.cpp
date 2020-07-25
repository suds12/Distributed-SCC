#include <cstddef> /* NULL */
#include <metis.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "graphReader.hpp"

// Install metis from:
// http://glaros.dtc.umn.edu/gkhome/fetch/sw/metis/metis-5.1.0.tar.gz
// or Ubuntu: sudo apt install libmetis-dev
// 
// Adjust makefile, then build with "make partition"
// To run:  ./partition

int main(int argc, char *argv[]){

    if (argc < 3) {
        cerr << "Usage: ./partition inputgraph num_part\n\tinputgraph: edgelist graph input\n\tnum_part: number of partitions\n";
        exit(1);
    }
    string filename = argv[1];
    idx_t nParts    = stoi(argv[2]);
 
    cout << "Partitioning " << filename << " into " << nParts << " partitions...\n";

    idx_t nWeights  = 1; // only one weight value per vertex


    GraphReader reader(1000);   // buffer size is 1000
    cout << "Reading graph...\n";
    reader.read(filename);      

    idx_t nVert = reader.get_num_vert();
    auto xadj = reader.get_adj_ind();
    auto adjncy = reader.get_adj_vert();
    idx_t objval;
    std::vector<idx_t> part(nVert, 0);
    cout << "Partitioning graph with " << nVert << " vertices...\n";
    int ret = METIS_PartGraphKway(&nVert,& nWeights, xadj.data(), adjncy.data(),
				  NULL, NULL, NULL, &nParts, NULL,
       				  NULL, NULL, &objval, part.data());

#if DEBUG >= 2
    std::cout << ret << std::endl;
    for (unsigned part_i = 0; part_i < part.size(); part_i++){
	std::cout << part_i << " " << part[part_i] << std::endl;
    }
#endif

    // Create mapping: a vector corresponding to the partitions, where each entry is the list of vertices 
    // in that partition
    vector<vector<idx_t>> partition(nParts);
    for (unsigned vertex = 0; vertex < part.size(); vertex++){
        partition[part[vertex]].push_back(vertex);
    }
#if DEBUG>=1
    for (unsigned i = 0; i < nParts; i++) {
        cout << i << ": ";
        for (unsigned j = 0; j < partition[i].size(); j++) 
            cout << partition[i][j] << " ";
        cout << endl;
    }
#endif

    // Write out partitioning files

    
    for(unsigned i = 0; i < nParts; i++) {
        stringstream partition_filename_stream;
        partition_filename_stream << filename <<  "_" << i;
        string pfilename = partition_filename_stream.str();
        ofstream partfile(pfilename, ios::out | ios::binary);
        idx_t *vertices = partition[i].data();
        partfile.write(reinterpret_cast<char *>(vertices), partition[i].size() * sizeof(idx_t));
        // For ASCII output, change to: 
        //ofstream partfile(partition_filename.str());
        //for (unsigned j = 0; j < partition[i].size(); j++) partfile << partition[i][j] << endl;
        partfile.close();
    }

#if DEBUG>=1
    // Read the partititioning data and create the same partitioning data structure
    vector<vector<idx_t>> inp_partition(nParts);
    cout << "After reading binary:\n"; 
    for(unsigned i = 0; i < nParts; i++) {
        stringstream partition_filename_stream;
        partition_filename_stream << filename <<  "_" << i;
        string pfilename = partition_filename_stream.str();
       
        ifstream fin(pfilename, ios::in | ios::binary);
        fin.seekg(0, ifstream::end);
        int size = fin.tellg() / sizeof (idx_t);
        idx_t tmp[size];
        fin.seekg(0, ifstream::beg);
        fin.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
        cout << i <<  ": ";
        for (int j=0; j < size; j++) cout << tmp[j] << " ";
        cout << endl;
        std::move(tmp,tmp+size,back_inserter(inp_partition[i]));
        fin.close();
    }
#endif
    return 0;
}
