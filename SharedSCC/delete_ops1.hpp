
#ifndef DELETE_OPS1_H
#define DELETE_OPS1_H

//Find components that are affected by deletion
void find_changed_componentsDel(SCC_Network *X,int b, int p)
{
#pragma omp parallel for num_threads(p)
for(int i=0;i<X->size();i++)
{
    int g1, g2;
    int n;
    
    //get the component
    g1=X->at(i).myV.glb_ID;
    //Deleted edges of batch b are marked with -b. To check across all the neighbors
    //Indegree
    //Position b-1 indicates whether any deletes occured at step b
    if(X->at(i).delIn[b-1]==true)
    {
    for(int j=0;j<X->at(i).In_Neigh.size();j++)
    {
        //n=X->at(i).In_Neigh[j].first;
        //printf("%d--%d --%d \n", i, n,X->at(i).In_Neigh[j].second );
        //Found deleted edge
      if(X->at(i).In_Neigh[j].second==-b)
      {
          //get the deleted node and its component
          n=X->at(i).In_Neigh[j].first;
          g2=X->at(n).myV.glb_ID;
          
          //printf("==%d %d %d %d\n", i, g1, n,g2);
          
          //not in same component--continue;
          if(g1!=g2){continue;}
          
          //Mark as changed
          X->at(i).myV.is_changed=true;
      }
    }//end of for j
    }//end of if
    
    
    //Outdegree
    //Position b-1 indicates whether any deletes occured at step b
    if(X->at(i).delOut[b-1]==true)
    {
        for(int j=0;j<X->at(i).Out_Neigh.size();j++)
        {
            //Found deleted edge
            if(X->at(i).Out_Neigh[j].second==-b)
            {
                //get the delted node and its component
                n=X->at(i).Out_Neigh[j].first;
                g2=X->at(n).myV.glb_ID;
                
                //not in same component--continue;
                if(g1!=g2){continue;}
                
                //Mark as changed
                X->at(i).myV.is_changed=true;
            }
        }//end of for j
    }//end of if
    
    } //end of for

    return;
}
/***End of Function ***/


//Find the new components
// Parallel over vertices in each component do fw-bw
//This is equal to doing fw-bw in parallel for each pivot
void change_componentsDel(SCC_Network *X, vector<SCC_Comp> *C_Info,int b,int p)
{
    //To continue until no change in ID
    bool change=true;
    int iter=0;
while(change)
{
    change=false;
    iter++;
#pragma omp parallel for num_threads(p) schedule(dynamic)
for(int i=0;i<X->size();i++)
{
    //Get Community ID
    int g=X->at(i).myV.glb_ID;

    //===Rationales to Skip
    //skip if vertex not associated with changed edge
    if(X->at(i).myV.is_changed==false)
    {continue;}
    
    //skip if vertex is alread in an id <=i
    if((X->at(i).myV.temp_ID<=i) && (X->at(i).myV.temp_ID>-1))
    {continue;}
    
    //Do not check components found to be unbroken after iteration 1
    if((iter>1) && (X->at(i).myV.temp_ID==-1))
    {continue;}
    
    //printf("====%d  %d  %d  %d %d \n",i,g,X->at(i).myV.mysrc.size(),X->at(i).myV.mysink.size(), X->at(i).myV.temp_ID);
    //printf("here \n");
    //printf("==%d %d \n", i, g);
    
    //Mark component as changed
    C_Info->at(g).changed=true;
    
    //===Start FW-BW Method
       queue<int> myQ;
       bool done=false;
       int ind=-1;

   // printf("Start Forward \n");
    //....Start Forward
    //Local Variables to Store Vertices reached through FW
       vector<bool> visited_fw;
       visited_fw.resize(C_Info->at(g).member_vertex.size(),false);

       myQ.push(i);
       ind=X->at(i).myV.index;
       visited_fw[ind]=true;
    
       while(!myQ.empty())
       {
           int n=myQ.front();
           myQ.pop();
           
           for(int j=0;j<X->at(n).In_Neigh.size();j++)
           {
               int m=X->at(n).In_Neigh[j].first;
            
               //Do not propagate if edge missing
               if(X->at(n).In_Neigh[j].second==-b){continue;}
               
               //Do not propagate if in another component
               if(X->at(m).myV.glb_ID!=g){continue;}
               
               //Do not propagate if visited
               int ind1=X->at(m).myV.index;
               if(visited_fw[ind1]==true){continue;}
               
               myQ.push(m);
               visited_fw[ind1]=true;
           }//end of for
           
       }//end of while
    

      //  printf("Start Backward \n");
    //....Start Backward
    //Local Variables to Store Vertices reached through BW
     vector<bool> visited_bw;
    visited_bw.resize(C_Info->at(g).member_vertex.size(),false);
    
    myQ.push(i);
    ind=X->at(i).myV.index;
    visited_bw[ind]=true;
    while(!myQ.empty())
    {
        int n=myQ.front();
        myQ.pop();
        
        //Mark component if also reached fw
         ind=X->at(n).myV.index;
        if(visited_fw[ind] && visited_bw[ind])
        {
            if(X->at(n).myV.temp_ID==-1)
            {X->at(n).myV.temp_ID=i;
            change=true;}
            else
            {
                //Reduce to lower
                if(X->at(n).myV.temp_ID>i)
                {  X->at(n).myV.temp_ID=i;
                    change=true;}
            }//end of else
        }//end of if
        //==End of marking component
        
        for(int j=0;j<X->at(n).Out_Neigh.size();j++)
        {
            int m=X->at(n).Out_Neigh[j].first;
            
            //Do not propagate if edge missing
            if(X->at(n).Out_Neigh[j].second==-b){continue;}
            
            //Do not propagate if in another component
            if(X->at(m).myV.glb_ID!=g){continue;}
            
            //Do not propagate if visited
            int ind1=X->at(m).myV.index;
            if(visited_bw[ind1]==true){continue;}
            
            myQ.push(m);
            visited_bw[ind1]=true;
        }//end of for
    }//end of while
    
}//end of for i
       } //end of while
    return;
}
/***End of Function ***/


void get_singletons (SCC_Network *X, vector<SCC_Comp> *C_Info, int p)
{
    
#pragma omp parallel for num_threads(p) schedule(dynamic)
    for(int i=0;i<C_Info->size();i++)
    {
        //Check per vertex
        //Move vertices to new communities
        for (int x=0;x< C_Info->at(i).member_vertex.size();x++)
        {
            int v=C_Info->at(i).member_vertex[x];
            int ID=X->at(v).myV.temp_ID;
            
            if((ID==-1) && (C_Info->at(i).changed))
            {
                X->at(v).single=true;
                printf("%d--\n",v);
            }
        }
    }
    return;
}
/***End of Function ***/


//Check whether the singletons form new components
void singleton_componentsDel(SCC_Network *X, vector<SCC_Comp> *C_Info,int b,int p)
{
    //To continue until no change in ID
    bool change=true;
    int iter=0;
    while(change)
    {
        change=false;
        iter++;
#pragma omp parallel for num_threads(p) schedule(dynamic)
        for(int i=0;i<X->size();i++)
        {
            //Get Community ID
            int g=X->at(i).myV.glb_ID;
            
            //===Rationales to Skip
            //skip if vertex is not a singleton
            if(X->at(i).single==false)
            {continue;}
            
            //skip if vertex is already in an id <=i
            if((X->at(i).myV.temp_ID<=i) && (X->at(i).myV.temp_ID>-1))
            {continue;}
            
            //Do not check components found to be unbroken after iteration 1
            if((iter>1) && (X->at(i).myV.temp_ID==-1))
            {continue;}
            
            
            //Mark component as changed
            C_Info->at(g).changed=true;
            
            //===Start FW-BW Method
            queue<int> myQ;
            bool done=false;
            int ind=-1;
            
            // printf("Start Forward \n");
            //....Start Forward
            //Local Variables to Store Vertices reached through FW
            vector<bool> visited_fw;
            visited_fw.resize(C_Info->at(g).member_vertex.size(),false);
            
            myQ.push(i);
            ind=X->at(i).myV.index;
            visited_fw[ind]=true;
            
            while(!myQ.empty())
            {
                int n=myQ.front();
                myQ.pop();
                
                for(int j=0;j<X->at(n).In_Neigh.size();j++)
                {
                    int m=X->at(n).In_Neigh[j].first;
                    
                    //Do not propagate if edge missing
                    if(X->at(n).In_Neigh[j].second==-b){continue;}
                    
                    //Do not propagate if in another component
                    if(X->at(m).myV.glb_ID!=g){continue;}
                    
                    //Do not propagate if neighbor was affected by change
                    //Becuase then the changed vertex is a bridge that separates the two components
                    if(X->at(m).myV.is_changed==true){continue;}
                    
                    //Do not propagate if visited
                    int ind1=X->at(m).myV.index;
                    if(visited_fw[ind1]==true){continue;}
                    
                    myQ.push(m);
                    visited_fw[ind1]=true;
                }//end of for
                
            }//end of while
            
            
            //  printf("Start Backward \n");
            //....Start Backward
            //Local Variables to Store Vertices reached through BW
            vector<bool> visited_bw;
            visited_bw.resize(C_Info->at(g).member_vertex.size(),false);
            
            myQ.push(i);
            ind=X->at(i).myV.index;
            visited_bw[ind]=true;
            while(!myQ.empty())
            {
                int n=myQ.front();
                myQ.pop();
                
                //Mark component if also reached fw
                ind=X->at(n).myV.index;
                if(visited_fw[ind] && visited_bw[ind])
                {
                    if(X->at(n).myV.temp_ID==-1)
                    {X->at(n).myV.temp_ID=i;
                        change=true;}
                    else
                    {
                        //Reduce to lower
                        if(X->at(n).myV.temp_ID>i)
                        {  X->at(n).myV.temp_ID=i;
                            change=true;}
                    }//end of else
                }//end of if
                //==End of marking component
                
                for(int j=0;j<X->at(n).Out_Neigh.size();j++)
                {
                    int m=X->at(n).Out_Neigh[j].first;
                    
                    //Do not propagate if edge missing
                    if(X->at(n).Out_Neigh[j].second==-b){continue;}
                    
                    //Do not propagate if in another component
                    if(X->at(m).myV.glb_ID!=g){continue;}
                    
                    //Do not propagate if neighbor was affected by change
                    //Becuase then the changed vertex is a bridge that separates the two components
                    if(X->at(m).myV.is_changed==true){continue;}
                    
                    //Do not propagate if visited
                    int ind1=X->at(m).myV.index;
                    if(visited_bw[ind1]==true){continue;}
                    
                    myQ.push(m);
                    visited_bw[ind1]=true;
                }//end of for
            }//end of while
            
        }//end of for i
    } //end of while
    return;
}
/***End of Function ***/

    //Update the component info
void update_cinfoDel(SCC_Network *X, vector<SCC_Comp> *C_Info,vector<SCC_Comp> *C_Info1, int p)
    {
      //Clear the C_info to intial conditions
#pragma omp parallel for num_threads(p) schedule(dynamic)
        for(int i=0;i<C_Info->size();i++)
        {
            //Check per vertex
            //Move vertices to new communities
            for (int x=0;x< C_Info->at(i).member_vertex.size();x++)
            {
                int v=C_Info->at(i).member_vertex[x];
                int ID=X->at(v).myV.temp_ID;
                            
                if(ID==-1)
                {
                    if(!C_Info->at(i).changed)
                     {ID= X->at(v).myV.glb_ID;}
                    else //if temp -1 but in changed community--then not part of cycle and is singleton
                    {ID=v;}
                }
                
                //Update info for new Component list
                X->at(v).myV.glb_ID=ID;
                X->at(v).myV.index=C_Info1->at(ID).member_vertex.size();
                C_Info1->at(ID).member_vertex.push_back(v);
                C_Info1->at(ID).size=C_Info1->at(ID).member_vertex.size();
                C_Info1->at(ID).head=ID;
            }//end of for
            
        }//end of for parallel
        
               return;
            }
            /***End of Function ***/


#endif /* deletes_h */
