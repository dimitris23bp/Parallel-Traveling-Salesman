#!/bin/bash

#Get the latest compiled version
make

#Name of file I want to execute
EXECUTABLE=$1

#Name of the file with the results
RESULTS_FILE=ser-$EXECUTABLE.txt

# Remove if it exists
rm $RESULTS_FILE 2> /dev/null

#Create this file again, with the new data
touch $RESULTS_FILE

for size in {11..15}
do

	echo "Starting size $size"

	./serialized -ws $size >> $RESULTS_FILE

	for num in 2 3 4
	do
		./$EXECUTABLE -r -t $num >> $RESULTS_FILE

		#mpiexec -np $num ./mpi -r

	done

done

echo "Done"
