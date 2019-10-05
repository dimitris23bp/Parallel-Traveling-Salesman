#include <stdio.h>
#include <time.h>
#include <stdlib.h>

/*
This is the function that is going to create arrays of any size
Weights are totally random

Meanwhile, every array must obey some rules
1. Be symmetric
2. Contains only integers
*/
void generator(int size, int (*adj)[size], int max_distance){

	//Initializion for rand();
	srand(time(NULL));

	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){

			if(j < i){
				//Include max_distance value too
				*(*(adj + i) + j) = (rand() % max_distance) + 1;
				*(*(adj + j) + i) = *(*(adj + i) + j);
			} else if (j == i){
				*(*(adj + i) + j ) = 0;
			} else {
				break;
			}

		}
	}
}

void display(int size, int (*adj)[size]){

	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){

			printf("%d ", *(*(adj + i) + j ));

		}
		printf("\n");
	}

}