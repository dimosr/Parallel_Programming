/*
 *  gpu_util.h -- GPU utility functions
 *
 *  Copyright (C) 2010-2013, Computing Systems Laboratory (CSLab)
 *  Copyright (C) 2010-2013, Vasileios Karakasis
 */ 

#ifndef GPU_UTIL_H__
#define GPU_UTIL_H__

#include "common.h"
#include <cuda_runtime.h>

BEGIN_C_DECLS__

// Number of GPU multiprocessors
#define NR_GPU_MP 14    // Tesla M2050

void gpu_init();
void *gpu_alloc(size_t count);
void gpu_free(void *gpuptr);
int copy_to_gpu(const void *host, void *gpu, size_t count);
int copy_from_gpu(void *host, const void *gpu, size_t count);
const char *gpu_get_errmsg(cudaError_t err);
const char *gpu_get_last_errmsg();

END_C_DECLS__

#endif  /* GPU_UTIL_H__ */
