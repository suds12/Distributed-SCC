#include <iostream>

#include <thrust/copy.h>
#include <thrust/reduce.h>
#include <thrust/sequence.h>

#include <stdgpu/memory.h>          // createDeviceArray, destroyDeviceArray
#include <stdgpu/iterator.h>        // device_begin, device_end
#include <stdgpu/platform.h>        // STDGPU_HOST_DEVICE
#include <stdgpu/unordered_map.cuh> // stdgpu::unordered_map
#include <stdgpu/vector.cuh>        // stdgpu::vector

#include "kernals.h"



void cuda_h2d(int* src, int* dst, int size)
{

	cudaMalloc(&dst, size*sizeof(int));
	cudaMemcpy(dst, src, size*sizeof(int), cudaMemcpyHostToDevice);



}

void deallocate_device_mem(int* ptr)
{
	cudaFree(ptr);

}

void cuda_sync()
{
	cudaDeviceSynchronize();
}

void create_MN_vector(int* meta_nodes, int size)
{
	cudaMalloc(&meta_nodes, size*sizeof(int));

	
}