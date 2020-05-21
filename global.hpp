#ifndef GLOBALS
#define GLOBALS

#include <boost/pending/disjoint_sets.hpp>
#include<set>
#include <map> 

#ifndef DEBUG
#define DEBUG 0
#endif

extern int world_rank, world_size, local_size;
std::vector<int>  rank (100);
std::vector<int>  parent (100);
extern boost::disjoint_sets<int*,int*> ds(&rank[0], &parent[0]);
extern boost::disjoint_sets<int*,int*> ds_test(&rank[0], &parent[0]);
extern boost::disjoint_sets<int*,int*> global_scc(&rank[0], &parent[0]);


extern std::vector<std::set <int> > sccSets;
#endif
