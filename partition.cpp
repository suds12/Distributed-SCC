#include <cstddef> /* NULL */
#include <metis.h>
#include <iostream>
#include <vector>

#include "graphReader.hpp"

// Install metis from:
// http://glaros.dtc.umn.edu/gkhome/fetch/sw/metis/metis-5.1.0.tar.gz
// or Ubuntu: sudo apt install libmetis-dev

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

#if DEBUG == 1
    std::cout << ret << std::endl;
    for(unsigned part_i = 0; part_i < part.size(); part_i++){
	std::cout << part_i << " " << part[part_i] << std::endl;
    }
#endif
    return 0;
}
