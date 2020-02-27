#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#include "matrix_generator.h"
#include "common_functions.h"

#define N 20

unsigned int *final_path;
unsigned int final_res = UINT_MAX; 

void recursion(
	int size, 
	int adj[size][size], 
	int curr_bound, 
	int curr_weight, 
	int level, 
	int curr_path[size+1], 
	int visited[size], 
	int** first_mins, int ** second_mins){ 

	if (level == size){ 

		if (adj[curr_path[level - 1]][curr_path[0]] != 0){ 
			int curr_res = curr_weight + adj[curr_path[level-1]][curr_path[0]]; 
	  
			if (curr_res < final_res){ 
				copy_to_final(size, curr_path, final_path); 
				final_res = curr_res; 

			} 
		} 			

		return; 	
	} 


	for (int i = 1; i < size; i++){ 
		if (adj[curr_path[level-1]][i] != 0 && visited[i] == 0){

			int temp = curr_bound; 
			curr_weight += adj[curr_path[level - 1]][i]; 
			curr_bound -= ((*(*second_mins + curr_path[level - 1]) + *(*first_mins + i))/2); 

			if (curr_bound + curr_weight < final_res){ 
				curr_path[level] = i; 
				visited[i] = 1; 
  
				recursion(size, adj, curr_bound, curr_weight, level + 1, curr_path, visited, first_mins, second_mins); 

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


void second_node(
	int size, 
	int adj[size][size], 
	int curr_bound, 
	int curr_path[size+1], 
	int visited[size], 
	int recv_size, 
	int recv_second[recv_size], 
	int** first_mins, int** second_mins){

  	for(int i = 0; i < recv_size; i++){
  		if(recv_second[i] != -1){

			int temp = curr_bound; 
			curr_bound -= ((*(*second_mins + curr_path[0]) + *(*first_mins + i))/2); 
	  		
			curr_path[1] = recv_second[i]; 
			visited[recv_second[i]] = 1; 

			recursion(size, adj, curr_bound, adj[curr_path[0]][recv_second[i]], 2, curr_path, visited, first_mins, second_mins); 

			curr_bound = temp; 
	 		memset(visited, 0, sizeof(int)*size);
	 		visited[0] = 1;
	 	}
	}

}

void first_node(
	int size, 
	int adj[size][size], 
	int numtasks, 
	int rank, 
	int** first_mins, int** second_mins){

	int curr_path[size+1]; 
 	memset(curr_path, -1, sizeof(curr_path));
	curr_path[0] = 0; 

 	int visited[size];
 	memset(visited, 0, sizeof(visited));
	visited[0] = 1; 

	int init_bound = 0; 
  	init_bound = *(*first_mins);
  	
	for (int i = 1; i < size; i++) {
		init_bound += *(*first_mins + i) + *(*second_mins + i); 
	}

	if(init_bound == 1){
		init_bound = init_bound / 2 + 1;
	} else {
		init_bound = init_bound / 2; 
	}
	

	int recv_size;
	if((size-1) % numtasks == 0){
		recv_size = size / numtasks;
	} else {
		recv_size = (size / numtasks) + 1;
	}

	//Second nodes that every proccessor will get
	// ***SPOILER ALERT*** some of them might be -1
	int recv_second[recv_size];

	//All second nodes (with the -1s too)
	int seconds[numtasks * recv_size];

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
			}

		}

	}

	//Share all possible second nodes to each processor 
	MPI_Scatter(&seconds, recv_size, MPI_INT, recv_second, recv_size, MPI_INT, 0, MPI_COMM_WORLD);

	int curr_bound = init_bound;

	second_node(size, adj, curr_bound, curr_path, visited, recv_size, recv_second, first_mins, second_mins); 

}

void find_mins(int size, int **first_mins, int **second_mins, int adj[size][size]){

	for(int i = 0; i < size; i++){
		*(*first_mins + i) = INT_MAX;
		*(*second_mins + i) = INT_MAX;

		for (int j = 0; j < size; j++){ 
				if (i != j) { 

					if (adj[i][j] <= *(*first_mins + i)){ 
						*(*second_mins + i) = *(*first_mins + i); 
						*(*first_mins + i) = adj[i][j]; 
					} 
					else if (adj[i][j] <= *(*second_mins + i)) 
						*(*second_mins + i) = adj[i][j]; 
				}
		}

	}

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
	
	if(rank == 0){
		display(size, adj);
	}

	//Starting time of solution
	clock_t begin = clock();

	
	//Get first_min and second_min as two arays instead of calling them each time 
	int *first_mins = malloc(size * sizeof(int));
	int *second_mins = malloc(size * sizeof(int));
	find_mins(size, &first_mins, &second_mins, adj);

	// This is the only function of the solution in main. Everything else is into this
	first_node(size, adj, numtasks, rank, &first_mins, &second_mins);

	int finals[numtasks];

	MPI_Gather(&final_res, 1, MPI_INT, finals, 1, MPI_INT, 0, MPI_COMM_WORLD);

	// Index of the rank with the best result
	int index = 0;

	// Find the minimum of each rank's minimum
	if (rank == 0)
	{	
		for(int i = 1; i < numtasks; i++){
			if (finals[i] < final_res){
				final_res = finals[i];
				index = i;
			}
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);

	// Send index to everyone so they know who must send its path
	MPI_Bcast(&index, 1, MPI_INT, 0, MPI_COMM_WORLD);


	// If the best is master, don't send/receive anything
	if(index != 0){

		int tag = 1;

		// If this rank has the minimum, then the path
		if(rank == index){
			MPI_Send(final_path, size+1, MPI_INT, 0, tag, MPI_COMM_WORLD);
		} else if(rank == 0){
			MPI_Status Status;
			MPI_Recv(final_path, size+1, MPI_INT, index, tag, MPI_COMM_WORLD, &Status);
		}
	
	}
	
   	// Display minimum cost and the path of it
   	if(rank == 0){
		printf("Minimum cost : %d\n", final_res); 
		printf("Path Taken : "); 
		for (int i = 0; i <= size; i++){ 
			printf("%d ", final_path[i]); 
		} 

		printf("\n");

		clock_t end = clock();
		double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

		printf("Time spent: %f\n", time_spent);

	}

	MPI_Finalize();


	return 0;
}
