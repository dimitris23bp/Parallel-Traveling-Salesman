#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "matrix_generator.h"

#define N 20

unsigned int *final_path;
unsigned int final_res = UINT_MAX; 

void copyToFinal(int size, int* curr_path) 
{ 
	for (int i = 0; i < size; i++) {
		*(final_path + i) = curr_path[i];
	} 

	*(final_path + size) = curr_path[0]; 
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
		  	
 			memset(visited, 0, sizeof(*visited)*size);
			for (int j = 0; j <= level - 1; j++) {
				visited[curr_path[j]] = 1; 
			}	
			

		}
	}
}
  
void first_node(int size, int adj[size][size]){

	int curr_path[size+1]; 
 	memset(curr_path, -1, sizeof(curr_path));

 	int visited[size];
 	memset(visited, 0, sizeof(visited));

	int curr_bound = 0; 
  
	for (int i = 0; i < size; i++) {
		curr_bound += (firstMin(size, adj, i) + secondMin(size, adj, i)); 
	}

	if(curr_bound == 1){
		curr_bound = curr_bound / 2 + 1;
	} else {
		curr_bound = curr_bound / 2; 
	}

	visited[0] = 1; 
	curr_path[0] = 0; 
  
	recursion(size, adj, curr_bound, 0, 1, curr_path, visited); 
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
	clock_t begin = clock();

	first_node(size, adj);
   	 
    printf("Minimum cost : %d\n", final_res); 
	printf("Path Taken : "); 
	for (int i = 0; i <= size; i++){ 
		printf("%d ", final_path[i]); 
	} 

	printf("\n");

	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("Time spent: %f\n", time_spent);


	return 0;
}