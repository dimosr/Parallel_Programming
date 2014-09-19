CC=/various/common_tools/cilkplus-gcc-4.8/bin/cilk-gcc
CFLAGS=-O3  -LLIBDIR -fcilkplus -lcilkrts -Wall -g -o 


all: mm_recursive

mm_recursive: mm_recursive.c
	$(CC) $(CFLAGS) mm_recursive mm_recursive.c

clean:
	rm mm_recursive 
