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

	// printf("Best by now: ");
	// for(int i = 0; i < size; i++){
	// 	printf("%d ",final_path[i]);
	// }
	// printf("\nResult: %d\n",final_res );
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
			else if (adj[i][j] <= second && adj[i][j] != first) 
				second = adj[i][j]; 
		}
	} 
	return second; 
} 

void recursion(int size, int adj[size][size], int curr_bound, int curr_weight, int level, int curr_path[size+1], int visited[size]){ 

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


	for (int i = 0; i < size; i++){ 
		if (adj[curr_path[level-1]][i] != 0 && visited[i] == 0){ 
			int temp = curr_bound; 
			curr_weight += adj[curr_path[level - 1]][i]; 
  
			if (level==1) {
				curr_bound -= ((secondMin(size, adj, curr_path[level - 1]) + firstMin(size, adj, i))/2); 
			} else{
				curr_bound -= ((secondMin(size, adj, curr_path[level - 1]) + firstMin(size, adj, i))/2); 
  			}

			if (curr_bound + curr_weight < final_res){ 
				curr_path[level] = i; 
				visited[i] = 1; 
  
				recursion(size, adj, curr_bound, curr_weight, level + 1, curr_path, visited);

			} 
  
			curr_weight -= adj[curr_path[level-1]][i]; 
			curr_bound = temp; 
		  	
 			memset(visited, 0, sizeof(*visited)*size);
			for (int j = 0; j <= level - 1; j++) {
				visited[curr_path[j]] = 1; 
			}	

		}
	}
}


void second_node(int size, int adj[size][size], int curr_bound, int curr_path[size+1], int visited[size], int j){
				int level = 1;
				int temp = curr_bound; 

				int curr_weight = adj[curr_path[level - 1]][j]; 

				curr_bound -= ((secondMin(size, adj, curr_path[level - 1]) + firstMin(size, adj, j))/2); 
				curr_path[level] = j; 
				visited[j] = 1; 

				recursion(size, adj, curr_bound, curr_weight, 2, curr_path, visited); 
	 		

}
  
void first_node(int size, int adj[size][size]){


	int init_bound = 0; 
  
	for (int i = 0; i < size; i++) {
		init_bound += (firstMin(size, adj, i) + secondMin(size, adj, i)); 
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
					second_node(size, adj, curr_bound, curr_path, visited, j); 


	 			}


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

	first_node(size, adj);
   	
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