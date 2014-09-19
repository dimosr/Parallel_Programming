/*
 *  gpu_kernels.cu -- GPU kernels
 *
 *  Copyright (C) 2014, Computing Systems Laboratory (CSLab)
 *  Copyright (C) 2014, Vasileios Karakasis
 */ 

#include <stdio.h>
#include <cuda.h>
#include "error.h"
#include "gpu_util.h"
#include "graph.h"
#include "timer.h"
#include <math.h>

#define GPU_KERNEL_NAME(name)   do_apsp_gpu ## name

weight_t *copy_graph_to_gpu(const graph_t *graph)
{
    size_t dist_size = graph->nr_vertices*graph->nr_vertices;
    weight_t *dist_gpu = (weight_t *) gpu_alloc(dist_size*sizeof(*dist_gpu));
    if (!dist_gpu)
        error(0, "gpu_alloc() failed: %s", gpu_get_last_errmsg());

    if (copy_to_gpu(graph->weights[0], dist_gpu,
                    dist_size*sizeof(*dist_gpu)) < 0)
        error(0, "copy_to_gpu() failed: %s", gpu_get_last_errmsg());

    return dist_gpu;
}

graph_t *copy_graph_from_gpu(const weight_t *dist_gpu, graph_t *graph)
{
    size_t dist_size = graph->nr_vertices*graph->nr_vertices;

    if (copy_from_gpu(graph->weights[0], dist_gpu,
                      dist_size*sizeof(*dist_gpu)) < 0)
        error(0, "copy_from_gpu() failed: %s", gpu_get_last_errmsg());

    return graph;
}

/*
 *  The naive GPU kernel
 */ 
__global__ void GPU_KERNEL_NAME(_naive)(weight_t *dist, int n, int k)
{
	int i_equal,j_equal;
    // FILLME: the naive GPU kernel code
	
	/* if Grid & blocks are 1-dimensional */
	//i_equal = (blockIdx.x*blockDim.x + threadIdx.x) / n;
        //j_equal = (blockIdx.x*blockDim.x + threadIdx.x) % n;
	//check 266
	/* if Grid & block  are 2-dimensional */
	i_equal = blockIdx.y*blockDim.y + threadIdx.y;
	j_equal = blockIdx.x*blockDim.x + threadIdx.x;

        if (dist[(i_equal*n+j_equal)] > dist[(i_equal*n+k)] + dist[(n*k+j_equal)]){
        	dist[(i_equal*n+j_equal)] = dist[(i_equal*n+k) ] + dist[(n*k+j_equal)];
        }
   
}


/*
 *  The tiled GPU kernel(s) using global memory
 */ 
__global__ void GPU_KERNEL_NAME(_tiled_stage_1)(weight_t *dist, int n,
                                                int k_tile, int tile_dim)
{
    int i,j,inner_k;          //inner_k used for iterating inside the tile
    

    int i_equal = threadIdx.y;
    int j_equal = threadIdx.x;      //i,j indexes inside the tile 
    i = k_tile*blockDim.y + threadIdx.y;
    j = k_tile*blockDim.x + threadIdx.x;	    //i,j in the whole array
    int index = i*n + j;				   //index in the 1D array 
    for(inner_k=0; inner_k<tile_dim; inner_k++){
            if( dist[index] > dist[index - (j_equal - inner_k)] + dist[index - (i_equal - inner_k)*n] ){
               	dist[index] = dist[index - (j_equal - inner_k)] + dist[index - (i_equal - inner_k)*n];
            }
            __syncthreads();        
    }
	
}
    

__global__ void GPU_KERNEL_NAME(_tiled_stage_2)(weight_t *dist, int n,
                                                int k_tile, int tile_dim)
{
    int i,j,inner_k;             //inner_k used for iterating inside the tile
    
    if( (blockIdx.y == 0) && (blockIdx.x != k_tile) ){//if the running block belongs to the vertical line (1st semi-half of grid)  and not in the (k,k) tile
            int i_equal = threadIdx.y;
    	    int j_equal = threadIdx.x;
	    i = blockIdx.x*blockDim.y + threadIdx.y;
	    j = k_tile*blockDim.x + threadIdx.x;
	    int index = i*n + j;
	    for(inner_k=0; inner_k<tile_dim; inner_k++){        
                if( dist[index] > dist[index - (j_equal - inner_k)] + dist[index - (i_equal - inner_k)*n - (blockIdx.x - k_tile)*blockDim.x*n]){
                    dist[index] = dist[index - (j_equal - inner_k)] + dist[index - (i_equal - inner_k)*n - (blockIdx.x - k_tile)*blockDim.x*n];
                }
                __syncthreads();
            }
    }
    if( (blockIdx.y == 1) && (blockIdx.x != k_tile) ){     //if the running block belongs to the horizontal line (2nd semi-half of grid)  and not in the (k,k) tile
     	int i_equal = threadIdx.y;
    	int j_equal = threadIdx.x;
	i = k_tile*blockDim.y + threadIdx.y;
        j = blockIdx.x*blockDim.x + threadIdx.x;
        int index = i*n + j;
    	for(inner_k=0; inner_k<tile_dim; inner_k++){
          	if( dist[index] > dist[index - (j_equal - inner_k) - (blockIdx.x - k_tile)*blockDim.x] + dist[index - (i_equal - inner_k)*n] ){
                       dist[index] = dist[index - (j_equal - inner_k) - (blockIdx.x - k_tile)*blockDim.x] + dist[index - (i_equal - inner_k)*n];
          	}
          	__syncthreads();
        }
    }
             
}

__global__ void GPU_KERNEL_NAME(_tiled_stage_3)(weight_t *dist, int n,
                                                int k_tile, int tile_dim)
{
    int i,j,inner_k;             //inner_k used for iterating inside the tile
    
    
    int i_equal = threadIdx.y;
    int j_equal = threadIdx.x;
    i = blockIdx.y*blockDim.y + threadIdx.y;
    j = blockIdx.x*blockDim.x + threadIdx.x;
    int index = i*n + j;	
    if( (blockIdx.x != k_tile) && (blockIdx.y != k_tile) ){       //this will be executed only by the blocks not belonging to the k_tile 
        for(inner_k=0; inner_k<tile_dim; inner_k++){
            if( dist[index] > dist[index - (j_equal - inner_k) - (blockIdx.x - k_tile)*blockDim.x] + dist[index - (i_equal - inner_k)*n - ( blockIdx.y - k_tile)*blockDim.y*n] ){
                dist[index] = dist[index - (j_equal - inner_k) - (blockIdx.x - k_tile )*blockDim.x] + dist[index - (i_equal - inner_k)*n - ( blockIdx.y - k_tile)*blockDim.y*n];
            }
        }
    }

}

__global__ void GPU_KERNEL_NAME(_tiled_shmem_stage_1)(weight_t *dist, int n,
                                                      int k_tile, int tile_dim)
{
    int i,j,i_equal,j_equal,inner_k;

    __shared__ weight_t local_tile[GPU_TILE_DIM][GPU_TILE_DIM];

    i_equal = threadIdx.y;      
    j_equal = threadIdx.x;      //i_equal,j_equal indexes inside the tile
    i = k_tile*blockDim.y + threadIdx.y;
    j = k_tile*blockDim.x + threadIdx.x;        //i,j indexes in the whole array
    int index = i*n + j;            //index in the 1D array
    local_tile[i_equal][j_equal] = dist[index]; 
    __syncthreads();
    for(inner_k = 0; inner_k < tile_dim; inner_k++){
            if( local_tile[i_equal][j_equal] > local_tile[i_equal][inner_k] + local_tile[inner_k][j_equal] ){
                local_tile[i_equal][j_equal] = local_tile[i_equal][inner_k] + local_tile[inner_k][j_equal];
            }
            __syncthreads();
    }
    dist[index] = local_tile[i_equal][j_equal];

}

__global__ void GPU_KERNEL_NAME(_tiled_shmem_stage_2)(weight_t *dist, int n,
                                                      int k_tile, int tile_dim)
{
    int i,j,i_equal,j_equal,inner_k;

    __shared__ weight_t local_KK_tile[GPU_TILE_DIM][GPU_TILE_DIM];
    __shared__ weight_t local_IK_tile[GPU_TILE_DIM][GPU_TILE_DIM];
    __shared__ weight_t local_KI_tile[GPU_TILE_DIM][GPU_TILE_DIM];

    if( (blockIdx.y == 0) && (blockIdx.x != k_tile) ){//if the running block belongs to the vertical line (1st semi-half of grid) and not in the (k,k) tile
	int i_equal = threadIdx.y;
        int j_equal = threadIdx.x;
        i = blockIdx.x*blockDim.y + threadIdx.y;
        j = k_tile*blockDim.x + threadIdx.x;
        int index = i*n + j;
        local_KK_tile[i_equal][j_equal] = dist[index - (blockIdx.x - k_tile)*blockDim.x*n];
        local_IK_tile[i_equal][j_equal] = dist[index];
	__syncthreads();
	    
        for(inner_k=0; inner_k<tile_dim; inner_k++){
            if( local_IK_tile[i_equal][j_equal] > local_IK_tile[i_equal][inner_k] + local_KK_tile[inner_k][j_equal] ){
                local_IK_tile[i_equal][j_equal] = local_IK_tile[i_equal][inner_k] + local_KK_tile[inner_k][j_equal];
            }
            __syncthreads();
        }
        dist[index] = local_IK_tile[i_equal][j_equal];
    }
    if( (blockIdx.y == 1) && (blockIdx.x != k_tile)  ){     //if the running block belongs to the horizontal line and not in the (k,k) tile
	int i_equal = threadIdx.y;
        int j_equal = threadIdx.x;
        i = k_tile*blockDim.y + threadIdx.y;
        j = blockIdx.x*blockDim.x + threadIdx.x;
        int index = i*n + j;
        local_KK_tile[i_equal][j_equal] = dist[index - (blockIdx.x - k_tile)*blockDim.x];
        local_KI_tile[i_equal][j_equal] = dist[index];
	__syncthreads();

        for(inner_k=0; inner_k<tile_dim; inner_k++){
            if( local_KI_tile[i_equal][j_equal] > local_KK_tile[i_equal][inner_k] + local_KI_tile[inner_k][j_equal] ){
                local_KI_tile[i_equal][j_equal] = local_KK_tile[i_equal][inner_k] + local_KI_tile[inner_k][j_equal];
            }
            __syncthreads();
        }

        dist[index] = local_KI_tile[i_equal][j_equal];
    }
}

__global__ void GPU_KERNEL_NAME(_tiled_shmem_stage_3)(weight_t *dist, int n,
                                                      int k_tile, int tile_dim)
{
    int i,j,i_equal,j_equal,inner_k;

    __shared__ weight_t local_IJ_tile[GPU_TILE_DIM][GPU_TILE_DIM];
    __shared__ weight_t local_IK_tile[GPU_TILE_DIM][GPU_TILE_DIM];
    __shared__ weight_t local_KJ_tile[GPU_TILE_DIM][GPU_TILE_DIM];

    i_equal = threadIdx.y;      
    j_equal = threadIdx.x;      //i_equal,j_equal indexes inside the tile
    i = blockIdx.y*blockDim.y + threadIdx.y;
    j = blockIdx.x*blockDim.x + threadIdx.x;        //i,j indexes in the whole array
    int index = i*n + j; 
    if( (blockIdx.x != k_tile) && (blockIdx.y != k_tile) ){       //this will be executed only by the blocks not belonging to the k_tile
        
        local_IJ_tile[i_equal][j_equal] = dist[index];
        local_IK_tile[i_equal][j_equal] = dist[index - (blockIdx.x - k_tile)*blockDim.x];
        local_KJ_tile[i_equal][j_equal] = dist[index - (blockIdx.y - k_tile)*blockDim.y*n];
	__syncthreads();

        for(inner_k=0; inner_k<tile_dim; inner_k++){
            if( local_IJ_tile[i_equal][j_equal] > local_IK_tile[i_equal][inner_k] + local_KJ_tile[inner_k][j_equal] ){
                local_IJ_tile[i_equal][j_equal] = local_IK_tile[i_equal][inner_k] + local_KJ_tile[inner_k][j_equal];
            }
        }

        dist[index] = local_IJ_tile[i_equal][j_equal];
    }

}


graph_t *MAKE_KERNEL_NAME(_gpu, _naive)(graph_t *graph)
{
    xtimer_t transfer_timer;
    timer_clear(&transfer_timer);
    timer_start(&transfer_timer);
    weight_t *dist_gpu = copy_graph_to_gpu(graph);
    timer_stop(&transfer_timer);

    /* FILLME: Set up and launch the kernel(s) */
    int block_dim = 32;
    int k;


    /* 1-Dimension Grid */
    //dim3 dimBlock(block_dim*block_dim);
    //dim3 dimGrid( (graph->nr_vertices/block_dim)*(graph->nr_vertices/block_dim) );
    //check 54    
    /* 2-Dimensions Grid */
    dim3 dimBlock(block_dim,block_dim);
    dim3 dimGrid( (graph->nr_vertices/block_dim),(graph->nr_vertices/block_dim) );


    for(k=0; k<graph->nr_vertices; k++){         //NOTICE: in CUDA the whole array is represented as 1D (nxn size)
        GPU_KERNEL_NAME(_naive) <<< dimGrid, dimBlock >>> (dist_gpu, graph->nr_vertices,k);         
        cudaThreadSynchronize();//wait for the last kernel to finish and then go to next k for right results
    }

    /*
     * Wait for last kernel to finish, so as to measure correctly the
     * transfer times Otherwise, copy from GPU will block
     */

    /* Copy back results to host */
    timer_start(&transfer_timer);
    copy_graph_from_gpu(dist_gpu, graph);
    timer_stop(&transfer_timer);
    printf("Total transfer times: %lf s\n",
           timer_elapsed_time(&transfer_timer));
    return graph;
}

graph_t *MAKE_KERNEL_NAME(_gpu, _tiled)(graph_t *graph)
{
    xtimer_t transfer_timer;
    timer_clear(&transfer_timer);
    timer_start(&transfer_timer);
    weight_t *dist_gpu = copy_graph_to_gpu(graph);
    timer_stop(&transfer_timer);

    int tile_dimension = GPU_TILE_DIM;
    int tiles_number_diagonally = graph->nr_vertices / tile_dimension;
    int k;
    //dim3 dimGrid(graph->nr_vertices/GPU_TILE_DIM,graph->nr_vertices/GPU_TILE_DIM);
    //dim3 dimBlock(GPU_TILE_DIM,GPU_TILE_DIM);

    /*
     * FILLME: Set up and launch the kernel(s)
     *
     * You may need different grid/block configurations for each stage
     * of the computation
     * 
     * Use GPU_TILE_DIM (see graph.h) as the tile dimension. You can
     * adjust its value during compilation. See `make help' for more
     * information.
     */
     for(k=0; k<tiles_number_diagonally; k++){
            /* --- PHASE 1 ---*/
	    dim3 dimGrid(1,1);
	    dim3 dimBlock(GPU_TILE_DIM,GPU_TILE_DIM);
            GPU_KERNEL_NAME(_tiled_stage_1) <<< dimGrid, dimBlock >>> (dist_gpu, graph->nr_vertices, k , tile_dimension);
            cudaThreadSynchronize();
            //printf("\n ***GPU****Stage 1<->k=%d*****GPU*****\n",k);
            //copy_graph_from_gpu(dist_gpu, graph);
            //graph_print(graph);


	    dim3 dimGrid2(graph->nr_vertices/GPU_TILE_DIM,2);
	    dim3 dimBlock2(GPU_TILE_DIM,GPU_TILE_DIM);
            /* --- PHASE 2 ---*/
            GPU_KERNEL_NAME(_tiled_stage_2) <<< dimGrid2, dimBlock2 >>> (dist_gpu, graph->nr_vertices, k , tile_dimension);
            cudaThreadSynchronize();
            //printf("\n ***GPU****Stage 2<->k=%d****GPU******\n",k);
            //copy_graph_from_gpu(dist_gpu, graph);
            //graph_print(graph);

            /* --- PHASE 3 ---*/
	    dim3 dimGrid3(graph->nr_vertices/GPU_TILE_DIM,graph->nr_vertices/GPU_TILE_DIM);
	    dim3 dimBlock3(GPU_TILE_DIM,GPU_TILE_DIM);
            GPU_KERNEL_NAME(_tiled_stage_3) <<< dimGrid3, dimBlock3 >>> (dist_gpu, graph->nr_vertices, k , tile_dimension);
            cudaThreadSynchronize();
            //printf("\n ****GPU*****Stage 3<->k=%d*****GPU*****\n",k);
            //copy_graph_from_gpu(dist_gpu, graph);
            //graph_print(graph);
     }


    /*
     * Wait for last kernel to finish, so as to measure correctly the
     * transfer times Otherwise, copy from GPU will block
     */
    cudaThreadSynchronize();

    /* Copy back results to host */
    timer_start(&transfer_timer);
    copy_graph_from_gpu(dist_gpu, graph);
    timer_stop(&transfer_timer);
    printf("Total transfer times: %lf s\n",
           timer_elapsed_time(&transfer_timer));
    return graph;
}

graph_t *MAKE_KERNEL_NAME(_gpu, _tiled_shmem)(graph_t *graph)
{
    xtimer_t transfer_timer;
    timer_clear(&transfer_timer);
    timer_start(&transfer_timer);
    weight_t *dist_gpu = copy_graph_to_gpu(graph);
    timer_stop(&transfer_timer);

    int tile_dimension = GPU_TILE_DIM;
    int tiles_number_diagonally = graph->nr_vertices / tile_dimension;
    int k;
    //dim3 dimGrid(graph->nr_vertices/GPU_TILE_DIM,graph->nr_vertices/GPU_TILE_DIM);
    //dim3 dimBlock(GPU_TILE_DIM,GPU_TILE_DIM);

    /*
     * FILLME: Set up and launch the kernel(s)
     *
     * You may need different grid/block configurations for each stage
     * of the computation
     * 
     * Use GPU_TILE_DIM (see graph.h) as the tile dimension. You can
     * adjust its value during compilation. See `make help' for more
     * information.
     */

     for(k=0; k<tiles_number_diagonally; k++){
            /* --- PHASE 1 ---*/
	    dim3 dimGrid(1,1);
	    dim3 dimBlock(GPU_TILE_DIM,GPU_TILE_DIM);
            GPU_KERNEL_NAME(_tiled_shmem_stage_1) <<< dimGrid, dimBlock  >>> (dist_gpu, graph->nr_vertices, k , tile_dimension);
            cudaThreadSynchronize();
            //printf("\n ***GPU****Stage 1<->k=%d*****GPU*****\n",k);
            //copy_graph_from_gpu(dist_gpu, graph);
            //graph_print(graph);

            /* --- PHASE 2 ---*/
	    dim3 dimGrid2(graph->nr_vertices/GPU_TILE_DIM,2);
	    dim3 dimBlock2(GPU_TILE_DIM,GPU_TILE_DIM);
            GPU_KERNEL_NAME(_tiled_shmem_stage_2) <<< dimGrid2, dimBlock2 >>> (dist_gpu, graph->nr_vertices, k , tile_dimension);
            cudaThreadSynchronize();
            //printf("\n ***GPU****Stage 2<->k=%d****GPU******\n",k);
            //copy_graph_from_gpu(dist_gpu, graph);
            //graph_print(graph);

            /* --- PHASE 3 ---*/
	    dim3 dimGrid3(graph->nr_vertices/GPU_TILE_DIM,graph->nr_vertices/GPU_TILE_DIM);
            dim3 dimBlock3(GPU_TILE_DIM,GPU_TILE_DIM);
            GPU_KERNEL_NAME(_tiled_shmem_stage_3) <<< dimGrid3, dimBlock3 >>> (dist_gpu, graph->nr_vertices, k , tile_dimension);
            cudaThreadSynchronize();
            //printf("\n ****GPU*****Stage 3<->k=%d*****GPU*****\n",k);
            //copy_graph_from_gpu(dist_gpu, graph);
            //graph_print(graph);
     }

    /*
     * Wait for last kernel to finish, so as to measure correctly the
     * transfer times Otherwise, copy from GPU will block
     */
    cudaThreadSynchronize();

    /* Copy back results to host */
    timer_start(&transfer_timer);
    copy_graph_from_gpu(dist_gpu, graph);
    timer_stop(&transfer_timer);
    printf("Total transfer times: %lf s\n",
           timer_elapsed_time(&transfer_timer));
    return graph;
}
