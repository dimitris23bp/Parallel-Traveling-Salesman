CC=gcc
CFLAGS = -fopenmp

openmp: openmp_implementation.c matrix_generator.h matrix_generator.c
	$(CC) -o openmp openmp_implementation.c matrix_generator.c $(CFLAGS)

serialized: serialized.c matrix_generator.c matrix_generator.h
	$(CC) -o serialized serialized.c matrix_generator.c

clean:
	rm openmp serialized a.out
