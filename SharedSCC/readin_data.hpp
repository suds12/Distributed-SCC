#ifndef READIN_DATA_HPP
#define READIN_DATA_HPP

#include "SP_structure.hpp"
using namespace std;

//Read Network From File
void readin_network(SCC_Network *X, char *file)
{
    //File reading parameters
    FILE *graph_file;
    char line[128];
    
    graph_file=fopen(file, "r");
    int l=0;
    int nodes=0;
    int_int dummy;
    dummy.first=1;
    dummy.second=0;
    while(fgets(line,128,graph_file) != NULL)
    {
        int n1,n2;
        //Read line
        sscanf(line,"%d %d",&n1,&n2);
        
        //Number of nodes given in the first line
        if(l==0)
        {l++; continue;}
        
        dummy.first=n2;
        X->at(n1).In_Neigh.push_back(dummy);
        dummy.first=n1;
        X->at(n2).Out_Neigh.push_back(dummy);
        
        //Find the component ids of the nodes
        int g1=X->at(n1).myV.glb_ID;
        int g2=X->at(n2).myV.glb_ID;
        
        //If the ids are not the same, mark the nodes as connectors
        if(g1!=g2)
        {   X->at(n1).conn=true;
            X->at(n2).conn=true;
        }
        
        l++;
    }//end of while
    fclose(graph_file);
    
    return;
}
/********* End of Function **********/


// Update Batch Size of the Vertices
void batch_update( SCC_Network *X,  int p)
{
#pragma omp parallel for num_threads(p)
    for(int i=0;i<X->size();i++)
    {
        int b_prev, b_new, b_sz;
        
       b_new=X->at(i).In_Neigh.size();
        X->at(i).batchIn.push_back(b_new);
        
        b_new=X->at(i).Out_Neigh.size();
        X->at(i).batchOut.push_back(b_new);
        
        X->at(i).delIn.push_back(false);
        X->at(i).delOut.push_back(false);
    }
    return;
}
/********* End of Function **********/



//Information about SCC at time step 0
void readin_SCC(char *file, SCC_Network *X, vector<SCC_Comp> *C_Info)
{
    FILE *graph_file;
    char line[128];
    graph_file=fopen(file, "r");
    int l=0;
    int ID;
    SCC_Vertex myV;
    
    while(fgets(line,128,graph_file) != NULL)
    {
        sscanf(line,"%d",&ID);
        
        //Add vertex to the component list
         myV.glb_ID=ID;
        myV.index=C_Info->at(ID).member_vertex.size();
       
        C_Info->at(ID).member_vertex.push_back(l);
        C_Info->at(ID).size++;
        C_Info->at(ID).valid=true;
        C_Info->at(ID).changed=false;
        C_Info->at(ID).head=ID;

       
        //Add the vertex info
        X->at(l).myV=myV;
        
        l++;
    }
     fclose(graph_file);
}
/*** End of Function ***/


//Information about set of changed edges
void readin_changes(char *myfile, int b, SCC_Network *X)
{
    //File reading parameters
    FILE *graph_file;
    char line[128];
    int_int edge;
    int type;
    
    graph_file=fopen(myfile, "r");
    while(fgets(line,128,graph_file) != NULL)
    {
        sscanf(line,"%d  %d %d",&edge.first, &edge.second, &type);
        
        int n1=edge.first;
        int n2=edge.second;
        int_int dummy;
        dummy.first=1;
        dummy.second=0;
        
        if(type==0)
        {
            //Delete edge in X
            //Mark with -b to show in which batch it was deleted.
            for(int i=0;i<X->at(n1).In_Neigh.size();i++)
            {
                if(X->at(n1).In_Neigh[i].first==n2)
                { X->at(n1).In_Neigh[i].second=-b;
                    break;}
            }//end of for
            
            //mark deleted
            X->at(n1).delIn[b-1]=true;
            
            for(int i=0;i<X->at(n2).Out_Neigh.size();i++)
            {
                if(X->at(n2).Out_Neigh[i].first==n1)
                {X->at(n2).Out_Neigh[i].second=-b;
                    break;}
            }//end of for
            
            //mark deleted
            X->at(n2).delOut[b-1]=true;
            
        }//end of delete
        

        if(type==1)
        {
         //Add edges in X
            dummy.first=n2;
            X->at(n1).In_Neigh.push_back(dummy);
            dummy.first=n1;
            X->at(n2).Out_Neigh.push_back(dummy);
        }//end of add
        
        
    }//end of while
    
    fclose(graph_file);
    
}
/*** End of Function ***/


#endif
