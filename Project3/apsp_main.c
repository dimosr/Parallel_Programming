/*
 *  apsp_main.c -- APSP front-end program
 *
 *  Copyright (C) 2010-2014, Computing Systems Laboratory (CSLab)
 *  Copyright (C) 2010-2014, Vasileios Karakasis
 */ 

#include <stdlib.h>
#include <stdio.h>
#include "alloc.h"
#include "error.h"
#include "gpu_util.h"
#include "graph.h"
#include "timer.h"
/*
static void report_results(xtimer_t *timer, size_t nr_edges,char *kernel,size_t nr_vertices);
static void print_usage();
static void check_result(const graph_t *test, const graph_t *orig);
*/
static void check_result(const graph_t *test, const graph_t *orig)
{
    printf("Checking ... ");
    fflush(stdout);
    FILE *f = fopen("2.1-Disdiastata.txt","a");//check line 46
    if (f == NULL)
    {
    printf("Error opening file!\n");
    exit(1);
    }
    if (!graph_equals(test, orig)) {
        printf("FAILED\n");
        fprintf(stderr, "Dist graphs not equal\n");
	fprintf(f,"FAILED\n");
	fclose(f);
        exit(1);
    } else {
        printf("PASSED\n");
	fprintf(f,"\nPASSED\n");
	fclose(f);
    }

}
static void report_results(xtimer_t *timer, size_t nr_edges,char *kernel,size_t nr_vertices)
{
    double elapsed_time = timer_elapsed_time(timer);
    FILE *f = fopen("2.1-Disdiastata.txt","a");//check line 24
    if (f == NULL)
    {
    printf("Error opening file!\n");
    exit(1);
    }
    fprintf(f,"\nKernel=%s\nnr_edges=%d\n",kernel,nr_vertices);
    fprintf(f,"GPU_TILE_SIZE= %d ,CPU_TILE_SIZE = %d\n",GPU_TILE_DIM,CPU_TILE_DIM);	    
    fprintf(f,"Elapsed time: %lf s\n", elapsed_time);
    fprintf(f,"Performance:  %lf Medges/s\n", nr_edges*1.e-6 / elapsed_time);
    fclose(f);
}

static void print_usage()
{
    printf("Usage: [KERNEL=<kernel_no>] "
           "%s <graph vertices>\n", program_name);
    printf("KERNEL defaults to 0\n");
    printf("Available kernels [id:descr]:\n");
    size_t i;
    for (i = 0; i < KERNEL_END; ++i)
        printf("\t%zd:%s\n", i, apsp_kernels[i].descr);
}

int main(int argc, char **argv)
{
    set_program_name(argv[0]);
    if (argc < 2) {
        warning(0, "too few arguments");
        print_usage();
        exit(EXIT_FAILURE);
    }

    size_t nr_vertices = atoi(argv[1]);
    if (!nr_vertices)
        error(0, "invalid argument: %s", argv[1]);

    printf("Adjusting vertices number to a multiple of tile dimensions ... ");
    fflush(stdout);
    long lcm = llcm(CPU_TILE_DIM, GPU_TILE_DIM);
    nr_vertices = lceil(nr_vertices, lcm)*lcm;
    printf("%ld\n", nr_vertices);

    char *kernel_env = getenv("KERNEL");
    size_t kernel_id = 0;
    if (kernel_env)
        kernel_id = atoi(kernel_env);

    if (kernel_id >= GPU_NAIVE) {
        // Initialize runtime, so as not to pay the cost at the first call
        printf("Initializing CUDA runtime ... ");
        fflush(stdout);
        gpu_init();
        printf("DONE\n");
    }

    graph_t *graph = graph_create(nr_vertices, 0);
    graph_init_rand(graph);
    //printf("\n******Initialized Graph*******\n");
    //graph_print(graph);
#ifndef _NOCHECK
    graph_t *check_graph = graph_copy(graph);
    MAKE_KERNEL_NAME(_cpu, _omp_tiled)(check_graph);
#endif

    /* Run and time the selected kernel  */
    printf("Launching kernel: %s\n", apsp_kernels[kernel_id].descr);
    xtimer_t timer;
    timer_clear(&timer);
    timer_start(&timer);
    apsp_kernels[kernel_id].fn(graph);
    timer_stop(&timer);
#ifndef _NOCHECK
    check_result(graph, check_graph);
#endif
    report_results(&timer, graph->nr_edges,apsp_kernels[kernel_id].descr,nr_vertices);
    graph_delete(graph);
#ifndef _NOCHECK
    graph_delete(check_graph);
#endif
    return EXIT_SUCCESS;
}
