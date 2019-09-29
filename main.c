#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <pthread.h>

#define N 4

unsigned int final_path[N+1]; 
int visited[N]; 
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

void TSPRec(int adj[N][N], int curr_bound, int curr_weight, int level, int curr_path[N+1]){ 

	if (level == N){ 

		if (adj[curr_path[level - 1]][curr_path[0]] != 0){ 
			int curr_res = curr_weight + adj[curr_path[level-1]][curr_path[0]]; 
	  
			if (curr_res < final_res){ 
				copyToFinal(curr_path); 
				final_res = curr_res; 
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
  
				TSPRec(adj, curr_bound, curr_weight, level + 1, curr_path); 
			} 
  
			curr_weight -= adj[curr_path[level-1]][i]; 
			curr_bound = temp; 
		  	
 			memset(visited, 0, sizeof(visited));
			for (int j = 0; j <= level - 1; j++) {
				visited[curr_path[j]] = 1; 
			}
			

		}
	}
}
  
void TSP(int adj[N][N]){

	int curr_path[N+1]; 

 	memset(curr_path, -1, sizeof(curr_path));
 	memset(visited, 0, sizeof(visited));

	int curr_bound = 0; 
  
	for (int i = 0; i < N; i++) {
		curr_bound += (firstMin(adj, i) + secondMin(adj, i)); 
	}

	if(curr_bound == 1){
		curr_bound = curr_bound / 2 + 1;
	} else {
		curr_bound = curr_bound / 2; 
	}

	visited[0] = 1; 
	curr_path[0] = 0; 
  
	TSPRec(adj, curr_bound, 0, 1, curr_path); 
} 


void* thread_stuff(void* vargp){
	
	int (*adj)[N] = vargp;

	TSP(adj);
}

int main(int argc, char const *argv[]){
	//Adjacency matrix for the given graph 
	int adj[N][N] = {{0, 10, 15, 20}, {10, 0, 35, 25}, {15, 35, 0, 30}, {20, 25, 30, 0} }; 

	pthread_t tid;

  	pthread_create(&tid, NULL, thread_stuff, (void*)&adj);
   	 
  	pthread_join(tid, NULL);
    printf("Minimum cost : %d\n", final_res); 
	printf("Path Taken : "); 
	for (int i = 0; i <= N; i++){ 
		printf("%d ", final_path[i]); 
	} 

	printf("\n");


	return 0;
}