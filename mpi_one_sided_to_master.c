#include "mpi.h"
#include <time.h>

#include "headers/matrix_generator.h"
#include "headers/common_functions.h"
#include "headers/arguments.h"

double begin = 0;
int shared_size = 2;
MPI_Win win;
int my_final_res = INT_MAX;

/*
 * parse_opt is a function required by Argp library
 * Every case is a different argument
 */
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

/*
 * Recursion is the main function that gets accessed for almost every node
 * (except for first and second node)
 */
void recursion(
	int size,
	int adj[size][size],
	int curr_bound,
	int curr_weight,
	int level,
	int curr_path[size + 1],
	int visited[size],
	int** first_mins, int** second_mins,
	int rank) {

	// If every node has been visited
	if (level == size) {

		int curr_res = curr_weight + adj[curr_path[level - 1]][curr_path[0]];

		// If my current result is less than the best so far
		// Copy current into best (result and path too)
		if (curr_res < final_res) {

			copy_to_final(size, curr_path, final_path);
			final_res = curr_res;
			my_final_res = final_res;

			MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win);
			MPI_Put(&final_res, shared_size, MPI_INT, 0, 0, shared_size, MPI_INT, win);
			MPI_Win_unlock(0, win);

		}

		int before_get = final_res;

		MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win);
		MPI_Get(&final_res, shared_size, MPI_INT, 0, 0, shared_size, MPI_INT, win);
		MPI_Win_unlock(0, win);

		if (before_get < final_res) {
			final_res = before_get;
		}

		return;
	}

	// Go through every node
	for (int i = 1; i < size; i++) {

		// Check if the node has been visited
		if (visited[i] == 0) {

			// Change variables due to the next visiting
			int temp = curr_bound;
			curr_weight += adj[curr_path[level - 1]][i];
			curr_bound -= ((*(*second_mins + curr_path[level - 1]) + *(*first_mins + i)) / 2);

			// If current result is less than the bound
			if (curr_bound + curr_weight < final_res) {
				curr_path[level] = i;
				visited[i] = 1;
				recursion(size, adj, curr_bound, curr_weight, level + 1, curr_path, visited, first_mins, second_mins, rank);
			}

			// Restore variables back to normal
			// Before the current node had been visited
			curr_bound = temp;
			curr_weight -= adj[curr_path[level - 1]][i];
			visited[i] = 0;

			// If uncommented, the current path is not always correct during execution
			// Because it contains previous paths (before backtracking)
			// The outcome is always the same
			// Every other variable is necessary to change because their values are being compared
			// I keep it for the better understanding of the program
			//curr_path[level] = -1;
		}
	}
}

void second_node(
	int size,
	int adj[size][size],
	int curr_bound,
	int curr_path[size + 1],
	int visited[size],
	int** first_mins, int** second_mins,
	int rank,
	int numtasks) {

	for (int i = rank + 1; i < size; i += numtasks) {

		double start = MPI_Wtime();

		int temp = curr_bound;
		curr_bound -= ((*(*second_mins + curr_path[0]) + *(*first_mins + i)) / 2);

		curr_path[1] = i;
		visited[i] = 1;

		recursion(size, adj, curr_bound, adj[curr_path[0]][i], 2, curr_path, visited, first_mins, second_mins, rank);

		curr_bound = temp;
		memset(visited, 0, sizeof(int)*size);
		visited[0] = 1;
	}
}

void first_node(
	int size,
	int adj[size][size],
	int numtasks,
	int rank,
	int** first_mins, int** second_mins) {

	int curr_path[size + 1];
	memset(curr_path, - 1, sizeof(curr_path));
	curr_path[0] = 0;

	int visited[size];
	memset(visited, 0, sizeof(visited));
	visited[0] = 1;

	int init_bound = 0;
	init_bound = *(*first_mins);

	for (int i = 1; i < size; i++) {
		init_bound += *(*first_mins + i) + *(*second_mins + i);
	}

	init_bound = init_bound / 2;

	second_node(size, adj, init_bound, curr_path, visited, first_mins, second_mins, rank, numtasks);
}

int main(int argc, char *argv[]) {

	int rank, numtasks;
	int sharedbuffer, localbuffer;

	// Index of the rank with the best result
	int index = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Win_create(&sharedbuffer, shared_size, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

	for (int i = 0; i < shared_size; i++) {
		sharedbuffer = INT_MAX;
	}

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

	// I add one more because of a corrupted size vs. prev_size error. Probably needs it to store \0 byte at the end
	final_path = (int*) malloc(arguments.size * sizeof(int) + 1);

	// Check if the memory has been successfully
	// allocated by malloc or not
	if (final_path == NULL) {
		printf("Memory not allocated.\n");
		exit(0);
	}

	//Starting time of solution
	begin = MPI_Wtime();

	//Get first_min and second_min as two arays instead of calling them each time
	int *first_mins = malloc(arguments.size * sizeof(int));
	int *second_mins = malloc(arguments.size * sizeof(int));
	find_mins(arguments.size, &first_mins, &second_mins, adj);

	// This is the only function of the solution in main. Everything else is into this
	first_node(arguments.size, adj, numtasks, rank, &first_mins, &second_mins);

	int finals[numtasks];

	MPI_Gather(&my_final_res, 1, MPI_INT, finals, 1, MPI_INT, numtasks - 1, MPI_COMM_WORLD);

	// Find the minimum of each rank's minimum
	if (rank == numtasks - 1)
	{
		for (int i = 0; i < numtasks; i++) {
			if (finals[i] < my_final_res) {
				final_res = finals[i];
				index = i;
			}
		}

		MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win);
		MPI_Put(&index, 1, MPI_INT, 0, 0, 1, MPI_INT, win);
		MPI_Win_unlock(0, win);

	}

	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Win_fence(0, win);
	MPI_Get(&index, 1, MPI_INT, 0, 0, 1, MPI_INT, win);
	MPI_Win_fence(0, win);

	// Display minimum cost and the path of it
	if (rank == index) {

		double end = MPI_Wtime();
		double time_spent = (double)(end - begin);

		printf("%f", time_spent);
	}

	MPI_Win_free(&win);
	MPI_Finalize();

	return 0;
}
