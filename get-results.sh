#!/bin/bash

#Get the latest compiled version
make


for size in {10..20}
do
	./openmp_simple -ws $size -t 8
	echo "$size is done"
done

echo "Done"
