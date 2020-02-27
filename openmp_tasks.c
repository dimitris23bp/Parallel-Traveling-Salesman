#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <omp.h>
#include <time.h>

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
} 
  
void recursion(int size, int adj[size][size], int curr_bound, int curr_weight, int level, int curr_path[size+1], int visited[size], int** first_mins, int** second_mins){ 

	if (level == size){ 
		if (adj[curr_path[level - 1]][curr_path[0]] != 0){ 
			int curr_res = curr_weight + adj[curr_path[level-1]][curr_path[0]]; 
	  		#pragma omp critical
	  		{
				if (curr_res < final_res){
					copyToFinal(size, curr_path); 
					final_res = curr_res; 
				}
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

			curr_path[level] = -1;


		}
	}
}


void second_node(int size, int adj[size][size], int curr_bound, int curr_path[size+1], int visited[size], int j, int** first_mins, int** second_mins){

	int temp = curr_bound; 


	curr_bound -= ((*(*second_mins + curr_path[0]) + *(*first_mins + j))/2); 

	curr_path[1] = j; 
	visited[j] = 1; 

	recursion(size, adj, curr_bound, adj[curr_path[0]][j], 2, curr_path, visited, first_mins, second_mins); 
	 		

}
  
void first_node(int size, int adj[size][size], int** first_mins, int** second_mins){


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
  

	omp_set_num_threads(NUM_OF_THREADS);
	#pragma omp parallel
	{

	 	#pragma omp single
	 	{
	 		for(int j = 1; j < size; j++){

	 			#pragma omp task firstprivate(j)
	 			{
	 				//Declare curr_path and set it to start from 0 node
					int curr_path[size + 1]; 
					memset(curr_path, -1, sizeof(curr_path));
					curr_path[0] = 0; 

					//Declare visited nodes and set it to have visited 
					int visited[size]; 
					memset(visited, 0, sizeof(visited));
					visited[0] = 1; 

					int curr_bound = init_bound;
					second_node(size, adj, curr_bound, curr_path, visited, j, first_mins, second_mins); 


	 			}


	 		}
	 	}

	}

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

int main(int argc, char const *argv[]){

	int size = N;
	char option = 'w';

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

	final_path = (int *)malloc(size * sizeof(int));
	

	if(option == 'w'){
		generator(size, adj, 50, 99);
		write_to_file(size, adj);
	} else if(option == 'r'){
		read_from_file(size, adj);
	} else{
		return 0;
	}

	display(size, adj);
	
	//Starting time of solution
	double start = omp_get_wtime();

	//Get first_min and second_min as two arays instead of calling them each time 
	int *first_mins = malloc(size * sizeof(int));
	int *second_mins = malloc(size * sizeof(int));
	find_mins(size, &first_mins, &second_mins, adj);

	first_node(size, adj, &first_mins, &second_mins);
   	
	printf("Minimum cost : %d\n", final_res); 
	printf("Path Taken : "); 
	for (int i = 0; i <= size; i++){ 
		printf("%d ", final_path[i]); 
	} 

	printf("\n");

	//Finishing time of solution
	double finish = omp_get_wtime();

	printf("Time spent: %f\n", finish - start);

	return 0;
}