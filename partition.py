#!/usr/bin/env python3

import networkx as nx
import sys
import metis
import argparse
import multiprocessing as mp


# Parse command-line arguments
parser = argparse.ArgumentParser(description='Generate partitioning for a graph using Metis.')
parser.add_argument('--graph', type=str, help='Input graph file (weighted or unweighted) in edge list format', required=True)
parser.add_argument('--nparts', type=int, help='Number of partitions to generate', required=True)
args = parser.parse_args()

# Check the format of the input graph (just first 5 lines)
inputgraph = args.graph
with open(inputgraph) as myfile:
    head = [next(myfile) for x in range(5)]
nvals = len(head[-1].split())    # The number of whitespace-separated entries per input line

print("Reading graph...")
fh=open(inputgraph, 'rb')
#G=nx.read_weighted_edgelist(fh)
if nvals == 3: 
  G = nx.read_edgelist(inputgraph, nodetype=int, data=(('weight',int),))
else:
  G = nx.read_edgelist(inputgraph, nodetype=int, data=(('weight',int),('weight2',int)),)
fh.close()
#nx.write_multiline_adjlist(G, inputgraph+'.metis')

print("Partitioning graph...")
(edgecuts, parts) = metis.part_graph(G, args.nparts)

print("Generating partition mapping...")
# Using map() and lambda 
mapping = list(map(lambda x, y:(x,y), parts, G.nodes)) 

#mapping = dict(zip(parts, G.nodes)) 
print(mapping[:100])

