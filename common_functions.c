#include <limits.h>

void copy_to_final(int size, int* curr_path, int* final_path) 
{ 
	for (int i = 0; i < size; i++) {
		*(final_path + i) = curr_path[i];
	} 

	*(final_path + size) = curr_path[0]; 
} 

/*
* This function calculates the first and the second minimum of every node
* In other words, the two shortest distances from every node
*/
void find_mins(int size, int **first_mins, int **second_mins, int adj[size][size]) {

	for (int i = 0; i < size; i++) {

		// Initialize current index with the biggest possible integer 
		// Just to be sure that there is no way I could find a number greater than that
		*(*first_mins + i) = INT_MAX;
		*(*second_mins + i) = INT_MAX;

		for (int j = 0; j < size; j++) {
			// If the node isn't traveling to itself
			if (i != j) {

				// Calculate if it is first, second or none of them
				if (adj[i][j] <= *(*first_mins + i)) {
					*(*second_mins + i) = *(*first_mins + i);
					*(*first_mins + i) = adj[i][j];
				}
				else if (adj[i][j] <= *(*second_mins + i))
					*(*second_mins + i) = adj[i][j];
			}
		}
	}
}
