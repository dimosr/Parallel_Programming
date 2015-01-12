Parallel Programming <br/>(CUDA, openMP, MPI)
============================================

Introduction
--------------------------------------------
This repository contains some implementations of parallel programming.
3 technologies are used : 
	* openMP for shared memory topologies 
	* openMPI for distributed memory topologies
	* CUDA for GPUs (graphic processing units)

Project 1
--------------------------------------------

## Description

This project contains parallelized implementations for the Gauss algorithm  for linear systems using openMP and openMPI and comparing the parallelization
and time gain between various parallel implementations and the serial one.
In the file LU_serial.c, the original serial code has been given.

There are 4 parallel implementations of the algorithm with openMPI that differ in the type of data allocation and in the type of communication between the processes. The 1st type of data allocation is continous block allocation. So, if we have an array of 100x100 elements and n processes, the first 100/n lines of the array are allocated to the first process, the next 100/n lines are allocated to the next process. The 2nd type of data allocation is cyclic allocation of data. So, if we have an array of 100x100 elements and n processes, the first lines is allocated to the first process, the second line to the second process, ..., the n+1 line again to the first process etc. We also have 2 types of communication : broadcast communication and p2p communication. In broadcast communication, when a process has a result, it broadcasts the result to all processes. In the p2p communication, each process sends the calculated data only to the process that will use it. An algorithm has been implemented for each different combination of allocation type and communication type :
* LU_block_bcast : block data allocation & broadcast communication
* LU_block_p2p : block data allocation & p2p communication
* LU_cyclic_bcast :	cyclic data allocation & broadcast communication
* LU_cyclic_p2p : cyclic data allocation & p2p communication

There is also 1 parallel implementation of the algorithm with openMP:
* LU_omp

All the algorithms take as first argument an integer A and they create a square array AxA.

## Compilation & Execution

First of all, you have to make sure you have installed in your machine :
* openMP (http://openmp.org/wp/), which is pre-installed in some compilers
* openMPI(http://www.open-mpi.org/), which can be installed 

In order to execute all the different algorithms and compare their results, execute the following commands (adjust 4 to the cores of your machine):
```sh
make
./lu_serial 1500 			#execution of the serial algorithm

export OMP_NUM_THREADS=4	#define number of threads that will execute
./lu_omp 1500				#execution of the openMP algorithm

mpirun -np 4 ./lu_block_bcast 1500
mpirun -np 4 ./lu_block_p2p 1500
mpirun -np 4 ./lu_cyclic_bcast 1500
mpirun -np 4 ./lu_cyclic_p2p 1500
```

Project 2
-------------------------------------------------------------

## Description

This project contains parallelized implementations for the algorithm used for 2-Dimension matrix multiplication with openMP and [Cilk](https://software.intel.com/en-us/intel-cilk-plus).
2 kinds of algorithms are used as beginning to work on :
- serial : the classic algorithm
- tiled : a block-tiled improved algorithm
- recursive : a much more improved algorithm using recusrion
- strassen : the most improved version of the algorithm
All those initial forms are placed in the folder "Original Code" (those are initial implementations using openMP).
Then, we use Cilk to create more improved versions of those algorithms.

In the folder "Serial", there are 2 improved versions of serial algorithm:
- mm_serial_for 
- mm_serial_spawn

In the folder "Tiled", there is 1 improved version of tiled algorithm :
- par_mm_tiled2_c_j

In the folder "Recursive", there is 1 improved version of recursive algorithm using:
- mm_recursive

In the folder "Strassen", there is 1 improvred version of strassen algorithm :
- mm_parallel_strassen

## Compilation & Execution

First of all, you have to also install [Cilk](https://software.intel.com/en-us/intel-cilk-plus).
You can test each different version of the algorithm and compare their performance executing the following commands :
```sh
#In folder Original Code
make
./serial 800 100           #comparing performance for array of 800 elements with blocks in size of 10 elements
./tiled 800 10
./recursive 800 10
./strassen 800 10

#In folder Recursive
make
./recursive 800 10

#In folder Serial
make
./serial_spawn 800 10
./serial_for 800 10

#In folder strassen
make
./parallel_strassen 800 10
```

Project 3
-------------------------------------------------------------

## Description

This repository contains a parallelized implementation of the algorithm of Floyd-Warshall using [Cuda](https://developer.nvidia.com/about-cuda). For this reason, the code was transformed using CUDA language and an NVIDIA GPU was used in order to test the improved version and compare the performance. There are 3 different implementations based on the basic algorithm:
- the basic implementation in CUDA
- the tiled algorithm implemented in CUDA
- the tiled algorithm implemented in CUDA and using the shared memory of the chip

The various files are serving the below functions :
- apsp_main.c reads the input parameters, creates the graph, executes and monitors the time of each kernel (gpu & cpu), and validates the results
- graph.c & graph.h contain helping functionalities about the graph, as well as the various versions of Floyd-Warshall algorithm.
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

## Compilation & Execution

First of all, you have to connect a GPU to your machine and install and configure CUDA.
Then, you can compile and execute the project with the following commands (Makefile may need some changes to the GPU-specific configurations) :
```sh
make        #make DEBUG=0, if you want to test performance-time
./gpuinfo   #to take information about your GPU
export KERNEL=4
./apsp_main 4096        #execute the algorithm in GPU using 4 kernels
```