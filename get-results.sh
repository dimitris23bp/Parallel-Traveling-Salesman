#!/bin/bash

#Get the latest compiled version
make

rm results.txt 2> /dev/null
touch results.txt

for size in {11..15}
do

	echo "Starting size $size"

	./serialized -ws $size >> results.txt 

	for num in 2 3 4
	do
		./openmp_simple -r -t $num >> results.txt

		#mpiexec -np $num ./mpi -r

	done

done

echo "Done"
