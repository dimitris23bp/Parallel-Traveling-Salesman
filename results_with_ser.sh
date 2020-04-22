#!/bin/bash

# Go to root directory of the
cd ..

#Get the latest compiled version
make

# Find what type of executable is the first argument
if [[ $1 =~ ^mpi ]]; then
	str1="mpi"
else
	str1="openmp"
fi

# Find what type of executable is the second argument
if [[ $2 =~ ^mpi ]]; then
	str2="mpi"
else
	str2="openmp"
fi

#Name of file I want to execute
EXECUTABLE1=$1
EXECUTABLE2=$2

#Directory with the results
DIRECTORY=results/
mkdir $DIRECTORY 2> /dev/null

#Name of the file with the results
RESULTS1=$DIRECTORY$EXECUTABLE1.txt
RESULTS2=$DIRECTORY$EXECUTABLE2.txt

# Remove the previous result files if they exist and create them brand new
rm $RESULTS1 $RESULTS2 2> /dev/null
touch $RESULTS1 $RESULTS2

# Variables for the iterations
CORES=(2 3 4 5 6 7 8)
SIZES=(11 12 13 14 15)
ITERATIONS=(0 1 2 3)

# The sizes to run the implementations
for size in ${SIZES[*]}
do
	sum1=0
	sum2=0

	echo "Starting size $size"

	# Run the programs many times for more accurate results
	for index in ${ITERATIONS[*]}
	do
		new=$(./serialized -w -s $size -f example-arrays/file0$index.txt)
		sum1=$(echo "$sum1 + $new" | bc -l)
	done

	# Average of serialized, because I need it for speed up calculation
	average_ser=$(echo "$sum1 / ${#ITERATIONS}" | bc -l)
	#Speed up of serialized is always 1.0
	echo "1 $size 1.0" >> $RESULTS1
	echo "1 $size 1.0" >> $RESULTS2

	# Number of cores
	for core in ${CORES[*]}
	do

		sum1=0
		sum2=0

		# Run the programs many times for more accurate results
		for index in ${ITERATIONS[*]}
		do

			# Run the first executable as intended
			if [[ $str1 == "mpi" ]]; then
				new=$(mpiexec -np $core ./$EXECUTABLE1 -r -f example-arrays/file0$index.txt)
			else
				new=$(./$EXECUTABLE1 -r -t $core -f example-arrays/file0$index.txt)
			fi
			sum1=$(echo "$sum1 + $new" | bc -l)

			# Run the second executable as intended
			if [[ $str2 == "mpi" ]]; then
				new=$(mpiexec -np $core ./$EXECUTABLE2 -r -f example-arrays/file0$index.txt)
			else
				new=$(./$EXECUTABLE2 -r -t $core -f example-arrays/file0$index.txt)
			fi
			sum2=$(echo "$sum2 + $new" | bc -l)

		done

		# Find the anerage values and calculate the speedups
		average=$(echo "$sum1 / ${#ITERATIONS}" | bc -l)
		speedup=$(echo "$average_ser / $average" | bc -l)
		echo "$core $size $speedup" >> $RESULTS1

		average=$(echo "$sum2 / ${#ITERATIONS}" | bc -l)
		speedup=$(echo "$average_ser / $average" | bc -l)
		echo "$core $size $speedup" >> $RESULTS2

	done

done

# Remove executables
make clean

echo "Done"
