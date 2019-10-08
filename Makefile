CC=gcc
CFLAGS = -fopenmp

all: openmp serialized 

openmp: openmp_implementation.c matrix_generator.h matrix_generator.c
	$(CC) -o openmp openmp_implementation.c matrix_generator.c $(CFLAGS)

serialized: serialized.c matrix_generator.c matrix_generator.h
	$(CC) -o serialized serialized.c matrix_generator.c

clean:
	rm openmp serialized a.out
