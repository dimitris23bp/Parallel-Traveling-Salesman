#!/bin/bash

#Get the latest compiled version
make

# Variables for the iterations
CORES=(4 8 12 16 20)
SIZES=(12 13 14)
ITERATIONS=(1 2 3)

./create-arrays ${SIZES[0]} ${SIZES[-1]}

for var in "$@"
do

	# Find what type of executable is the first argument
	if [[ $var =~ ^mpi ]]; then
		str="mpi"
	else
		str="openmp"
	fi

	#Name of file I want to execute
	EXECUTABLE=$var

	#Directory with the results
	DIRECTORY=results/
	mkdir $DIRECTORY 2> /dev/null

	#Name of the file with the results
	RESULTS=$DIRECTORY$EXECUTABLE.txt

	# Remove the previous result files if they exist and create them brand new
	rm $RESULTS 2> /dev/null
	touch $RESULTS

	# The sizes to run the implementations
	for size in ${SIZES[*]}
	do
		sum=0

		echo "Starting size $size"

		# Run the programs many times for more accurate results
		for index in ${ITERATIONS[*]}
		do
			new=$(./serialized -r -f example-arrays/file${size}-${index}.txt)
			echo "example-arrays/file${size}-${index}.txt"

			sum=$(echo "$sum + $new" | bc -l)
		done

		# Average of serialized, because I need it for speed up calculation
		average_ser=$(echo "$sum / ${#ITERATIONS}" | bc -l)
		#Speed up of serialized is always 1.0
		echo "1 $size 1.0" >> $RESULTS

		# Number of cores
		for core in ${CORES[*]}
		do

			sum=0

			# Run the programs many times for more accurate results
			for index in ${ITERATIONS[*]}
			do
				# Run the first executable as intended
				if [[ $str == "mpi" ]]; then
					new=$(mpiexec -np $core ./$EXECUTABLE -r -f example-arrays/file${size}-${index}.txt)
				else
					new=$(./$EXECUTABLE -r -t $core -f example-arrays/file${size}-${index}.txt)
				fi
				sum=$(echo "$sum + $new" | bc -l)

			done

			# Find the anerage values and calculate the speedups
			average=$(echo "$sum / ${#ITERATIONS}" | bc -l)
			speedup=$(echo "$average_ser / $average" | bc -l)
			echo "$core $size $speedup" >> $RESULTS

		done

	done

done

# Remove executables
make clean

echo "Done"
