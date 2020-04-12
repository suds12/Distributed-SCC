
#ifndef VERIFY_HPP
#define VERIFY_HPP

/*** This File contains functions to verify and print the results ***/

//Print SCC Information about Vertex
void print_SCCV(SCC_Vertex *myV)
{
    printf(" SCC Vertex Information \n");
    
    printf("Comp_ID %d: \n", myV->glb_ID);
    printf("Comp_Index %d: \n", myV->index);
    
}
//End of Function

//Print Information about SCC_Network
void print_Network(SCC_Network *X)
{
    for(int i=0;i<X->size();i++)
    {
        printf(" Node:%d \n",i);
        printf("--> in neighbors: ");
        for(int j=0;j<X->at(i).In_Neigh.size();j++)
        { printf("%d:%d  ", X->at(i).In_Neigh[j].first, X->at(i).In_Neigh[j].second); }
        printf("\n");
        
        printf("--> in batch: ");
        for(int j=0;j<X->at(i).batchIn.size();j++)
        { printf("%d  ", X->at(i).batchIn[j]); }
        printf("\n");
        
        printf("--> in del: ");
        for(int j=0;j<X->at(i).delIn.size();j++)
        { printf("%d  ", (int)(X->at(i).delIn[j])); }
        printf("\n");
        
        printf("--> out neighbors: ");
        for(int j=0;j<X->at(i).Out_Neigh.size();j++)
        { printf("%d:%d  ", X->at(i).Out_Neigh[j].first, X->at(i).Out_Neigh[j].second); }
        printf("\n");
        
        printf("--> out batch: ");
        for(int j=0;j<X->at(i).batchOut.size();j++)
        { printf("%d  ", X->at(i).batchOut[j]); }
        printf("\n");
        
        printf("--> out del: ");
        for(int j=0;j<X->at(i).delOut.size();j++)
        { printf("%d  ", (int)(X->at(i).delOut[j])); }
        printf("\n");
        
        print_SCCV(&X->at(i).myV);
       
        printf("\n");
        printf("--------------\n");
    }//end of for i
    return;
}
//End of Function

//Print Information about C_Info
void print_CInfo(vector<SCC_Comp> *C_Info)
{
    for(int i=0;i<C_Info->size();i++)
    {
        if(C_Info->at(i).size==0){continue;}
        printf("ID of Component is %d: \n", i);
       // printf("Number of Vertices %d \n", C_Info->at(i).size);
        printf("Changed %d \n", C_Info->at(i).changed);
        printf("Changed %d \n", C_Info->at(i).head);
        printf("Member Vertices: ");
        
               for(int j=0;j<C_Info->at(i).member_vertex.size();j++)
               {printf("%d  ", C_Info->at(i).member_vertex[j]);}
               
               printf("\n");
        printf("--------------\n");
    } //end of for i
    
    return;
}
//End of Function


//Print Component of Vertices
void print_component(SCC_Network *X, vector<SCC_Comp> *C_Info)
{
    
    for(int i=0;i<X->size();i++)
    {
        int g=X->at(i).myV.glb_ID;
        
        printf("%d:%d \n", i, C_Info->at(g).head);
        
    }
    
    return;
}
//End of Function

//Print Information about Cmap
void print_Cmap(vector<int> *Cmap)
{
    for(int i=0;i<Cmap->size();i++)
    { printf ("Component %d is at index %d \n", Cmap->at(i),i);}

    return;
}
//End of Function


#endif /* verify_h */
