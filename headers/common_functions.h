#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define SIZE 17

unsigned int final_res = UINT_MAX; 
unsigned int *final_path;

void copy_to_final(int size, int* curr_path, int* final_path);
