CC=/various/common_tools/cilkplus-gcc-4.8/bin/cilk-gcc
CFLAGS=-O3 -LLIBDIR -fcilkplus -lcilkrts -Wall -g -o 

all: serial_spawn serial_for 

serial_spawn: mm_serial_spawn.c 
	$(CC) $(CFLAGS) serial_spawn mm_serial_spawn.c 

serial_for: mm_serial_for.c
	$(CC) $(CFLAGS) serial_for mm_serial_for.c 

queue:
	qsub -q parlab make.sh 

run_measurements: run_dun_all run_ter_all

run_dun_all: run_dun_for run_dun_spawn

run_ter_all: run_ter_for run_ter_spawn

run_dun_for:
	qsub -q parlab2 dun_for.sh

run_dun_spawn:
	qsub -q parlab2 dun_spawn.sh

run_ter_for:
	qsub -q parlab2 ter_for.sh

run_ter_spawn:
	qsub -q parlab2 ter_spawn.sh

clean:
	rm serial_spawn serial_for *.out *.err
