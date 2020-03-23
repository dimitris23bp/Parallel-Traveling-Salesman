#!/bin/bash

#Get the latest compiled version
make

for size in {1..17}
do

	echo "Starting size $size"

	./serialized -ws $size

	for num in 2 4 8
	do
		./openmp_simple -r -t $num

		#mpiexec -np $num ./mpi -r

	done

	echo "$size size is done"

done

echo "Done"
