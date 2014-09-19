CC=/various/common_tools/cilkplus-gcc-4.8/bin/cilk-gcc
CFLAGS=-O3 -LLIBDIR -fcilkplus -lcilkrts -Wall -g -o 

all: parallel_strassen

parallel_strassen: mm_parallel_strassen.c
	$(CC) $(CFLAGS) parallel_strassen mm_parallel_strassen.c
clean:
	rm parallel_strassen
	
