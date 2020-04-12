
#ifndef INSERT1_OPS_H
#define INSERT1_OPS_H

//Find components that are affected by deletion
void find_changed_componentsIns(SCC_Network *X,vector<SCC_Comp> *C_Info,int b, int p)
{
#pragma omp parallel for num_threads(p)
    for(int i=0;i<X->size();i++)
    {
        int g1, g2;
        int n;
        
        //get the component
        g1=X->at(i).myV.glb_ID;
        
        //Inserted edges of batch b are neighbors from batch[b-1] to batch[b]
        
        //Indegree
            for(int j=X->at(i).batchIn[b-1];j<X->at(i).batchIn[b];j++)
            {
                    //get the delted node and its component
                    n=X->at(i).In_Neigh[j].first;
                    g2=X->at(n).myV.glb_ID;
                    
                    //if in same component--continue;
                    if(g1==g2){continue;}
                    
                    //Else mark component as changed
                    C_Info->at(g1).myV.is_changed=true;
                
                //Mark node as a connector
                X->at(i).conn=true;
            }//end of for j
       
        //Outdegree
        for(int j=X->at(i).batchOut[b-1];j<X->at(i).batchOut[b];j++)
        {
            //get the delted node and its component
            n=X->at(i).Out_Neigh[j].first;
            g2=X->at(n).myV.glb_ID;
            
            //if in same component--continue;
            if(g1==g2){continue;}
            
            //Else mark component as changed
            C_Info->at(g1).myV.is_changed=true;
            
            //Mark node as a connector
            X->at(i).conn=true;
        }//end of for j
        
    }//end of for i
    return;
}
/***End of Function ***/


//Map C_Info to Cmap for shorter array
void map_components(vector<SCC_Comp> *C_Info, vector<int> *Cmap, int p)
{
    //Find number of active components
    int s=0;
    #pragma omp parallel for num_threads(p)
    for(int i=0;i<C_Info->size();i++)
    {
        if(C_Info->at(i).size>0)
        {
         #pragma omp atomic
            s=s+1;
        }
    }
    
    //Map to Cmap
    Cmap->resize(s,-1);
    int l=0;
#pragma omp parallel for num_threads(p)
    for(int i=0;i<C_Info->size();i++)
    {
        if(C_Info->at(i).size>0)
        {
            //Add to first available spot in Cmap
            int t=0;
            while(Cmap->at(t)!=-1)
             {t++;}
            
            Cmap->at(t)=i;
            C_Info->at(i).myV.index=t;
        }
    }//end of for
    
    bool change;
    while(change)
    {
        change=false;
    //Check for unmatched index
    int ind;
    for(int i=0;i<Cmap->size();i++)
    {
        ind=Cmap->at(i);
        
        //If not matched,
     if(C_Info->at(ind).myV.index!=i)
     {
        // then some spots should be available--put it in available spot
         int t=0;
         while(Cmap->at(t)!=-1)
         {t++;}
         
         Cmap->at(t)=ind;
         C_Info->at(ind).myV.index=t;
         
         change=true;
     }
        
    }//end of for
    }//end of while
    return;
}
/**** End of Function ***/

//Find the new components
// Parallel over vertices in each component do fw-bw
//This is equal to doing fw-bw in parallel for each pivot
void change_componentsIns(SCC_Network *X, vector<SCC_Comp> *C_Info, vector<int> *Cmap,int b, int p)
{
bool change=true;
while(change)
{
    change=false;
#pragma omp parallel for num_threads(p) schedule(dynamic)
for(int i=0;i<C_Info->size();i++)
{
    //===Rationale to Skip
    //skip if vertex not src/sink
    if(C_Info->at(i).myV.is_changed==false)
    {continue;}
    
    //skip if vertex is alread in an id <=i
    if((C_Info->at(i).myV.temp_ID<=i) && (C_Info->at(i).myV.temp_ID>-1))
    {continue;}
    
  //===Start FW-BW Method
    queue<int> myQ;
    bool done=false;
    int ind=-1;
       
    //Start Forward
    //Local Variables to Store Vertices reached through FW
    vector<bool> visited_fw;
    visited_fw.resize(Cmap->size(),false);
    
    
    myQ.push(i);
    ind=C_Info->at(i).myV.index;
    visited_fw[ind]=false;
    
    
       while(!myQ.empty())
       {
           int g=myQ.front();
           myQ.pop();
           
           //Propagate from all elements of Component g
           for(int k=0;k<C_Info->at(g).member_vertex.size();k++)
           {
               int n=C_Info->at(g).member_vertex[k];
               
               //continue if it is not a connector
               if(X->at(n).conn==false){continue;}
               
           for(int j=0;j<X->at(n).In_Neigh.size();j++)
           {
               int m=X->at(n).In_Neigh[j].first;
               
               //Do not propagate if edge missing
               if(X->at(n).In_Neigh[j].second==-b){continue;}
               
               int g1=X->at(m).myV.glb_ID;
               
               //Do not propagate if in same component
               if(g1==g){continue;}
               
               //Do not propagate if component visited
               int ind1=C_Info->at(g1).myV.index;
               if(visited_fw[ind1]==true){continue;}
               
               myQ.push(g1);
               visited_fw[ind1]=true;
           } //end of j
           
               
           }//end of k;
       }//end of while
    
    
    //Start Backward
    //Local Variables to Store Vertices reached through FW
   vector<bool> visited_bw;
    visited_bw.resize(Cmap->size(),false);
    
    int found_bw=0; //whether path found
    
    //Do backward propagation on i only
    //Mark component Id during propagation
    myQ.push(i);
    ind=C_Info->at(i).myV.index;
    visited_bw[ind]=true;
    int ID;
    
    while(!myQ.empty())
    {
        int g=myQ.front();
        myQ.pop();
        
        //Mark component if also reached fw
        ind=C_Info->at(g).myV.index;
        if(visited_fw[ind] && visited_bw[ind])
        {
            if(C_Info->at(g).myV.temp_ID==-1)
            {C_Info->at(g).myV.temp_ID=i;
                change=true;}
            else
            {
                //Reduce to lower
                if(C_Info->at(g).myV.temp_ID>i)
                {  C_Info->at(g).myV.temp_ID=i;
                    change=true;}
            }//end of else
        }//end of if
        //==End of marking component
        
        
        //Propagate from all elements of Component c
        for(int k=0;k<C_Info->at(g).member_vertex.size();k++)
        {
            int n=C_Info->at(g).member_vertex[k];
            
            //continue if it is not a connector
            if(X->at(n).conn==false){continue;}
            
            for(int j=0;j<X->at(n).Out_Neigh.size();j++)
            {
                int m=X->at(n).Out_Neigh[j].first;
                
                //Do not propagate if edge missing
                if(X->at(n).Out_Neigh[j].second==-b){continue;}
                
                int g1=X->at(m).myV.glb_ID;
                
                //Do not propagate if in same component
                if(g1==g){continue;}
                
                //Do not propagate if component visited
                int ind1=C_Info->at(g1).myV.index;
                if(visited_bw[ind1]==true){continue;}
                
                myQ.push(g1);
                visited_bw[ind1]=true;
            }//end of for j
        } //end of for k
       }//end of while
    
    
    }//end of i
        
    }//end of while for change
    
    return ;
}
/*** End of Function ****/

//Update the component info
void update_cinfoIns(SCC_Network *X, vector<SCC_Comp> *C_Info, int p)
{
    //Clear the C_info to intial conditions
#pragma omp parallel for num_threads(p) schedule(dynamic)
    for(int i=0;i<C_Info->size();i++)
    {
        if(C_Info->at(i).member_vertex.size()==0)
        {continue;}
        
        int ID=C_Info->at(i).myV.temp_ID;
       // printf("%d---%d\n",i,ID);
        
        //the component not changed; continue;
        if(ID==-1){continue;}
        
        //Otherwise update id
        /*for (int x=0;x< C_Info->at(i).member_vertex.size();x++)
        {
            int v=C_Info->at(i).member_vertex[x];
            //Update info for new Component list
            X->at(v).myV.glb_ID=ID;
            
           // printf("%d---%d\n",v, ID);
        }//end of for*/
        
        C_Info->at(i).head=ID;
        
    }//end of for parallel
    
    return;
}
/***End of Function ***/






#endif /* insert_ops_h */
