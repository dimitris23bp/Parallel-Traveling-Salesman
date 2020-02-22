#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "matrix_generator.h"

#define N 20
#define NUM_OF_THREADS 8

unsigned int *final_path;
unsigned int final_res = UINT_MAX; 


void copyToFinal(int size, int* curr_path) 
{ 
	for (int i = 0; i < size; i++) {
		*(final_path + i) = curr_path[i];
	} 

	*(final_path + size) = curr_path[0];

	//I will eventually add Bcast or Send in the near future
	//MPI_Bcast(&final_path, size + 1, MPI_INT, status.MPI_SOURCE, MPI_COMM_WORLD); 
} 

int firstMin(int size, int adj[size][size], int i) 
{ 
	int min = INT_MAX; 
	
	for (int k = 0; k < size; k++){ 
		if (adj[i][k] < min && i != k){ 
			min = adj[i][k]; 
		}
	}
	
	return min;
} 
  
int secondMin(int size, int adj[size][size], int i) 
{ 
	int first = INT_MAX;
	int second = INT_MAX;

	for (int j=0; j<size; j++){ 
		if (i != j) { 

			if (adj[i][j] <= first){ 
				second = first; 
				first = adj[i][j]; 
			} 
			else if (adj[i][j] <= second) 
				second = adj[i][j]; 
		}
	} 
	return second; 
} 

void recursion(int size, int adj[size][size], int curr_bound, int curr_weight, int level, int curr_path[size+1], int visited[size]){ 

	if (level == size){ 

		if (adj[curr_path[level - 1]][curr_path[0]] != 0){ 
			int curr_res = curr_weight + adj[curr_path[level-1]][curr_path[0]]; 
	  
			if (curr_res < final_res){ 
				copyToFinal(size, curr_path); 
				final_res = curr_res; 

			} 
		} 			

		return; 	
	} 


	for (int i = 1; i < size; i++){ 
		if (adj[curr_path[level-1]][i] != 0 && visited[i] == 0){

			int temp = curr_bound; 
			curr_weight += adj[curr_path[level - 1]][i]; 
			curr_bound -= ((secondMin(size, adj, curr_path[level - 1]) + firstMin(size, adj, i))/2); 

			if (curr_bound + curr_weight < final_res){ 
				curr_path[level] = i; 
				visited[i] = 1; 
  
				recursion(size, adj, curr_bound, curr_weight, level + 1, curr_path, visited); 

			} 

			curr_weight -= adj[curr_path[level-1]][i]; 
			curr_bound = temp; 
		  	
 			memset(visited, 0, sizeof(int)*size);
			for (int j = 0; j <= level - 1; j++) {
				visited[curr_path[j]] = 1; 
			}	
			

		}
	}
}


void second_node(int size, int adj[size][size], int curr_bound, int curr_path[size+1], int visited[size], int recv_size, int recv_second[recv_size]){

  	for(int i = 1; i < recv_size; i++){
  		if(recv_second[i] != 0){
		
			int temp = curr_bound; 
			curr_bound -= ((secondMin(size, adj, curr_path[0]) + firstMin(size, adj, recv_second[i]))/2); 
	  			
			curr_path[1] = recv_second[i]; 
			visited[recv_second[i]] = 1; 

			recursion(size, adj, curr_bound, adj[curr_path[0]][recv_second[i]], 2, curr_path, visited); 

			curr_bound = temp; 
	 		memset(visited, 0, sizeof(int)*size);
	 		visited[0] = 1;
	 	}
	}


}


void first_node(int size, int adj[size][size], int numtasks, int rank){

	int curr_path[size+1]; 
 	memset(curr_path, -1, sizeof(curr_path));

 	int visited[size];
 	memset(visited, 0, sizeof(visited));


	int init_bound = 0; 
  
	for (int i = 0; i < size; i++) {
		init_bound += (firstMin(size, adj, i) + secondMin(size, adj, i)); 
	}

	if(init_bound == 1){
		init_bound = init_bound / 2 + 1;
	} else {
		init_bound = init_bound / 2; 
	}

	visited[0] = 1; 
	curr_path[0] = 0; 
	

	int recv_size;
	if(size % numtasks == 0){
		recv_size = size / numtasks;
	} else {
		recv_size = (size / numtasks) + 1;
	}

	//Second nodes that every proccessor will get
	// ***SPOILER ALERT*** some of them might be -1
	int recv_second[recv_size];

	//All second nodes (with the -1s too)
	int seconds[size];

	if(rank == 0){
		//How many real seconds nodes
		int how_many_each[numtasks];

		//How many second nodes are the minimum
		int baseline_of_seconds = size / numtasks;

		// Minus 1, because I don't care about node number 0 (it is the root node)
		int diafora = size % numtasks - 1;



		// Calculate how_many_each
		for(int i = 0; i < numtasks; i++){
			how_many_each[i] = baseline_of_seconds;
			if(diafora != 0){
				how_many_each[i]++;
				diafora--;
			}
		}
	
		printf("How many: %d\n", how_many_each[0]);
		printf("How many: %d\n", how_many_each[1]);
		printf("How many: %d\n", how_many_each[2]);
		printf("How many: %d\n", how_many_each[3]);


		int second_node = 1;

		//Iterate as many times as the number of machines
		for(int i = 0; i < numtasks; i++){

			// Iterate as many times as the biggest amount of second nodes on one machine
			for(int j = 0; j < recv_size; j++){
				if(how_many_each[i] != 0){
					how_many_each[i]--;
					seconds[(i*recv_size) + j] = second_node++;
				} else {
					seconds[(i*recv_size) + j] = -1;
				}
				printf("Seconds[%d]: %d\n", i*recv_size +j, seconds[i*recv_size +j]);
			}

		}

	}

	//Share all possible second nodes to each processor 
	MPI_Scatter(&seconds, recv_size, MPI_INT, recv_second, recv_size, MPI_INT, 0, MPI_COMM_WORLD);

	printf("Got %d in rank %d\n", recv_second[0], rank);
	printf("Got %d in rank %d\n", recv_second[1], rank);
	printf("Got %d in rank %d\n", recv_second[2], rank);
	
	int curr_bound = init_bound;

	second_node(size, adj, curr_bound, curr_path, visited, recv_size, recv_second); 


}
 


int main(int argc, char *argv[]){

	int size = N;
	char option = 'r';

	int rank, numtasks;

	MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(argc == 2){
		option = argv[1][0];
		if(option == 'r'){
			size = get_size_of_matrix();
		}
	} else if( argc == 3){
		option = argv[1][0];
		size = atoi(argv[2]);
	}

	int adj[size][size];

	// I add one more because of a corrupted size vs. prev_size error. Probably needs it to store \0 byte at the end
	final_path = (int*) malloc(size * sizeof(int) +1);

	// Check if the memory has been successfully 
    // allocated by malloc or not 
    if (final_path == NULL) { 
        printf("Memory not allocated.\n"); 
        exit(0); 
    } 

	if(option == 'w'){
		generator(size, adj, 50, 99);
		write_to_file(size, adj);
	} else if(option == 'r'){
		read_from_file(size, adj);
	} else{
		return 0;
	}
	
	if(rank == 0)
		display(size, adj);
	
	//Starting time of solution

	first_node(size, adj, numtasks, rank);

   	
	printf("Minimum cost : %d\n", final_res); 
	printf("Path Taken : "); 
	for (int i = 0; i <= size; i++){ 
		printf("%d ", final_path[i]); 
	} 

	printf("\n");

	//Finishing time of solution
	//double finish = omp_get_wtime();

	//printf("Time spent: %f\n", finish - start);

	MPI_Finalize();


	return 0;
}