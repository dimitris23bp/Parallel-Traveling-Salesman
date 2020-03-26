CC=gcc
CFLAGS = -fopenmp
MPICC=mpicc
vpath %.h headers
INCLUDED_FILES= common_functions.c matrix_generator.c
INCLUDES_HEADERS= matrix_generator.h common_functions.h arguments.h

all:  serialized openmp mpi

openmp: openmp_simple openmp_tasks openmp_schedule 

serialized: serialized.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(CC) -o serialized serialized.c $(INCLUDED_FILES)

openmp_simple: openmp_simple.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(CC) -o openmp_simple openmp_simple.c $(INCLUDED_FILES) $(CFLAGS)

openmp_tasks: openmp_tasks.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(CC) -o openmp_tasks openmp_tasks.c $(INCLUDED_FILES) $(CFLAGS)

openmp_schedule: openmp_simple_schedule.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(CC) -o openmp_schedule openmp_simple_schedule.c $(INCLUDED_FILES) $(CFLAGS)

mpi: mpi_implementation.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(MPICC) -o mpi mpi_implementation.c $(INCLUDED_FILES)

clean:
	rm openmp_simple serialized openmp_tasks mpi openmp_schedule

