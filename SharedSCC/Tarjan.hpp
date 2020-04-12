//
//  Tarjan.hpp
//  
//
//  Created by Sanjukta Bhowmick on 4/4/19.
//Modified from https://www.geeksforgeeks.org/tarjan-algorithm-find-strongly-connected-components/
//

#ifndef Tarjan_h
#define Tarjan_h

#include "SP_structure.hpp"


//Recursive function of Tarjan's Algorithm
void SCC_DFS(int u, vector<int> *disc, vector<int> *low,  vector<bool> *stackMember, stack<int> *stck, vector<int> *parent,  A_Network *X, A_Network *Y, vector<SCC_Vertex> *V_Info,int *iter)
{
    
    
    // Initialize discovery time and low value
    *iter=*iter+1;
    disc->at(u) = low->at(u) = *iter;
    stck->push(u);
    stackMember->at(u)= true;
    int_int dummy;
    dummy.second=1;
    
    // Go through all vertices adjacent to this
    for (int i=0;i<X->at(u).In_ListW.size();i++)
    {
        int v = X->at(u).In_ListW[i].first;  // v is current adjacent of 'u'
        
        parent->at(v)=u;
        
        printf("::%d %d ||%d %d %d\n",u,v, low->at(u), disc->at(u), *iter);
        
        //Only go through own component
      /*  if(V_Info->at(v).mapID!=V_Info->at(u).mapID)
        {
        dummy.first=v;
            printf("%d %d \n",v,u);
        Z->at(u).In_ListW.push_back(dummy);
            continue;
        }*/
       
        //cout <<v << ":::"<<u <<"\n";
        // If v is not visited yet, then recur for it
        if (disc->at(v) == -1)
        {
            cout<<"...\n";
            dummy.first=v;
            Y->at(u).In_ListW.push_back(dummy);
            dummy.first=u;
            Y->at(v).Out_ListW.push_back(dummy);
            
            SCC_DFS(v, disc, low, stackMember, stck, parent,X, Y,V_Info,iter);
            // Check if the subtree rooted with 'v' has a
            // connection to one of the ancestors of 'u'
            // Case 1 (per above discussion on Disc and Low value)
            low->at(u)  = min(low->at(u), low->at(v));
        }
        
        // Update low value of 'u' only of 'v' is still in stack
        // (i.e. it's a back edge, not cross edge).
        // Case 2 (per above discussion on Disc and Low value)
        else
        {
            if (stackMember->at(v) == true)
        {
           /* if(low->at(u)>low->at(v))
            {dummy.first=v;
                Y->at(u).In_ListW.push_back(dummy);
                dummy.first=u;
                Y->at(v).Out_ListW.push_back(dummy);}*/
            cout<< "::::"<<low->at(u) <<"::"<< low->at(v) << "::" << disc->at(u) <<"::" << disc->at(v) <<"\n";
            low->at(u) = min(low->at(u), disc->at(v));
             cout<< low->at(u) <<"::"<< low->at(v) << "::" << disc->at(u) <<"::" << disc->at(v) <<"\n";
        }
            else //was visited but no longer in stack therefore across edge
            {
                dummy.first=v;
                Y->at(u).In_ListW.push_back(dummy);
                dummy.first=u;
                Y->at(v).Out_ListW.push_back(dummy);
            }//end of else
        }//end of else
    }//end of for
    
    //Pop Stack if Head reached
    if (low->at(u) == disc->at(u))
    {
        while (stck->top() != u)
        {
           int w = (int) stck->top();
              cout<< w <<"...----\n";
            stackMember->at(w) = false;
            stck->pop();
        }
        int w = (int) stck->top();
        stackMember->at(w) = false;
        stck->pop();
    }//end of if
    
    
    return;
}
/***** End OF Function *****/

//Recursive function of Tarjan's Algorithm---gets SCCID
void SCC_V(int u, vector<int> *disc, vector<int> *low, stack<int> *stck, vector<bool> *stackMember, A_Network *X, vector<SCC_Vertex> *V_Info, bool *change, int *iter)
{
    
    // Initialize discovery time and low value
    *iter=*iter+1;
    disc->at(u) = low->at(u) = *iter;
    stck->push(u);
    stackMember->at(u)= true;
    
    // printf(":::: %d \n",u);
    // Go through all vertices adjacent to this
    
    for (int i=0;i<X->at(u).In_ListW.size();i++)
    {
        if(X->at(u).In_ListW[i].second==0) {continue;} //continue if edge deleted
        
        int v = X->at(u).In_ListW[i].first;  // v is current adjacent of 'u'
        
        //Only go through undecided vertices
        if(V_Info->at(v).is_done) {continue;}
        
        // If v is not visited yet, then recur for it
        if (disc->at(v) == -1)
        {
            SCC_DFS_IDC(v, disc, low, stck, stackMember, X, V_Info, change,iter);
            // Check if the subtree rooted with 'v' has a
            // connection to one of the ancestors of 'u'
            low->at(u)  = min(low->at(u), low->at(v));
        }
        
        // Update low value of 'u' only of 'v' is still in stack
        // (i.e. it's a back edge, not cross edge)
        else if (stackMember->at(v) == true)
        {
            low->at(u) = min(low->at(u), disc->at(v));
        }
    }//end of for
    
    
    //Set V_Info to mark new Component
    //Head Node Found
    // head node found, pop the stack and print an SCC
    int w = 0;  // To store stack extracted vertices
    vector<int> newSCC;
    newSCC.clear();
    
    vector<int> newV;
    newV.clear();
    if (low->at(u) == disc->at(u))
    {
        while (stck->top() != u)
        {
            w = (int) stck->top();
            V_Info->at(w).glbl_ID=u;
            V_Info->at(w).is_done=true;
            stackMember->at(w) = false;
            stck->pop();
        }
        w = (int) stck->top();
        newV.push_back(w);
        newSCC.push_back(V_Info->at(w).mapID);
        stackMember->at(w) = false;
        stck->pop();
    }
    
    return ;
}
/**********************/

//Recursive function of Tarjan's Algorithm---gets SCCID
void SCC_DFS_ID(int u, vector<int> *disc, vector<int> *low, stack<int> *stck, vector<bool> *stackMember, A_Network *X, vector<SCC_Vertex> *V_Info, bool *change, int *iter)
{

    // Initialize discovery time and low value
    *iter=*iter+1;
    disc->at(u) = low->at(u) = *iter;
    stck->push(u);
    stackMember->at(u)= true;
   
   // printf(":::: %d \n",u);
    // Go through all vertices adjacent to this
    
    for (int i=0;i<X->at(u).In_ListW.size();i++)
    {
        if(X->at(u).In_ListW[i].second==0) {continue;} //continue if edge deleted
    
        int v = X->at(u).In_ListW[i].first;  // v is current adjacent of 'u'
        
        //Only go through own component
        if(V_Info->at(v).mapID!=V_Info->at(u).mapID) {continue;}
    
        // If v is not visited yet, then recur for it
        if (disc->at(v) == -1)
        {
           SCC_DFS_ID(v, disc, low, stck, stackMember, X, V_Info, change,iter);
            // Check if the subtree rooted with 'v' has a
            // connection to one of the ancestors of 'u'
            low->at(u)  = min(low->at(u), low->at(v));
        }
        
        // Update low value of 'u' only of 'v' is still in stack
        // (i.e. it's a back edge, not cross edge)
        else if (stackMember->at(v) == true)
        {
            low->at(u) = min(low->at(u), disc->at(v));
        }
    }//end of for
    
 
    //Set V_Info to mark new Component
    //Head Node Found
    // head node found, pop the stack and print an SCC
    int w = 0;  // To store stack extracted vertices
    vector<int> newSCC;
    newSCC.clear();
    if (low->at(u) == disc->at(u))
    {
        while (stck->top() != u)
        {
            w = (int) stck->top();
          //  cout<< w <<"...\n";
            newSCC.push_back(w);
            stackMember->at(w) = false;
            stck->pop();
        }
        w = (int) stck->top();
        newSCC.push_back(w);
        stackMember->at(w) = false;
        stck->pop();
    }
    
   
    //Find smallest vertex id
    //required to avoid conflicts in parallel
    int minV=X->size();
    for(int s=0;s<newSCC.size();s++)
    {
        int n=newSCC[s];
       if(n<minV)
       {minV=n;}
       // cout << n << "---" << minV <<"\n";
    }
    
    //Update V_Info for componentID;
    for(int s=0;s<newSCC.size();s++)
    {
        int n=newSCC[s];
        if(V_Info->at(n).cw!=-1)
        {   if(V_Info->at(n).cw>minV)
            {V_Info->at(n).cw=minV;
            *change=true;}
        }
        V_Info->at(n).cw=minV;
    }
    
    return ;
}
/**********************/

//Recursive function of Tarjan's Algorithm---on Components
void SCC_DFS_IDX(int u, vector<int> *disc, vector<int> *low, stack<int> *stck, vector<bool> *stackMember,  vector<SCC_Comp> *C_Info, bool *change, int *iter)
{
    
    // Initialize discovery time and low value
    *iter=*iter+1;
    disc->at(u) = low->at(u) = *iter;
    stck->push(u);
    stackMember->at(u)= true;
    
    
    // Go through all vertices adjacent to this
    
    for (int i=0;i<C_Info->at(u).In_ListW.size();i++)
    {
        
        int v = C_Info->at(u).In_ListW[i];  // v is current adjacent of 'u'
        

        // If v is not visited yet, then recur for it
        if (disc->at(v) == -1)
        {
            SCC_DFS_IDX(v, disc, low, stck, stackMember, C_Info, change,iter);
            // Check if the subtree rooted with 'v' has a
            // connection to one of the ancestors of 'u'
            low->at(u)  = min(low->at(u), low->at(v));
        }
        
        // Update low value of 'u' only of 'v' is still in stack
        // (i.e. it's a back edge, not cross edge)
        else if (stackMember->at(v) == true)
        {
            low->at(u) = min(low->at(u), disc->at(v));
        }
    }//end of for
    
    
    //Head Node Found
    // head node found, pop the stack and print an SCC
    int w = 0;  // To store stack extracted vertices
    vector<int> newSCC;
    newSCC.clear();
    if (low->at(u) == disc->at(u))
    {
        while (stck->top() != u)
        {
            w = (int) stck->top();
            //  cout<< w <<"...\n";
            newSCC.push_back(w);
            stackMember->at(w) = false;
            stck->pop();
        }
        w = (int) stck->top();
        newSCC.push_back(w);
        stackMember->at(w) = false;
        stck->pop();
    }
    
    
    //Find smallest vertex id
    //required to avoid conflicts in parallel
    int minV=C_Info->size();
    for(int s=0;s<newSCC.size();s++)
    {
        int n=newSCC[s];
        if(n<minV)
        {minV=n;}
        // cout << n << "---" << minV <<"\n";
    }
    
    //Update V_Info for componentID;
    for(int s=0;s<newSCC.size();s++)
    {
        int n=newSCC[s];
        if(C_Info->at(n).cw!=-1)
        {   if(C_Info->at(n).cw>minV)
        {C_Info->at(n).cw=minV;
            *change=true;}
        }
        C_Info->at(n).cw=minV;
    }
    
    return ;
}


#endif /* Tarjan_h */
