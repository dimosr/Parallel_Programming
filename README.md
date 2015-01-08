

		-------------------------------------------------
		|						|
		|    Parallel Programming (CUDA, openMP, MPI)   |
		|						|
		-------------------------------------------------


	This repository contains some implementations of parallel programming.
	3 technologies are used : 
	- openMP for shared memory topologies 
	- openMPI for distributed memory topologies
	- CUDA for GPUs (graphic processing units)

----------------------------- Project 1 --------------------------------

This project contains parallel implementations for the Gauss algorithm 
for linear systems using openMP and openMPI and comparing the parallelization
and time gain.

In the file LU_serial.c, the original serial code has been given.
There are 4 parallel implementations of the algorithm with openMPI:
- LU_block_bcast : 	continuous block allocation of data to processes
				   	broadcast communication between processes
- LU_block_p2p : 	continuous block allocation of data to processes
					p2p communication between processes
- LU_cyclic_bcast :	cyclic (line-by-line) allocation of data to processes
					broadcast communication between processes
- LU_cyclic_p2p :	cyclic (line-by-line) allocation of data to processes
					p2p communication between processes

There is also 1 parallel implementation of the algorithm with openMP:
- LU_omp


----------------------------- Project 2 --------------------------------

This project contains parallel implementations for the algorithm used 
for 2-Dimension matrix multiplication with openMP.
2 kinds of algorithms are used as beginning to work on :
- serial : the classic algorithm
- tiled : a block-tiled improced algorithm
- recursive : a much more improved algorithm using recusrion
- strassen : the most improved version of the algorithm
All those initial forms are placed in folder "Original Code"

In the folder "Serial", there are 2 improved versions of serial algorithm :
- mm_serial_for 
- mm_serial_spawn

In the folder "Tiled", there is 1 improved version of tiled algorithm :
- par_mm_tiled2_c_j

In the folder "Recursive", there is 1 improved version of recursive algorithm :
- mm_recursive

In the folder "Strassen", there is 1 improvred version of strassen algorithm :
- mm_parallel_strassen

----------------------------- Project 3 --------------------------------

This repository contains a parallel implementation of the algorithm of 
Floyd-Warshall. For this reason, an NVIDIA GPU was used and the parallelization
was implemented with CUDA. There are 3 different implementations based on the 
basic algorithm, the tiled algorithm and a last one using the on-chip shared
memory of the GPU.

The various files are serving the below functions :
- apsp_main.c reads the input parameters, creates the graph, executes and 
monitors the time of each kernel (gpu & cpu), and validates the results
- graph.c & graph.h contain helping functionalities about the graph, as well
as the various versions of Floyd-Warshall algorithm.
- cpu_kernels.c &cpu_kernels.h, calls the routines of graph.c for the cpu kernels
- gpu_kernels.cu making everything that is necessary to call the gpu kernels
(memory allocation in GPU, copy from/to GPU, parameters of kernels etc.)

Helping functions : alloc.c,error.c, timer.c, gpu_util.cu

In the file gpu_kernels, the 3 implementations are covered mainly in the
following functions:
- GPU_KERNEL_NAME(_naive)
- GPU_KERNEL_NAME(_tiled_stage_1), GPU_KERNEL_NAME(_tiled_stage_2), GPU_KERNEL_NAME(_tiled_stage_3)
- GPU_KERNEL_NAME(_tiled_shmem_stage_1), GPU_KERNEL_NAME(_tiled_shmem_stage_2),GPU_KERNEL_NAME(_tiled_shmem_stage_3)

NOTICE : The tiled implementation has 3 phases :
1. calculation for tile T{kk}
2. calculation for tiles T{ik} and T{ki} using T{kk}
3. calculation for all the rest tiles T{ij} using T{ik} and T{ki}

The same phases exist in the shared memory implementation, just with the difference
that the data are first copied in the shared memory of the GPU and then
used from there for the rest execution.

