/*
 *  graph.h -- Graph implementation
 *
 *  Copyright (C) 2010-2014, Computing Systems Laboratory (CSLab)
 *  Copyright (C) 2010-2014, Vasileios Karakasis
 */ 

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "alloc.h"
#include "graph.h"

graph_t *graph_create(size_t nr_vertices, size_t nr_edges)
{
    graph_t *ret = (graph_t *) malloc(sizeof(*ret));
    if (ret) {
        ret->nr_vertices = nr_vertices;
        ret->nr_edges = (nr_edges) ? nr_edges : nr_vertices*nr_vertices;
        ret->weights = (weight_t **) calloc_2d(nr_vertices, nr_vertices,
                                               sizeof(weight_t));
        ret->i_off = ret->j_off = 0;
        if (!ret->weights) {
            free(ret);
            ret = NULL;
        }
    }

    return ret;
}

void graph_init_rand(graph_t *g)
{
    srand48(0);
    for (size_t i = 0; i < g->nr_vertices; ++i)
        for (size_t j = 0; j < g->nr_vertices; ++j)
            g->weights[i][j] = WEIGHT_MAX*drand48();

    /* Zero-out the diagonal */
    for (size_t i = 0; i < g->nr_vertices; ++i)
        g->weights[i][i] = 0;

    /* Erase non-existent edges */
    size_t nr_non_edges = g->nr_vertices*g->nr_vertices - g->nr_edges;
    size_t non_edge_cnt = 0;
    while (non_edge_cnt != nr_non_edges) {
        size_t i_del = (size_t) (drand48()*g->nr_vertices);
        size_t j_del = (size_t) (drand48()*g->nr_vertices);
        if (g->weights[i_del][j_del] != WEIGHT_MAX) {
            g->weights[i_del][j_del] = WEIGHT_MAX;
            ++non_edge_cnt;
        }
    }
}

void graph_delete(graph_t *g)
{
    if (g) {
        if (g->weights)
            free_2d((void **) g->weights);
    }

    free(g);
}

void graph_print(const graph_t *g)
{
    printf("Number of vertices: %ld\n", g->nr_vertices);
    printf("Number of edges:    %ld\n", g->nr_edges);
    printf("Non-edge weight:    %ld\n", WEIGHT_MAX);
    printf(">>> Adjacency matrix <<<\n");
    for (size_t i = 0; i < g->nr_vertices; ++i) {
        for (size_t j = 0; j < g->nr_vertices; ++j)
            printf("%3.2lf ", g->weights[i][j]);
        printf("\n");
    }
}

graph_t *graph_copy(const graph_t *g)
{
    graph_t *ret = graph_create(g->nr_vertices, g->nr_edges);
    ret->i_off = g->i_off;
    ret->j_off = g->j_off;
    copy_2d((void **) ret->weights, (const void **) g->weights,
            ret->nr_vertices, ret->nr_vertices, sizeof(**ret->weights));

    return ret;
}

int graph_equals(const graph_t *g1, const graph_t *g2)
{
    if (g1->nr_vertices != g2->nr_vertices ||
        g1->nr_edges    != g2->nr_edges)
        return 0;

    for (size_t i = 0; i < g1->nr_vertices; ++i)
        for (size_t j = 0; j < g2->nr_vertices; ++j)
            if (ABS(g1->weights[i+g1->i_off][j+g1->j_off] -
                    g2->weights[i+g2->i_off][j+g2->j_off]) > EPS) {
	      //                printf("I was checking the graph that omp_naive creates versus your graph!As i was checking the element [%d][%d] i found that %f != %f so the graphs are not the same!\n",i,j,g1->weights[i+g1->i_off][j+g1->j_off],g2->weights[i+g2->i_off][j+g2->j_off]);
                return 0;
            }

    return 1;
}

/*
 *  All-pairs shortest path using the iterative Floyd-Warshall algorithm.
 */ 
graph_t *graph_apsp_base(graph_t *a, graph_t *b, graph_t *c)
{
    weight_t **da = a->weights;
    weight_t **db = b->weights;
    weight_t **dc = c->weights;

    size_t ai_off = a->i_off;
    size_t aj_off = a->j_off;
    size_t bi_off = b->i_off;
    size_t bj_off = b->j_off;
    size_t ci_off = c->i_off;
    size_t cj_off = c->j_off;
    size_t n = a->nr_vertices;
    for (size_t k = 0; k < n; ++k)
        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < n; ++j)
                da[i+ai_off][j+aj_off] =
                    MIN(da[i+ai_off][j+aj_off],
                        db[i+bi_off][k+bj_off] + dc[k+ci_off][j+cj_off]);

    return a;
}

graph_t *graph_apsp_base_omp(graph_t *a, graph_t *b, graph_t *c)
{
    weight_t **da = a->weights;
    weight_t **db = b->weights;
    weight_t **dc = c->weights;

    size_t ai_off = a->i_off;
    size_t aj_off = a->j_off;
    size_t bi_off = b->i_off;
    size_t bj_off = b->j_off;
    size_t ci_off = c->i_off;
    size_t cj_off = c->j_off;
    
    for (size_t k = 0; k < a->nr_vertices; ++k)
#pragma omp parallel for
        for (size_t i = 0; i < a->nr_vertices; ++i)
#pragma omp parallel for
            for (size_t j = 0; j < a->nr_vertices; ++j)
                da[i+ai_off][j+aj_off] =
                    MIN(da[i+ai_off][j+aj_off],
                        db[i+bi_off][k+bj_off] + dc[k+ci_off][j+cj_off]);
    return a;
}

graph_t **graph_extract_tiles(const graph_t *g, size_t bsize)
{
    size_t nr_tiles_r = g->nr_vertices / bsize;
    graph_t **ret = (graph_t **) calloc_2d(nr_tiles_r, nr_tiles_r,
                                           sizeof(**ret));
    for (size_t i = 0; i < nr_tiles_r; ++i)
        for (size_t j = 0; j < nr_tiles_r; ++j) {
            ret[i][j].weights = g->weights;
            ret[i][j].nr_vertices = bsize;
            ret[i][j].nr_edges = bsize*bsize;
            ret[i][j].i_off = i*bsize;
            ret[i][j].j_off = j*bsize;
        }                

    return ret;
}

graph_t *graph_apsp_tile(graph_t *g)
{
    graph_t **tiles = graph_extract_tiles(g, CPU_TILE_DIM);
    size_t  nr_blocks = g->nr_vertices / CPU_TILE_DIM;

#pragma omp parallel
    {
    for (size_t bk = 0; bk < nr_blocks; ++bk) {
        graph_apsp_base(&tiles[bk][bk], &tiles[bk][bk], &tiles[bk][bk]);
        /* Compute the same block row and column*/
	#pragma omp master
	{
	  //Printf("\n___OMP___Stage1<->k=%d___OMP___\n",bk);
	//graph_print(g);
	}
#pragma omp for nowait
        for (size_t bi = 0; bi < nr_blocks; ++bi)
            if (bi != bk) {
                graph_apsp_base(&tiles[bi][bk], &tiles[bi][bk], &tiles[bk][bk]);
                graph_apsp_base(&tiles[bk][bi], &tiles[bk][bk], &tiles[bk][bi]);
            }
#pragma omp barrier    
	#pragma omp master
	{
	  //printf("\n___OMP___Stage2<->k=%d___OMP___\n",bk);    
        //graph_print(g);
	}
        /* Compute the rest of the matrix */
        for (size_t bi = 0; bi < nr_blocks; ++bi) {
            if (bi == bk)
                continue;
#pragma omp for nowait
            for (size_t bj = 0; bj < nr_blocks; ++bj)
                if (bj != bk)
                    graph_apsp_base(&tiles[bi][bj],
                                    &tiles[bi][bk], &tiles[bk][bj]);
        }
#pragma omp barrier
#pragma omp master
{
  //printf("\n___OMP___Stage3<->k=%d___OMP___\n",bk);
//graph_print(g);
}
    }

    }

    free_2d((void **) tiles);
    return g;
}
