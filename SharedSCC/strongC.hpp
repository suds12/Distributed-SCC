//
//  strongC.hpp
//  
//
//  Created by Sanjukta Bhowmick on 4/5/19.
//

#ifndef strongC_h
#define strongC_h
#include "Tarjan.hpp"

//Complete the cycles to get the sparse SCCs
void complete_cycle (A_Network *X, A_Network *Y, vector<SCC_Vertex> *V_Info, int p)
{
//PARALLEL
//Connect the sinks with the source if possible--otherwise all other neighbors
//   vector<bool> new_connS;
//  new_connS.resize(nodes,0);
     #pragma omp parallel for num_threads(p)
for(int i=0;i<X->size();i++)
{ int_int n;
    vector<int_int> myns;
    myns.clear();
    if(Y->at(i).In_ListW.size()==0)
    {
        bool found=false;
        //connect to source if possible
        for(int j=0;j<X->at(i).In_ListW.size();j++)
        {
            n=X->at(i).In_ListW[j];
            //If same component
            if(V_Info->at(n.first).mapID==V_Info->at(i).mapID)
            {
                
                //store the neighbors
                myns.push_back(n);
                
                //if Source then break;
                if(Y->at(n.first).Out_ListW.size()==0)
                {found=true; break;}
            }//end of if
        }//end of for
        
        if(found) //only add to source
        {Y->at(i).In_ListW.push_back(n);}
        else //add all
        {Y->at(i).In_ListW=myns;}
    }//end of if
    
}//end of for

    return;
}
/*** End of Function **/

/**** Process the Inserted Edges on Meta Vertices by Doing Rsynch FW/BW  from srcs***/
void process_edgesXX(vector<SCC_Comp> *C_Info, int p)
{
    bool change;
    change=true;
    
    while(change)
    {change=false;
        
#pragma omp parallel for num_threads(p)
        for(int i=0;i<C_Info->size();i++)
        {
            
            //If not a connector continue;
            if(C_Info->at(i).is_conn==false) {continue;}
            
            
            
            int nodes=C_Info->size();
            //Do a DFS while keeping track of low and disc as per Trajan's method
            //Get the edges forming the SCC
            //Initialize low disc and stack
            vector<int> disc;
            disc.resize(nodes,-1);
            
            vector<int> low;
            low.resize(nodes,-1);
            
            vector<bool> stackMember;
            stackMember.resize(nodes, false);
            
            stack<int> stck;
            
            int iter=0;
            SCC_DFS_IDX(i, &disc, &low, &stck, &stackMember, C_Info,&change, &iter);
            
        }//end of for
        
    }//end of while
    return;
}
/**** End of Function ***/

/**** Process the Inserted Edges on Actual Vertices by Doing FW/BW  from srcs***/
//Parallel is on number of components ***/
void process_edgesC(A_Network *X, vector<SCC_Vertex> *V_Info, int p)
{
    bool change;
    change=true;
    
    while(change)
    {change=false;
        
#pragma omp parallel for num_threads(p)
        for(int i=0;i<V_Info->size();i++)
        {
            
            //If not a outside connector continue;
            if(V_Info->at(i).is_out==false) {continue;}
            
              printf("%d--\n",i);
            
            int nodes=V_Info->size();
            //Do a DFS while keeping track of low and disc as per Trajan's method
            //Get the edges forming the SCC
            //Initialize low disc and stack
            vector<int> disc;
            disc.resize(nodes,-1);
            
            vector<int> low;
            low.resize(nodes,-1);
            
            vector<bool> stackMember;
            stackMember.resize(nodes, false);
            
            stack<int> stck;
            
            int iter=0;
            SCC_DFS_IDC(i, &disc, &low, &stck, &stackMember, X, V_Info,&change, &iter);
            
        }//end of for
        
    }//end of while
    return;
}
/**** End of Function ***/

/**** Process the Inserted Edges on Actual Vertices by Doing FW/BW  from srcs***/
//Parallel is on number of components ***/
void process_edgesV(A_Network *X, vector<SCC_Vertex> *V_Info, int p)
{
    bool change;
    change=true;
    
    while(change)
    {change=false;
        
#pragma omp parallel for num_threads(p)
        for(int i=0;i<V_Info->size();i++)
        {
            
            //If not a connector continue;
            if(V_Info->at(i).is_conn==false) {continue;}
            
         //   printf("%d--\n",i);
            
            int nodes=V_Info->size();
            //Do a DFS while keeping track of low and disc as per Trajan's method
            //Get the edges forming the SCC
            //Initialize low disc and stack
            vector<int> disc;
            disc.resize(nodes,-1);
            
            vector<int> low;
            low.resize(nodes,-1);
            
            vector<bool> stackMember;
            stackMember.resize(nodes, false);
            
            stack<int> stck;
            
            int iter=0;
            SCC_DFS_ID(i, &disc, &low, &stck, &stackMember, X, V_Info,&change, &iter);
            
        }//end of for
        
    }//end of while
    return;
}
/**** End of Function ***/

/**** Process the Inserted Edges on Actual Vertices by Doing FW/BW  from srcs***
//Parallel is on number of components ***
void process_edgesVX(A_Network *X, vector<SCC_Vertex> *V_Info, int max_comm, int p)
{
    bool change;
    vector<bool> complete;
    complete.resize(X->size(),false);
    
    while(1)
    {change=false;
        
#pragma omp parallel for num_threads(p)
        for(int i=0;i<V_Info->size();i++)
        {
            //If not a connector continue;
            if(V_Info->at(i).is_conn==false) {continue;}
            
            //If traversal done continue
            if(complete[i]==true){continue;}
            
            //Skip if already in SCC
            if(!V_Info->at(i).is_valid){continue;}
            
        //Do FW/BW on first source  available
        V_Info->at(i).fw=i;
        V_Info->at(i).bw=i;
            
            //mark as completed
            complete[i]=false;
         
           cout<< "\n \n at root " << i <<"\n";
            //Forward traversal
           queue<int> FQueue;
            FQueue.push(i);
            vector<bool> Fvisited;
            Fvisited.resize(V_Info->size(),false);
            
      while(!FQueue.empty())
      {
          int u=FQueue.front();
          FQueue.pop();
          Fvisited[u]=true;
          int ny;
         // cout << "new node " << u << "\n";
          
          for(int j=0;j<X->at(u).In_ListW.size();j++)
          {
              ny=X->at(u).In_ListW[j].first;
              
            //  cout <<"fneigh " << ny <<":"<<V_Info->at(ny).fw <<"::"<<V_Info->at(i).fw<<"::"<<V_Info->at(ny).mapID<<"::"<<V_Info->at(u).mapID<<"\n";
            
              //Skip if disconnected edge
               if(X->at(u).In_ListW[j].second==0) {continue;}
              
              //Skip if not in same community
              if(V_Info->at(u).mapID!=V_Info->at(ny).mapID) {continue;}
              
              //Skip if fw is filled by a lower number
              //then mark as incomplete
              if(V_Info->at(ny).fw!=-1)
              {
                  //get the node with which the conflict occurs
                  int cft=V_Info->at(ny).fw;
                  
                  //If the conflicted node is still traversing
                  // then stop if id is lower
                  if(!complete[cft] && cft<i)
                  {
                      complete[i]=false;
                      change=true;
                      continue;
                  }
              }//end of if
              
              //If node was traversed by this root continue;
             if(V_Info->at(ny).fw==V_Info->at(i).fw)
             {continue;}
              
              //Otherwise Update fw and continue;
              V_Info->at(ny).fw=V_Info->at(i).fw;
              FQueue.push(ny);
          }//end of for
        }//end of while
          
          
          //Backward traversal
          queue<int> BQueue;
            BQueue.push(i);
          
          vector<bool> Bvisited;
          Bvisited.resize(V_Info->size(),false);
          
          while(!BQueue.empty())
          {
              int u=BQueue.front();
              BQueue.pop();
              Bvisited[u]=true;
              int ny;
              for(int j=0;j<X->at(u).Out_ListW.size();j++)
              {
                  ny=X->at(u).Out_ListW[j].first;
                  
                //   cout <<"bneigh " << ny <<":"<<V_Info->at(ny).bw <<"::"<<V_Info->at(i).bw<<"\n";
                  
                  //Skip if disconnected edge
                  if(X->at(u).Out_ListW[j].second==0) {continue;}
                  
                  //Skip if bw is filled by a lower number
                  //then mark as incomplete
                  if(V_Info->at(ny).bw!=-1)
                  {
                      //get the node with which the conflict occurs
                      int cft=V_Info->at(ny).bw;
                      
                      //If the conflicted node is still traversing
                      // then stop if id is lower
                      if(!complete[cft] && cft<i)
                      {
                          complete[i]=false;
                          change=true;
                          continue;
                      }
                  }//end of if
                  
                  //If node was traversed by this root continue;
                  if(V_Info->at(ny).bw==V_Info->at(i).bw)
                  {continue;}
                  
                  V_Info->at(ny).bw=V_Info->at(i).bw;
              BQueue.push(ny);
              }//end of for
        }//end of while
        
            complete[i]=true;
    
        } //end of for

/*#pragma omp parallel for num_threads(p)
        for(int i=0;i<V_Info->size();i++)
        {
            if(V_Info->at(i).fw==V_Info->at(i).bw)
               {//V_Info->at(i).is_valid=false;
                   continue;
               }
            if((V_Info->at(i).fw==-1)&&(V_Info->at(i).bw==-2))
            {//V_Info->at(i).is_valid=false;
                continue;
            }
            
            else {V_Info->at(i).is_conn=true;
                change=true;
                
            }
        }
if(change==false){break;}
}//end of while
    return;
}
/**** End of Function ***/



void remap_id(vector<SCC_Vertex> *V_Info, int *max_comm, int p)
{
    int nodes=V_Info->size();
    vector<int> Comps;
    Comps.resize(nodes,-1);
    
#pragma omp parallel for num_threads(p)
    for(int i=0;i<V_Info->size();i++)
    { Comps[i]=V_Info->at(i).mapID;}
    
    //Get the list of unique Comps
    sort(&Comps);
    int highC=Comps[Comps.size()-1];
    vector<int> Ucomps;
    Ucomps=unique(Comps);
    
    *max_comm=Ucomps.size();
    
    int_int nscc;
    nscc.first=-1;
    nscc.second=-1;
    vector<int> myMap;
    myMap.resize(highC,-1);
    
#pragma omp parallel for num_threads(p)
    for(int i=0;i<Ucomps.size();i++)
    {myMap[Ucomps[i]]=i;}
    
    //Set mapID and reset sccID
#pragma omp parallel for num_threads(p)
    for(int i=0;i<V_Info->size();i++)
    {   int map=V_Info->at(i).mapID;
        V_Info->at(i).mapID=myMap[map];}
       
    
    
    return;
}
//End of Function

#endif /* strongC_h */
