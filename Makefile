CC=gcc
CFLAGS = -fopenmp
MPICC=mpicc
INCLUDED_FILES= common_functions.c matrix_generator.c

all: openmp_simple serialized openmp_tasks mpi

openmp_simple: openmp_simple.c matrix_generator.h common_functions.h
	$(CC) -o openmp_simple openmp_simple.c $(INCLUDED_FILES) $(CFLAGS)

serialized: serialized.c matrix_generator.h common_functions.h arguments.h
	$(CC) -o serialized serialized.c $(INCLUDED_FILES)

openmp_tasks: openmp_tasks.c matrix_generator.h common_functions.h
	$(CC) -o openmp_tasks openmp_tasks.c $(INCLUDED_FILES) $(CFLAGS)

mpi: mpi_implementation.c matrix_generator.h common_functions.h
	$(MPICC) -o mpi mpi_implementation.c $(INCLUDED_FILES)

clean:
	rm openmp_simple serialized a.out openmp_tasks mpi
