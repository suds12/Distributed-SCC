
Set the path to ESSENS directory in the Makefile
Use the ESSENS included with this zip, since it contains data structures for directed graphs.

Compile The Codes
make all
make cE
make sparse


Create RMAT File using generator code
Delete Duplicate Edges
awk '{if($1< $2) print $1" " $2; else print $2" "$1 }' RMAT18_all.txt| sort -n -k1 -k2 |uniq  > tmp
mv tmp RMAT18ER

INPUTS for Sparsification. 
1. Graph: File format should be n1 n2 (Note:Only TWO Columns. No edge weights). Vertices should start from 0.The first line is # of vertices # of edges
2. SCC map of vertices: One column of integers. If value in row i is x, then vertex i is in row x.

 ./sparse.out <Graph> <SCC_map>

OUTPUT will be the sparsified graph . Here each SCC is represented by a cycle, and a few more edges if exact cycle cannot be determined.
OUTPUT is printed in SparseCC



===================================

Create set of changed Edges
./cE.out <Graph> <number of edges>  <Percentage of inserted edges (0 to 100)> ChangeE



========================
Run the update
./a.out
FIVE inputs required. 
First: Original Graph File (first line: node# edge#) then n1 n2 (no edge weights). 
Second: SCC map of each node. 
Third: Sparse Representation of SCC. 
Fourth: List of Edges to be Updated. 
Fifth: number of threads\n";

./a.out<Graph> <SCC_map> <SparseCC> <ChangeE>



