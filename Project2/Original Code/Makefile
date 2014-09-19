CC=gcc
CFLAGS=-O3 -fopenmp -Wall -g -o 

all: serial recursive strassen tiled 

serial: mm_serial.c 
	$(CC) $(CFLAGS) serial mm_serial.c 

recursive: mm_recursive.c 
	$(CC) $(CFLAGS) recursive mm_recursive.c 

strassen: mm_strassen.c
	$(CC) $(CFLAGS) strassen mm_strassen.c

tiled: mm_tiled.c
	$(CC) $(CFLAGS) tiled mm_tiled.c 

clean:
	rm serial recursive strassen tiled 
	
