#!/bin/bash

#Get the latest compiled version
make


for size in {10..17}
do

	echo "Starting size $size"

	./serialized -ws $size
	echo "1 thread is done for size $size"

	for num_of_threads in 2 4 8
	do
		./openmp_simple -r -t $num_of_threads
		echo "$num_of_threads threads are done for size $size"
	done

	echo "$size size is done"

done

echo "Done"
