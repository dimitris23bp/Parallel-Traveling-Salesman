#include <time.h>

#include "matrix_generator.h"
#include "common_functions.h"
#include "arguments.h"

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
	int **first_mins, int **second_mins){ 

	if (level == size){ 

		//if (adj[curr_path[level - 1]][curr_path[0]] != 0){ 
			int curr_res = curr_weight + adj[curr_path[level-1]][curr_path[0]]; 
			
			if (curr_res < final_res){ 
				copy_to_final(size, curr_path, final_path); 
				final_res = curr_res; 

			}
		//} 			

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
		  	
 			memset(visited, 0, sizeof(*visited)*size);
			for (int j = 0; j <= level - 1; j++) {
				visited[curr_path[j]] = 1; 
			}	

			curr_path[level] = -1;


		}
	}
}
  
void first_node(int size, int adj[size][size], int **first_mins, int **second_mins){

	int curr_path[size+1]; 
 	memset(curr_path, -1, sizeof(curr_path));

 	int visited[size];
 	memset(visited, 0, sizeof(visited));

	int curr_bound = 0; 
  	curr_bound = *(*first_mins);
  	
	for (int i = 1; i < size; i++) {
		curr_bound += *(*first_mins + i) + *(*second_mins + i); 
	}


	if(curr_bound == 1){
		curr_bound = curr_bound / 2 + 1;
	} else {
		curr_bound = curr_bound / 2; 
	}

	visited[0] = 1; 
	curr_path[0] = 0; 
  
	recursion(size, adj, curr_bound, 0, 1, curr_path, visited, first_mins, second_mins); 
} 

// Should somehow transfer this into stack 
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

    //display(arguments.size, adj);

	final_path = (int *)malloc(arguments.size * sizeof(int));

	//Starting time of solution
	clock_t begin = clock();

	//Get first_min and second_min as two arays instead of calling them each time 
	int *first_mins = malloc(arguments.size * sizeof(int));
	int *second_mins = malloc(arguments.size * sizeof(int));
	find_mins(arguments.size, &first_mins, &second_mins, adj);

	first_node(arguments.size, adj, &first_mins, &second_mins);
   	 
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("1 %d %f\n",arguments.size, time_spent);


	return 0;
}