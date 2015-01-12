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
 *  gpu_util.h -- GPU utility functions
 *  Copyright (C) 2010-2013, Computing Systems Laboratory (CSLab)
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
