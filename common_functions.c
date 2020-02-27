#include <stdio.h>

void copy_to_final(int size, int* curr_path, int* final_path) 
{ 
	for (int i = 0; i < size; i++) {
		*(final_path + i) = curr_path[i];
	} 

	*(final_path + size) = curr_path[0]; 
} 
