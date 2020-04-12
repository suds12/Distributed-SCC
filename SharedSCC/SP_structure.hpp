#ifndef SP_STRUCTURE_HPP
#define SP_STRUCTURE_HPP

#include<iostream>
#include<vector>
#include<algorithm>
#include <iterator>
#include<utility>
#include <fstream>
#include<cmath>
#include <ctime>
#include <queue>
#include <stack>
#include <map>
#include<list>
#include <omp.h>

//#include <concurrent_vector.h>
using namespace std;


typedef pair<int, int> int_int;  /** /typedef pair of integers */

//Properties of a vertex requored for SCC
struct SCC_Vertex
{
    int glb_ID; //The component number global
    int temp_ID; //The component number set temporarily while updating new components
    
    int index; //Index of where the vertex is in the component list of member vertices
    bool is_changed; //whether the edge associated with the vertex was changed
    //Set when vertex part of changed edge
    vector<int> mysrc; //srcS of vertex if it is sink
    vector<int> mysink; //sinkS of vertex if it is src
    //bool head; //Whether this vertex is the highest in its component
               //only used for deletion
    
    //Constructor
   SCC_Vertex()
    {
        glb_ID=-1;
        temp_ID=-1;
        index=-1;
        is_changed=false;
        mysrc.resize(0);
        mysink.resize(0);
        //head=false;
    }
    
    //Destructor
    void clear()
    {}
};



//Properties of the Network having the SCC
struct VertexProp
{
    
    vector <int_int> In_Neigh; //indegree neighbors
    vector<int_int> Out_Neigh; //outdegree neighbors
    
    //Number of edges at batch x is given by
    //batchIn[x]-batchIn[x-1]
    //Number of initial edges, i.e. batch[0] given by BatchIn[0]
    vector<int> batchIn; //number of indegree neighbors at batch x
    vector<int> batchOut; //number of outdegree neighbors at batch x
    
    //Mark whether any neighbors were deleted in this batch
    vector<bool> delIn; //Whether batch i had any In neighbors deleted
    vector<bool> delOut; //Whether batch i had any In neighbors deleted
    
    bool conn; //connects to a different component
    bool single; //is a singleton component
    
    SCC_Vertex myV;
    
    //Constructor
    VertexProp()
    {In_Neigh.clear();
        Out_Neigh.clear();
        batchIn.clear();
        batchOut.clear();
        delIn.clear();
        delOut.clear();
        conn=false;
        single=false;
    }
    
    //Destructor
    void clear()
    {
        while(!In_Neigh.empty()) {In_Neigh.pop_back();}
        while(!Out_Neigh.empty()) {Out_Neigh.pop_back();}
        while(!batchIn.empty()) {batchIn.pop_back();}
        while(!batchOut.empty()) {batchOut.pop_back();}
        while(!delIn.empty()) {delIn.pop_back();}
        while(!delOut.empty()) {delOut.pop_back();}
    }
};
typedef  vector<VertexProp> SCC_Network;



//Properties of a SCC
struct SCC_Comp
{
    vector<int> member_vertex; //list of vertices in the component
    int size; //number of vertices in component
    bool valid; //whether the component is valid
    bool changed; //whether any edges of the component has been deleted
    int head; //needed for connecting multiple components; head denotes the component which provides the id
    
    SCC_Vertex myV; //properties when the component is treated as a vertex
    
    //Constructor
    SCC_Comp()
    {  member_vertex.clear();
        size=0;
        valid=false;
        changed=false;
        head =-1;
    }
    
    //Destructor
    void clear()
    {}
};




//Structure of Edge with level
/*struct lEdge {
    Edge theEdge;
    int Level;
};

//Data Structure for Remainder Edges
//Bucket of edge list and its Level in buckets ranging from 0-10; 10-20 20-30  ....90-100
struct RE_Bucket
{
    int st;
    int end;
    vector<Edge> REdges; //set of edges in this bucket
    vector<int_double> val_ind; //index from which a particular value starts
    
    //Constructor
    RE_Bucket()
    {
        st=-1;
        end=-1;
        REdges.clear();
        val_ind.clear();
    }
    
    //Destructor
    void clear()
    {}
    
};

//Define compare for edge
struct EdgeCompare
{
    bool operator() (const Edge& edge1, const Edge& edge2) const
    {
        return edge1.edge_wt< edge2.edge_wt;
    }
};
//Map of Edge to Position Index
typedef map<Edge,int, EdgeCompare> map_Edge_int;
*/





#endif
