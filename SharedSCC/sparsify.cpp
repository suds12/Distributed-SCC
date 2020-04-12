
//INPUT HEADERS
#include "translate_from_input.hpp"
#include "input_to_network.hpp"
#include "structure_defs.hpp"

//#include "ADJ/traversal.hpp"

//OUTPUT HEADERS
#include "printout_network.hpp"
#include "printout_others.hpp"

#include "SP_structure.hpp"
#include "readin_data.hpp"
//#include "sparsekey.hpp"


using namespace std;

int main(int argc, char *argv[])
{       
	clock_t q, q1, q2,t;
	
    /***** Preprocessing to Graph (GUI) ***********/
    q=clock();
    //Check if valid input is given
    if ( argc < 3) { cout << "INPUT ERROR:: 2 inputs required. First: filename \n Second: file with component id \n  Third: Set of changed edges \n Fourth: Number of threads \n";}
    //Check to see if file opening succeeded
    ifstream the_file1 ( argv[1] ); if (!the_file1.is_open() ) { cout<<"INPUT ERROR:: Could not open file\n";}
    ifstream the_file2 ( argv[2] ); if (!the_file2.is_open() ) { cout<<"INPUT ERROR:: Could not open file\n";}
    ifstream the_file3 ( argv[3] ); if (!the_file3.is_open() ) { cout<<"INPUT ERROR:: Could not open file\n";}
    
  
    /******* Read Graph (GUI) *****************/
         A_Network X;
        //Obtain the list of edges.
        q=clock();
         readin_networkUDI(&X,argv[1],0,-1);
        q=clock()-q;
    
        cout << "Total Time for Reading Network"<< ((float)q)/CLOCKS_PER_SEC <<"\n";
        int nodes=X.size();
   /**** Read Graph (GUI) ***********/
    
    
    /******* Read Components of Vertices*****************/
    //Information about SCC of each vertex
    SCC_Vertex VI;
    vector<SCC_Vertex> V_Info;
    
    SCC_Comp CI;
    vector<SCC_Comp> C_Info;
    map_int_int CCmap;
    
    
    q=clock();
    readin_SCC(argv[2],&V_Info, &CCmap, &C_Info);
    q=clock()-q;
    
    int max_comm=C_Info.size();
    
    /*cout<<"C: "<< max_comm <<"\n";
    print_vector(Comm_map);*/
    
    cout << "Total Time for Reading SCC "<< ((float)q)/CLOCKS_PER_SEC <<"\n";
    /**** Read Graph (GUI) ***********/
    
    
    /*** Reading in Changed Edges **/
    q=clock();
    vector<int_int> CIns;
    vector<int_int> CDel;
    readin_changes(argv[3], &X, &CIns,&CDel);
    
    q=clock()-q;
    cout << "Total Time for Reading Changed Edges "<< ((float)q)/CLOCKS_PER_SEC <<"\n";
    /*** Reading in Changed Edges **/
    
    
    int p = atoi(argv[4]);  //total number of threads per core
    
    /******* Step 1: Create Sparse Graph for SCC *****************/
   
    //*** Finding Key Edges in Same Component ***/
    //For each vertex get one in degree and one out degree in the same component
    //This will create cycles=components of SCC
    //A single SCC may be broken into multiple cycles
   /*
    A_Network Y;
   create_Network(nodes,0,&Y);
  sparsekey_inC(&X,&Y,&V_Info,p);
    
     print_network(Y, "KeyInComp");
    *******/
    
    /*** Finding Key Edges Across Component ***
    A_Network Z;
    create_Network(max_comm,0,&Z);
    sparsekey_xC(&X,&Z,&V_Info, &C_Info, &CCmap,p);
    
    print_network(Z, "KeyXComp");
   /*******/
    
    //Mark vertices that are singleton
     #pragma omp parallel for num_threads(p)
     for(int i=0;i<X.size();i++)
     {
         if(X[i].In_ListW.size()==0)
         {V_Info[i].is_done=true;}
         
         if(X[i].Out_ListW.size()==0)
         {V_Info[i].is_done=true;}
     }
    
    /***Step 2: Process Deletes ***/
    //1. Update Delete in X
    //2. If Delete within component
    //    spread vertex id of connectors to neighbors
    //stop when (i) all connectors have same (max id) OR (ii) all vertices are reached--which ever ends earlier.
    //3. If Delete across component--do nothing
   
    //Whether an edge in this component has been deleted--if so mark true
    vector<bool> Cdone;
    Cdone.resize(C_Info.size(),true);
    
    vector<int> Cmax;
    Cmax.resize(C_Info.size(),-1);
    //Initialize the
    
    q=clock();
    //Find components that are affected by deletion
#pragma omp parallel for num_threads(p)
    for(int x=0;x<CDel.size();x++)
    {
        //get the nodes
        int n1=CDel[x].first;
        int n2=CDel[x].second;
        
        //Delete edge in X
        for(int i=0;i<X[n1].In_ListW.size();i++)
        {
            if(X[n1].In_ListW[i].first==n2)
            {
                X[n1].In_ListW[i].second=0;
                break;
            }
        }//end of for
        for(int i=0;i<X[n2].Out_ListW.size();i++)
        {
            if(X[n2].Out_ListW[i].first==n1)
            {
                X[n2].Out_ListW[i].second=0;
                break;
            }
        }//end of for
        
        
        //get the glb_IDs
        int g1=V_Info[n1].glb_ID;
        int g2=V_Info[n2].glb_ID;
      
        //not in same component--continue;
        if(g1!=g2){continue;}
        
        //get component map
        int f1=CCmap.find(g1)->second;
        
        V_Info[n1].is_conn=true;
        V_Info[n2].is_conn=true;
        
        V_Info[n1].fw=n1;
        V_Info[n1].bw=n1;
        
        V_Info[n2].fw=n2;
        V_Info[n2].bw=n2;
        
        int max=n1;
        if(max<n2){max=n2;}
        
        if(Cmax[f1]<max){Cmax[f1]=max;}
        
        Cdone[f1]=false;
        
    } //end of for
    
   
    //Do modified coloing in the graph--step 2
    //    spread vertex id of connectors to neighbors
    //stop when (i) all connectors have same (max id) OR (ii) all vertices are reached--which ever ends earlier.
    bool change1 =true;
    bool change2=true;
    
    //continue until any one of the conditions fail
    while(change1 && change2)
    {
        change1=false;
        change2=false;
        printf("-----\n");
#pragma omp parallel for num_threads(p)
    for(int i=0; i<X.size();i++)
    {
        
        int g1=V_Info[i].glb_ID;
        int f1=CCmap.find(g1)->second;
        int max_col=Cmax[f1];
        Cdone[f1]=true;
        
        //If the component is not affected the component of vertex is found
        if(Cmax[f1]==-1){V_Info[i].is_done=1;}
        
       // printf("%d --%d--%d--%d--%d \n",i, f1, Cmax[f1],V_Info[i].fw,V_Info[i].bw );
     
        //check if i is a connector and it reached the max color
        //If not set change2=true for further iterations
        if(V_Info[i].is_conn)
        {
            if(V_Info[i].fw<max_col){change2=true; Cdone[f1]=false;}
            if(V_Info[i].bw<max_col){change2=true; Cdone[f1]=false;}
        }
        
       // if(Cdone[f1]==true) {continue;} //no need to process if work on component completed
        
      if(V_Info[i].is_done) {continue;} //no need to process for known components
        
        
        //propagate only if value received
        if(V_Info[i].fw>-1)
        {
            int edg=0;
        for(int j=0;j<X[i].In_ListW.size();j++)
        {
            if(X[i].In_ListW[j].second==0) {continue;} //do not propagate through deleted edge
            
            int n=X[i].In_ListW[j].first;
            //get the glb_IDs
            int g2=V_Info[n].glb_ID;
            
            if(g1!=g2) {continue;} //propagate only within community
            
            //Do not propagate to sink or source--mark as a component
            if(V_Info[n].is_single){continue;}
            
            edg++;

            if(V_Info[n].fw<V_Info[i].fw)
            {V_Info[n].fw=V_Info[i].fw;
                change1=true;
            }
        }//end of for j
            
         //If edg==0 then it is sink and mark as a  component
            if(edg==0)
            { V_Info[i].is_single=true;}
            
            
        } //end of if
        
        //propagate only if value received
        if(V_Info[i].bw>-1)
        {
            int edg=0;
        for(int j=0;j<X[i].Out_ListW.size();j++)
        {
            if(X[i].Out_ListW[j].second==0) {continue;} //do not propagate through deleted edge
            
            int n=X[i].Out_ListW[j].first;
            //get the glb_IDs
            int g2=V_Info[n].glb_ID;
            
            if(g1!=g2) {continue;} //propagate only within community
            
            //Do not propagate to sink or source--mark as a component
            if(V_Info[n].is_single){continue;}
            
            edg++;
            
            if(V_Info[n].bw<V_Info[i].bw)
            {V_Info[n].bw=V_Info[i].bw;
                change1=true;
            }
        }//end of for j
            
            //If edg==0 then it is src and mark as a component
            if(edg==0)
            { V_Info[i].is_single=true;}
        }//end of if
        
        if(V_Info[i].is_single )
        {  V_Info[i].fw=i;
            V_Info[i].bw=i;
            V_Info[i].is_done=1;
        }
        
        //If vertex obtained highest value from both fw and bw then component found
        if((V_Info[i].fw==Cmax[f1]) && (V_Info[i].bw==Cmax[f1])){V_Info[i].is_done=1;}
        
    }//end of for i
    
    }//end of while
    
    
    //Once the components are found--find the components where fw and bw are different
    bool changeX=true;
    while(changeX)
    {
        changeX=false;
    
    bool change=true;
    while(change)
    {
        change=false;
#pragma omp parallel for num_threads(p)
        for(int i=0; i<X.size();i++)
        {
            if(V_Info[i].is_done) {continue;}
            
            //if(V_Info[i].fw1==-1){V_Info[i].fw1=i;}
            //if(V_Info[i].bw1==-1){V_Info[i].bw1=i;}
            
             int g1=V_Info[i].glb_ID;
            //Do forward swwep to find communities
            for(int j=0;j<X[i].In_ListW.size();j++)
            {
                if(X[i].In_ListW[j].second==0) {continue;} //do not propagate through deleted edge
                
                int n=X[i].In_ListW[j].first;
                
                //Do not propagate to sink or source--mark as a component
                if(V_Info[n].is_done){continue;}
                
                
                //get the glb_IDs
                int g2=V_Info[n].glb_ID;
                
                if(g1!=g2) {continue;} //propagate only within community
                
                if(V_Info[n].fw1<V_Info[i].fw1)
                {V_Info[n].fw1=V_Info[i].fw1;
                    change=true;}
            }//end of for j
            
            //Do forward swwep to find communities
            for(int j=0;j<X[i].Out_ListW.size();j++)
            {
                if(X[i].Out_ListW[j].second==0) {continue;} //do not propagate through deleted edge
                
                int n=X[i].Out_ListW[j].first;
                
                //Do not propagate to sink or source--mark as a component
                if(V_Info[n].is_done){continue;}
                
                //get the glb_IDs
                int g2=V_Info[n].glb_ID;
                
                if(g1!=g2) {continue;} //propagate only within community
                
                if(V_Info[n].bw1<V_Info[i].bw1)
                {V_Info[n].bw1=V_Info[i].bw1;
                    change=true;}
            }//end of for j
            
      }//end of for i
    
    }//end of while
    
#pragma omp parallel for num_threads(p)
    for(int i=0; i<X.size();i++)
    {
        if(V_Info[i].is_done) {continue;}
       if(V_Info[i].fw1==V_Info[i].bw1)
       {V_Info[i].is_done=true;}
        else
        { V_Info[i].fw1=i;
            V_Info[i].bw1=i;
            changeX=true;
        }
        
    }
}//end of while
    
 /*   for(int i=0;i<X.size();i++)
    {
        printf("%d  %d %d|| %d %d :%d %d %d\n", i, V_Info[i].fw, V_Info[i].bw, V_Info[i].fw1, V_Info[i].bw1, V_Info[i].is_done, V_Info[i].lcl_ID, V_Info[i].glb_ID);
        
    }*/
    
 //Get the new component IDs
    int maxC=-1;
#pragma omp parallel for num_threads(p)
    for(int i=0; i<X.size();i++)
    {
        if(V_Info[i].fw==V_Info[i].bw)
        {if(V_Info[i].fw>-1)
        {V_Info[i].lcl_ID=V_Info[i].fw;}
         else
         {V_Info[i].lcl_ID=V_Info[i].glb_ID;}
        }
       else
       {
        if(V_Info[i].fw1==V_Info[i].bw1)
        {{V_Info[i].lcl_ID=V_Info[i].fw1;} }
       }
        
        //make atomic update
        if(maxC<V_Info[i].lcl_ID){
#pragma omp atomic
            {maxC=V_Info[i].lcl_ID;}
            
        }
        
        //set values to default
       V_Info[i].is_conn=false;
        V_Info[i].fw=-1;
        V_Info[i].bw=-1;
        V_Info[i].fw1=-1;
        V_Info[i].bw1=-1;
        V_Info[i].is_done=false;
    }
    q=clock()-q;
    cout << "Total Time for Deletion "<< ((float)q)/CLOCKS_PER_SEC <<"\n";
    /*for(int i=0;i<X.size();i++)
    {
        printf("%d  %d %d|| %d %d :%d %d %d\n", i, V_Info[i].fw, V_Info[i].bw, V_Info[i].fw1, V_Info[i].bw1, V_Info[i].is_done, V_Info[i].lcl_ID, V_Info[i].glb_ID);
        
    }*/
  //==================
    //Obtain new C_INFO
    
    SCC_Comp CXI;
    vector<SCC_Comp> CX_Info;
    map_int_int CCXmap;
    CX_Info.resize(maxC,CXI);
    
    vector<int> CX_max;
    CX_max.resize(maxC, -1);
    
    q=clock();
    #pragma omp parallel for num_threads(p)
    for(int i=0;i<C_Info.size();i++)
    {
        for(int j=0;j<C_Info[i].member_vertex.size();j++)
        {
            int k=C_Info[i].member_vertex[j];
            int n=V_Info[k].lcl_ID;
            //printf("%d  %d \n", n, maxC);
            CX_Info[n].ID=n;
            CX_Info[n].member_vertex.push_back(k);
            if(CX_max[n]<k)
            {CX_max[n]=k;}
        }
    }
    
  //=====
    
  //Now find the connectors for insertions
    //Find components that are affected by deletion
#pragma omp parallel for num_threads(p)
    for(int x=0;x<CIns.size();x++)
    {
        //get the nodes
        int n1=CIns[x].first;
        int n2=CIns[x].second;
        
        //get the glb_IDs
        int g1=V_Info[n1].lcl_ID;
        int g2=V_Info[n2].lcl_ID;
        
        //If in same component-continue;
        if(g1==g2){continue;}
        
        V_Info[n1].is_conn=true;
        V_Info[n2].is_conn=true;
        
        //Set fw  and bw of the two end points indicating edge n1-->n2
        V_Info[n1].fw=CX_max[g1];
        V_Info[n1].bw=CX_max[g2];
        
        V_Info[n2].fw=CX_max[g1];
        V_Info[n2].bw=CX_max[g2];
        
    } //end of for
    
    //==Do Fw-Bw to find the components
    //Once the components are found--find the components where fw and bw are different
    changeX=true;
    while(changeX)
    {
        changeX=false;
        
        bool change=true;
        while(change)
        {
            //printf("heerXX \n");
            change=false;
#pragma omp parallel for num_threads(p)
            for(int i=0; i<X.size();i++)
            {
                if(V_Info[i].is_done) {continue;}
                
               // int g1=V_Info[i].lcl_ID;
                //Do forward swwep to find communities
                for(int j=0;j<X[i].In_ListW.size();j++)
                {
                    if(X[i].In_ListW[j].second==0) {continue;} //do not propagate through deleted edge
                    
                    int n=X[i].In_ListW[j].first;
            
                    if(V_Info[n].fw<V_Info[i].fw)
                    {V_Info[n].fw=V_Info[i].fw;
                        change=true;}
                }//end of for j
                
                //Do forward swwep to find communities
                for(int j=0;j<X[i].Out_ListW.size();j++)
                {
                    if(X[i].Out_ListW[j].second==0) {continue;} //do not propagate through deleted edge
                    
                    int n=X[i].Out_ListW[j].first;
                    
                    if(V_Info[n].bw<V_Info[i].bw)
                    {V_Info[n].bw=V_Info[i].bw;
                        change=true;}
                }//end of for j
                
            }//end of for i
            
        }//end of while
 
#pragma omp parallel for num_threads(p)
        for(int i=0; i<X.size();i++)
        {
            //printf("heer \n");
            if(V_Info[i].is_done) {continue;}
            int g1=V_Info[i].lcl_ID;
           // printf("%d  %d %d||\n", i, V_Info[i].fw, V_Info[i].bw);
            if(V_Info[i].fw==V_Info[i].bw)
            {V_Info[i].is_done=true;}
            else
            { V_Info[i].fw=CX_max[g1];
                V_Info[i].bw=CX_max[g1];
                changeX=true;
            }
            // printf("---%d  %d %d||\n", i, V_Info[i].fw, V_Info[i].bw);
        }
    }//end of while
    q=clock()-q;
    cout << "Total Time for Reading Changed Edges "<< ((float)q)/CLOCKS_PER_SEC <<"\n";
   /* for(int i=0;i<X.size();i++)
    {
        printf("%d  %d %d|| :%d %d %d\n", i, V_Info[i].fw, V_Info[i].bw, V_Info[i].is_done, V_Info[i].lcl_ID, V_Info[i].glb_ID);
    }*/
    
	return 0;
}//end of main
	

