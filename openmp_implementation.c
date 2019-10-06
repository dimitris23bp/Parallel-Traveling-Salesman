#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <omp.h>
#include <time.h>

#include "matrix_generator.h"

#define N 20
#define NUM_OF_THREADS 4

//Example path - May not be used 
int adj[N][N] = {{0,13,51,18,15,83,27,44,69,34,15,44,35, 52, 27, 70, 64, 61, 9, 88}, 
{13, 0, 33, 3, 28, 11, 56, 3, 64, 18, 61, 6, 75, 76, 43, 49, 44, 73, 73, 92},
{51, 33, 0, 88, 15, 53, 12, 44, 62, 95, 23, 88, 79, 14, 71, 15, 68, 69, 33, 16},
{18, 39 ,88 ,0, 14, 87, 96, 94 ,34 ,48 ,78 ,35 ,30 ,63 ,94 ,77 ,58 ,37 ,21 ,30},
{15, 28, 15, 14, 0 ,57 ,37 ,41, 66, 31, 8 ,27 ,23 ,86 ,36 ,52 ,66 ,25 ,66 ,28},
{83, 11, 53, 87, 57, 0 ,81 ,83, 94, 30, 12, 3 ,61 ,65 ,85 ,90 ,61 ,11 ,82 ,52},
{27, 56, 12, 96, 37, 81, 0 ,40, 3 ,59 ,10 ,97 ,11 ,29 ,29 ,90 ,93 ,79 ,14 ,18},
{44, 3 ,44 ,94 ,41 ,83 ,40 ,0 ,68 ,35 ,51 ,60 ,33 ,38 ,11 ,67 ,50 ,43 ,20 ,81},
{69, 64, 62, 34, 66, 94, 3 ,68, 0 ,68 ,14 ,68 ,95 ,60 ,63 ,75 ,41 ,37 ,98 ,4},
{34, 18, 95, 48, 31, 30, 59, 35, 68, 0, 53, 42, 78, 86, 10, 6 ,3, 84, 15, 38},
{15, 61, 23, 78, 8 ,12 ,10 ,51, 14, 53, 0 ,62 ,99 ,25 ,85 ,95 ,65 ,89 ,7, 22},
{44, 6 ,88 ,35 ,27 ,3, 97, 60 ,68 ,42 ,62 ,0, 7, 47, 76, 65, 2 ,1, 89, 20},
{35, 75, 79, 30, 23, 61, 11, 33, 95, 78, 99, 7, 0 ,12 ,72 ,18 ,87 ,53 ,88 ,4},
{52, 76, 14, 63, 86, 65, 29, 38, 60, 86, 25, 47, 12, 0, 17, 32, 40, 20, 61, 82},
{27, 43, 71, 94, 36, 85, 29, 11, 63, 10, 85, 76, 72, 17, 0, 89, 73, 42, 50, 44},
{70, 49, 15, 77, 52, 90, 90, 67, 75, 6, 95, 65, 18, 32, 89, 0 ,3, 16, 12, 36},
{64, 44, 68, 58, 66, 61, 93 ,50, 41, 3, 65, 2 ,87 ,40 ,73 ,30, 15, 82, 54, 78},
{61, 73, 69, 37, 25, 11, 79, 43, 37, 8, 48, 9, 53, 20, 42, 16, 15, 0 ,38 ,61},
{9 ,73 ,33 ,21 ,66 ,82 ,14 ,20, 98, 15, 7 ,89 ,88 ,61 ,50 ,12 ,82 ,38 ,0, 70},
{88, 92, 16, 30, 28, 52, 18, 81, 4, 38, 22, 20, 4, 82, 44 ,36, 54, 61, 70, 0}};

unsigned int final_path[N+1]; 
unsigned int final_res = UINT_MAX; 

void copyToFinal(int* curr_path) 
{ 
	for (int i = 0; i < N; i++) {
		final_path[i] = curr_path[i];
	} 

	final_path[N] = curr_path[0]; 
} 
  
int firstMin(int adj[N][N], int i) 
{ 
	int min = INT_MAX; 
	
	for (int k = 0; k < N; k++){ 
		if (adj[i][k] < min && i != k){ 
			min = adj[i][k]; 
		}
	}
	
	return min;
} 
  
int secondMin(int adj[N][N], int i) 
{ 
	int first = INT_MAX;
	int second = INT_MAX;

	for (int j=0; j<N; j++){ 
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

void recursion(int adj[N][N], int curr_bound, int curr_weight, int level, int curr_path[N+1], int visited[N]){ 

	if (level == N){ 
		if (adj[curr_path[level - 1]][curr_path[0]] != 0){ 
			int curr_res = curr_weight + adj[curr_path[level-1]][curr_path[0]]; 
	  		#pragma omp critical
	  		{
				if (curr_res < final_res){
					copyToFinal(curr_path); 
					final_res = curr_res; 
				}
			} 
		} 			

		return; 
		
	} 


	for (int i = 0; i < N; i++){ 
		if (adj[curr_path[level-1]][i] != 0 && visited[i] == 0){ 
			int temp = curr_bound; 
			curr_weight += adj[curr_path[level - 1]][i]; 
  
			if (level==1) {
				curr_bound -= ((firstMin(adj, curr_path[level - 1]) + firstMin(adj, i))/2); 
			} else{
				curr_bound -= ((secondMin(adj, curr_path[level - 1]) + firstMin(adj, i))/2); 
  			}

			if (curr_bound + curr_weight < final_res){ 
				curr_path[level] = i; 
				visited[i] = 1; 
  
				recursion(adj, curr_bound, curr_weight, level + 1, curr_path, visited);

			} 
  
			curr_weight -= adj[curr_path[level-1]][i]; 
			curr_bound = temp; 
		  	
 			memset(visited, 0, sizeof(*visited)*N);
			for (int j = 0; j <= level - 1; j++) {
				visited[curr_path[j]] = 1; 
			}	

		}
	}
}


void second_node(int adj[N][N], int curr_bound, int curr_weight, int level, int curr_path[N+1], int visited[N]){

	for (int j = omp_get_thread_num()+1; j < N; j+=NUM_OF_THREADS){
		
		int temp = curr_bound; 
		curr_weight += adj[curr_path[level - 1]][j]; 
		curr_bound -= ((firstMin(adj, curr_path[level - 1]) + firstMin(adj, j))/2); 
  			
		curr_path[level] = j; 
		visited[j] = 1; 

		recursion(adj, curr_bound, curr_weight, 2, curr_path, visited); 

		curr_weight = 0;
		curr_bound = temp; 
 		memset(visited, 0, sizeof(*visited)*N);
 		visited[0] = 1;

	}


}
  
void first_node(int adj[N][N]){


	int init_bound = 0; 
  
	for (int i = 0; i < N; i++) {
		init_bound += (firstMin(adj, i) + secondMin(adj, i)); 
	}

	if(init_bound == 1){
		init_bound = init_bound / 2 + 1;
	} else {
		init_bound = init_bound / 2; 
	}
  

	omp_set_num_threads(NUM_OF_THREADS);
	#pragma omp parallel
	{
		//Declare curr_path and set it to start from 0 node
		int curr_path[N+1]; 
	 	memset(curr_path, -1, sizeof(curr_path));
		curr_path[0] = 0; 

		//Declare visited nodes and set it to have visited 
		int visited[N]; 
 		memset(visited, 0, sizeof(visited));
		visited[0] = 1; 


	 	int curr_bound = init_bound;

		int id = omp_get_thread_num();

		second_node(adj, curr_bound, 0, 1, curr_path, visited); 

	}

} 

int main(int argc, char const *argv[]){

	generator(N, adj, 100);
	display(N, adj);

	//Starting time of solution
	double start = omp_get_wtime();

	first_node(adj);
   	 
    printf("Minimum cost : %d\n", final_res); 
	printf("Path Taken : "); 
	for (int i = 0; i <= N; i++){ 
		printf("%d ", final_path[i]); 
	} 

	printf("\n");

	//Finishing time of solution
	double finish = omp_get_wtime();

	printf("Time spent: %f\n", finish - start);

	return 0;
}