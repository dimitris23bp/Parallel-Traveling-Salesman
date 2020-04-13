#!/bin/bash

#Get the latest compiled version
make

#Name of file I want to execute
EXECUTABLE1=$1
EXECUTABLE2=$2

#Name of the file with the results
RESULTS1=$EXECUTABLE1.txt
RESULTS2=$EXECUTABLE2.txt
# Remove if it exists
rm $RESULTS1 $RESULTS2 2> /dev/null

#Create this file again, with the new data
touch $RESULTS1 $RESULTS2

for size in {11..15}
do

	echo "Starting size $size"

	./$EXECUTABLE1 -ws $size >> $RESULTS1
	./$EXECUTABLE2 -r -t 1 >> $RESULTS2

	for num in 2 3 4
	do
		./$EXECUTABLE1 -r -t $num >> $RESULTS1
		./$EXECUTABLE2 -r -t $num >> $RESULTS2

	done

done

echo "Done"
