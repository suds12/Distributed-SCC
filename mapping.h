//
// Created by sriramsrinivas@unomaha.edu on 5/22/20.
//

#ifndef BHOWMICKPR_MAPPING_H
#define BHOWMICKPR_MAPPING_H

#include <string>
#include <iostream>
#include<vector>
#include <fstream>
#include<map>
using namespace std;
typedef std::pair<int, int> int_int;
void mapping(char * file1, char * file2, char * file3) {

    char line[128];
    FILE *graph_file;
    graph_file = fopen(file1, "r");

    int counter = 0;
    int linenumber = 0;
    std::vector <int_int> map1;
    bool firstbreak = false;
    int_int value;
    int maxNode = 0;
    int countVertices = 0;
    int firstMissing = 0;
    ofstream outputfile;
    outputfile.open(file3);
    std::map<int, int> graphMap;

    while (fgets(line, 128, graph_file) != NULL) {
        int n1, n2;
//Read line
        sscanf(line, "%d %d", &n1, &n2);
        if (linenumber != 0) {
            if (n1 == counter) {


            } else {
                countVertices++;
                int p = counter + 1;
                if (n1 == p) {
                    counter = p;


                } else if (firstbreak == false) {
                    firstbreak = true;
                    counter = n1;
                    firstMissing = p;
                    value.first = n1;
                    value.second = p;

                } else {
                    counter = n1;
                }


            }
            if (maxNode < n1)
                maxNode = n1;
            if (maxNode < n2)
                maxNode = n2;


        }


        linenumber++;


    }

    counter = value.first;
    int mapValue = value.second;
    int i = 0;

    while (i < counter) {
        if (i < firstMissing) {
            value.first = i;
            value.second = i;
            graphMap.insert(std::pair<int, int>(value.first, value.second));

        }
        i++;
    }

    ofstream myfile;
    myfile.open(file2);


    while (counter <= maxNode) {

        value.first = counter++;
        value.second = mapValue++;
        graphMap.insert(std::pair<int, int>(value.first, value.second));
    }


    std::cout << linenumber << "\n";

    std::map<int, int>::iterator itr;

    for (itr = graphMap.begin(); itr != graphMap.end(); ++itr) {
       myfile << '\t' << itr->first
             << '\t' << itr->second << '\n';
    }


    cout << maxNode;
    cout << firstMissing << "\n";

    graph_file = fopen("file_0.txt", "r");
    outputfile << maxNode << " " <<linenumber<<"\n";

    linenumber = 0;
    while (fgets(line, 128, graph_file) != NULL) {
        int n1, n2;
        if (linenumber != 0) {
            sscanf(line, "%d %d", &n1, &n2);

            outputfile << graphMap.at(n1) << " " << graphMap.at(n2) << "\n";
        }
        linenumber++;

    }

    outputfile.close();

}

struct mappingoutput{
    vector<int_int> outputGraphVector;
    map<int,int> graphMap;
};
typedef struct mappingoutput Struct;
Struct mappingZeroIndexing(vector<int_int> initialGraphVector)
{

      Struct outputStruct;

    map<int, int> initialGraph;
//    map<int,int> graphMap;


//    const auto p = std::minmax_element(initialGraphVector.begin(), initialGraphVector.end());
//    auto min = p.first->first;
//    auto max = p.second->first;
//    std::cout << "min=" << min << ", max=" << max << std::endl;

    int min=initialGraphVector.at(0).first;
    int max=0;
    for(int i =0;i<initialGraphVector.size();i++)
    {
        if(min>initialGraphVector.at(i).first)
            min=initialGraphVector.at(i).first;
        if(min>initialGraphVector.at(i).second)
            min=initialGraphVector.at(i).second;

        if(max<initialGraphVector.at(i).first)
            max=initialGraphVector.at(i).first;

        if(max<initialGraphVector.at(i).first)
            max=initialGraphVector.at(i).first;

    }

    std::cout << "min=" << min << ", max=" << max << std::endl;

    int counter=0;
    for (int i=min;i<=max;i++)
    {
        outputStruct.graphMap.insert(std::pair<int, int>(i,counter));
        counter++;
    }
    std::map<int, int>::iterator itr;

//    for (itr = graphMap.begin(); itr != graphMap.end(); ++itr) {
//        cout << '\t' << itr->first
//             << '\t' << itr->second << '\n';
//    }
//
//    for(int i=0;i<initialGraphVector.size();i++)
//    {
//        cout<<initialGraphVector.at(i).first<<" "<<initialGraphVector.at(i).second<<"\n";
//    }

    outputStruct.outputGraphVector.push_back(std::pair<int, int>(outputStruct.graphMap.at(max)+1,initialGraphVector.size()));

    for(int i=0;i<initialGraphVector.size();i++)
    {
        outputStruct.outputGraphVector.push_back(std::pair<int, int>(outputStruct.graphMap.at(initialGraphVector.at(i).first),outputStruct.graphMap.at(initialGraphVector.at(i).second) ));
    }



    return outputStruct;

}



int mapKey(Struct outputStruct, int someValue) {
    for (auto const& x  : outputStruct.graphMap)
        if (x.second == someValue)
            return x.first;
}

#endif //BHOWMICKPR_MAPPING_H
