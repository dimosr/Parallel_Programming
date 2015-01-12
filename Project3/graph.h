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
 *  graph.h -- Graph-related declarations
 *  Copyright (C) 2010-2014, Computing Systems Laboratory (CSLab)
 */ 

#ifndef GRAPH_H__
#define GRAPH_H__

#include "common.h"
#include <stddef.h>

#define WEIGHT_MAX  100L

#ifdef USE_DP_VALUES
typedef double weight_t;
#else
typedef float weight_t;
#endif

BEGIN_C_DECLS__

typedef struct graph_struct {
    weight_t **weights;
    size_t nr_vertices;
    size_t nr_edges;
    size_t i_off, j_off;
} graph_t;

#ifndef CPU_TILE_DIM
#   define CPU_TILE_DIM 32
#endif
#ifndef GPU_TILE_DIM
#   define GPU_TILE_DIM 32
#endif

/* Graph routines */
graph_t *graph_create(size_t nr_vertices, size_t nr_edges);
void graph_init_rand(graph_t *g);
void graph_delete(graph_t *g);
void graph_print(const graph_t *g);
graph_t *graph_copy(const graph_t *g);
int graph_equals(const graph_t *g1, const graph_t *g2);
graph_t *graph_apsp_base(graph_t *a, graph_t *b, graph_t *c);
graph_t *graph_apsp_base_omp(graph_t *a, graph_t *b, graph_t *c);
graph_t *graph_apsp_tile(graph_t *g);

/* Used by the tiled FW implementation */
graph_t **graph_extract_tiles(const graph_t *g, size_t bsize);

/* Kernel selection framework */
#define __MAKE_KERNEL_NAME(arch, name)  apsp ## arch ## name
#define MAKE_KERNEL_NAME(arch, name)    __MAKE_KERNEL_NAME(arch, name)
#define DECLARE_KERNEL(arch, name) \
    graph_t *MAKE_KERNEL_NAME(arch, name)(graph_t *g)

typedef graph_t *(*apsp_kernel_fn_t)(graph_t *g);
typedef struct {
    const char *descr;
    apsp_kernel_fn_t fn;
} apsp_kernel_t;

enum {
    CPU_OMP_NAIVE = 0,
    CPU_OMP_TILED,
    GPU_NAIVE,
    GPU_TILED,
    GPU_TILED_SHMEM,
    KERNEL_END
};

DECLARE_KERNEL(_cpu, _omp_naive);
DECLARE_KERNEL(_cpu, _omp_tiled);
DECLARE_KERNEL(_gpu, _naive);
DECLARE_KERNEL(_gpu, _tiled);
DECLARE_KERNEL(_gpu, _tiled_shmem);

static apsp_kernel_t apsp_kernels[] = {
    {
        "cpu omp naive",
        MAKE_KERNEL_NAME(_cpu, _omp_naive),
    },

    {
        "cpu omp tiled",
        MAKE_KERNEL_NAME(_cpu, _omp_tiled),
    },

    {
        "gpu naive",
        MAKE_KERNEL_NAME(_gpu, _naive),
    },

    {
        "gpu tiled",
        MAKE_KERNEL_NAME(_gpu, _tiled),
    },

    {
        "gpu tiled shmem",
        MAKE_KERNEL_NAME(_gpu, _tiled_shmem),
    },
};

END_C_DECLS__

#endif  /* GRAPH_H__ */
