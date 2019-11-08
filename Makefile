CC=gcc
CFLAGS = -fopenmp
MPICC=mpicc

all: openmp_simple serialized openmp_tasks

openmp_simple: openmp_simple.c matrix_generator.h matrix_generator.c
	$(CC) -o openmp_simple openmp_simple.c matrix_generator.c $(CFLAGS)

serialized: serialized.c matrix_generator.c matrix_generator.h
	$(CC) -o serialized serialized.c matrix_generator.c

openmp_tasks: openmp_tasks.c matrix_generator.c matrix_generator.h
	$(CC) -o openmp_tasks openmp_tasks.c matrix_generator.c $(CFLAGS)

clean:
	rm openmp_simple serialized a.out openmp_tasks
