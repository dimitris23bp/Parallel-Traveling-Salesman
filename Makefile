CC=gcc
CFLAGS = -fopenmp
MPICC=mpicc
vpath %.h headers
INCLUDED_FILES= common_functions.c matrix_generator.c
INCLUDES_HEADERS= matrix_generator.h common_functions.h arguments.h

all:  serialized openmp mpi

openmp: openmp_simple openmp_tasks openmp_schedule openmp_for
mpi: mpi_one_sided_to_master mpi_one_sided_to_rank mpi_simple

serialized: serialized.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(CC) -o serialized serialized.c $(INCLUDED_FILES)

openmp_simple: openmp_simple.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(CC) -o openmp_simple openmp_simple.c $(INCLUDED_FILES) $(CFLAGS)

openmp_tasks: openmp_tasks.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(CC) -o openmp_tasks openmp_tasks.c $(INCLUDED_FILES) $(CFLAGS)

openmp_schedule: openmp_schedule.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(CC) -o openmp_schedule openmp_schedule.c $(INCLUDED_FILES) $(CFLAGS)

openmp_for: openmp_for.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(CC) -o openmp_for openmp_for.c $(INCLUDED_FILES) $(CFLAGS)

mpi_simple: mpi_simple.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(MPICC) -o mpi_simple mpi_simple.c $(INCLUDED_FILES)

mpi_one_sided_to_master: mpi_one_sided_to_master.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(MPICC) -o mpi_one_sided_to_master mpi_one_sided_to_master.c $(INCLUDED_FILES)

mpi_one_sided_to_rank: mpi_one_sided_to_rank.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(MPICC) -o mpi_one_sided_to_rank mpi_one_sided_to_rank.c $(INCLUDED_FILES)

clean:
	rm serialized openmp_simple openmp_tasks openmp_schedule openmp_for mpi_simple mpi_one_sided_to_rank mpi_one_sided_to_master
