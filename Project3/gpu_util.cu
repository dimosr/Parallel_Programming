/**************************************************
# Copyright (C) 2014 Raptis Dimos <raptis.dimos@yahoo.gr>
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
# **************************************************/

/*
 *  gpu_util.cu -- GPU utility functions
 *  Copyright (C) 2010-2013, Computing Systems Laboratory (CSLab)
 */ 

#include <cuda.h>
#include "gpu_util.h"

/* Initialize the CUDA runtime */
void gpu_init()
{
    cudaFree(0);
}

void *gpu_alloc(size_t count)
{
    void *ret;
    if (cudaMalloc(&ret, count) != cudaSuccess) {
        ret = NULL;
    }

    return ret;
}

void gpu_free(void *gpuptr)
{
    cudaFree(gpuptr);
}

int copy_to_gpu(const void *host, void *gpu, size_t count)
{
    if (cudaMemcpy(gpu, host, count, cudaMemcpyHostToDevice) != cudaSuccess)
        return -1;
    return 0;
}

int copy_from_gpu(void *host, const void *gpu, size_t count)
{
    if (cudaMemcpy(host, gpu, count, cudaMemcpyDeviceToHost) != cudaSuccess)
        return -1;
    return 0;
}

const char *gpu_get_errmsg(cudaError_t err)
{
    return cudaGetErrorString(err);
}

const char *gpu_get_last_errmsg()
{
    return gpu_get_errmsg(cudaGetLastError());
}
    
