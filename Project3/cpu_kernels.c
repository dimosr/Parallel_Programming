/*
 *  cpu_kernels.c -- CPU kernels
 *
 *  Copyright (C) 2014, Computing Systems Laboratory (CSLab)
 *  Copyright (C) 2014, Vasileios Karakasis
 */ 

#include "graph.h"

graph_t *MAKE_KERNEL_NAME(_cpu, _omp_naive)(graph_t *graph)
{
    return graph_apsp_base_omp(graph, graph, graph);
}

graph_t *MAKE_KERNEL_NAME(_cpu, _omp_tiled)(graph_t *graph)
{
    return graph_apsp_tile(graph);
}

