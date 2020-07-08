#!/bin/bash

NCORES=4
unamestr=`uname`
if [[ "$unamestr" == "Linux" ]]; then
        NCORES=`grep -c ^processor /proc/cpuinfo`
fi

if [[ "$unamestr" == "Darwin" ]]; then
        NCORES=`sysctl -n hw.ncpu`
fi

rm -rf deploy
rm -rf build
mkdir build
cd build
cmake ../
make -j $NCORES
cd ..

mkdir deploy
cp ./build/kaffpa deploy/
cp ./build/evaluator deploy/
cp ./build/label_propagation deploy/
cp ./build/kaffpaE deploy/
cp ./build/graphchecker deploy/
cp ./build/partition_to_vertex_separator deploy/
cp ./build/node_separator deploy/
cp ./build/edge_partitioning deploy/
cp ./build/libinterface_static.a deploy/libkahip.a
cp ./build/parallel/parallel_src/dsp* ./deploy/distributed_edge_partitioning
cp ./build/parallel/parallel_src/g* ./deploy
cp ./build/parallel/parallel_src/parhip* ./deploy/parhip
cp ./build/parallel/parallel_src/toolbox* ./deploy/
cp ./build/parallel/parallel_src/libparhip_inter*.a deploy/libparhip.a
cp ./interface/kaHIP_interface.h deploy/
cp ./parallel/parallel_src/interface/parhip_interface.h deploy/

mkdir deploy/parallel
cp ./build/parallel/modified_kahip/lib*.a deploy/parallel/libkahip.a

