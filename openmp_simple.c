#include <omp.h>

#include "headers/matrix_generator.h"
#include "headers/common_functions.h"
#include "headers/arguments.h"

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
    case 'r':
       	arguments->mode = READ_MODE;
    	break;
    case 'w':
       	arguments->mode = WRITE_MODE;
    	break;
    case 's':
    	arguments->size = atoi(arg);
    	break;
    case 'f':
    	arguments->file_name = arg;
    	break;
	case 't':
		arguments->num_of_threads = atoi(arg);
		break;
    case ARGP_KEY_ARG: return 0;
    default: return ARGP_ERR_UNKNOWN;
    }   
    return 0;
}

void recursion(
	int size, 
	int adj[size][size], 
	int curr_bound, 
	int curr_weight, 
	int level, 
	int curr_path[size+1], 
	int visited[size], 
	int** first_mins, int** second_mins){ 

	if (level == size){ 

		//I don't think this will ever be 0
		if (adj[curr_path[level - 1]][curr_path[0]] != 0){ 
			int curr_res = curr_weight + adj[curr_path[level-1]][curr_path[0]]; 
	  
	  		#pragma omp critical
			{

				if (curr_res < final_res){ 
					copy_to_final(size, curr_path, final_path); 
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

			//I THINK I need to turn that back
			curr_path[level] = -1;

		}
	}
}


void second_node(
	int size, 
	int adj[size][size], 
	int curr_bound, 
	int curr_path[size+1], 
	int visited[size], 
	int** first_mins, int** second_mins,
	int num_of_threads){

	for (int j = omp_get_thread_num()+1; j < size; j+=num_of_threads){		

		int temp = curr_bound; 
		curr_bound -= ((*(*second_mins + curr_path[0]) + *(*first_mins + j))/2); 
  			
		curr_path[1] = j; 
		visited[j] = 1; 

		recursion(size, adj, curr_bound, adj[curr_path[0]][j], 2, curr_path, visited, first_mins, second_mins); 

		curr_bound = temp; 
 		memset(visited, 0, sizeof(*visited)*size);
 		visited[0] = 1;

	}

}
  
void first_node(int size, int adj[size][size], int **first_mins, int **second_mins, int num_of_threads){


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
  

	omp_set_num_threads(num_of_threads);
	#pragma omp parallel firstprivate(init_bound)
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

		second_node(size, adj, curr_bound, curr_path, visited, first_mins, second_mins, num_of_threads); 

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


int main(int argc, char *argv[]){

	struct arguments arguments;

	arguments.size = SIZE;
	arguments.mode = WRITE_MODE;
	arguments.file_name = "example-arrays/file01.txt";
	arguments.num_of_threads = 8;

	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	//I need this to create the adj out of the scope of the below if statements
	if (arguments.mode == READ_MODE) {
		arguments.size = get_size_of_matrix(arguments.file_name);
	}

	int adj[arguments.size][arguments.size];

	if (arguments.mode == WRITE_MODE) {
		generator(arguments.size, adj, 50, 99);
		write_to_file(arguments.size, adj, arguments.file_name);
	} else {
		read_from_file(arguments.size, adj, arguments.file_name);
	}

	final_path = (int *)malloc(arguments.size * sizeof(int));
	
	//Starting time of solution
	double start = omp_get_wtime();

	//Get first_min and second_min as two arays instead of calling them each time 
	int *first_mins = malloc(arguments.size * sizeof(int));
	int *second_mins = malloc(arguments.size * sizeof(int));
	find_mins(arguments.size, &first_mins, &second_mins, adj);

	first_node(arguments.size, adj, &first_mins, &second_mins, arguments.num_of_threads);
   	
	//Finishing time of solution
	double finish = omp_get_wtime();

	printf("%d %d %f\n",arguments.num_of_threads, arguments.size, finish - start);

	return 0;
}