#!/usr/bin/python
import sys
import random
# python3 edge_list_gen.py -edges 10 -nodes 5 -process 2


def main(argList):
        if '-process' in argList:
                fin = open("r_graph_process.txt", "w")

        else:
                fin = open("r_graph.txt", "w")

        if '-edges' in argList:
                a = argList.index('-edges')
                edges = int(argList[a+1])
        else:
                edges = 512

        if '-nodes' in argList:
                a = argList.index('-nodes')
                nodes=int(argList[a+1])
        else:
                nodes = 128
        if '-process' in argList:
                a = argList.index('-process')
                processes = int(argList[a+1])
        else:
                processes = 0

        if '-seed' in argList:
                a = argList.index('-seed')
                seed = int(argList[a+1])
        else:
                seed = 1048576

        for i in genPair(nodes,edges,seed, processes):
                fin.write(i)
                fin.write('\n')


def genPair(nodes, edges, seed, processes):
        neighbors = 15
        graph = set()
        nodeList = range(1, nodes+1)
        processList = range(processes)
        neighborList = range(7, neighbors)
        random.seed(seed)
        if processes == 0:
                while len(graph) < edges:
                        pair = "%d\t%d" % (int(random.choice(nodeList)), int(random.choice(nodeList)))
                        graph.add(pair)
        else:
                for i in range(1, nodes+1):
                        triplet = "%d\t%d\t%d" % (i, int(random.choice(neighborList)),
                                                int(random.choice(processList)))
                        graph.add(triplet)


        return  graph

if __name__ == '__main__':
        main(sys.argv[1:])
