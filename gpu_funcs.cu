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

	cudaMallocManaged(&dst, size*sizeof(int));
	cudaMemcpy(dst, src, size*sizeof(int), cudaMemcpyHostToDevice);
	//cudaMemcpy(temp, dst, size*sizeof(int), cudaMemcpyDeviceToHost);
}

void cuda_d2h(int* src, int* dst, int size)
{
	//cudaMallocManaged(&src, size*sizeof(int));
	cudaMemcpy(src, dst, size*sizeof(int), cudaMemcpyDeviceToHost);
	
	
}

void deallocate_device_mem(int* ptr)
{
	cudaFree(ptr);

} 

void cuda_sync()
{
	cudaDeviceSynchronize();
}





