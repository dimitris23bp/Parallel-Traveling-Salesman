CC=gcc
CFLAGS = -fopenmp
MPICC=mpicc
vpath %.h headers
INCLUDED_FILES= common_functions.c matrix_generator.c
INCLUDES_HEADERS= matrix_generator.h common_functions.h arguments.h

all: create_arrays.o serialized.o openmp mpi

openmp: openmp_simple.o openmp_tasks.o openmp_schedule.o openmp_for.o
mpi: mpi_one_sided_to_master.o mpi_one_sided_to_rank.o mpi_simple.o

create_arrays.o: create_arrays.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(CC) -o create_arrays.o create_arrays.c $(INCLUDED_FILES)

serialized.o: serialized.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(CC) -o serialized.o serialized.c $(INCLUDED_FILES)

openmp_simple.o: openmp_simple.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(CC) -o openmp_simple.o openmp_simple.c $(INCLUDED_FILES) $(CFLAGS)

openmp_tasks.o: openmp_tasks.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(CC) -o openmp_tasks.o openmp_tasks.c $(INCLUDED_FILES) $(CFLAGS)

openmp_schedule.o: openmp_schedule.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(CC) -o openmp_schedule.o openmp_schedule.c $(INCLUDED_FILES) $(CFLAGS)

openmp_for.o: openmp_for.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(CC) -o openmp_for.o openmp_for.c $(INCLUDED_FILES) $(CFLAGS)

mpi_simple.o: mpi_simple.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(MPICC) -o mpi_simple.o mpi_simple.c $(INCLUDED_FILES)

mpi_one_sided_to_master.o: mpi_one_sided_to_master.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(MPICC) -o mpi_one_sided_to_master.o mpi_one_sided_to_master.c $(INCLUDED_FILES)

mpi_one_sided_to_rank.o: mpi_one_sided_to_rank.c $(INCLUDED_FILES) $(INCLUDES_HEADERS)
	$(MPICC) -o mpi_one_sided_to_rank.o mpi_one_sided_to_rank.c $(INCLUDED_FILES)

clean:
	rm *.o
