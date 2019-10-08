#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/*
This is the function that is going to create arrays of any size
Weights are totally random

Meanwhile, every array must obey some rules
1. Be symmetric
2. Contains only integers
*/
void generator(int size, int (*adj)[size], int min_distance, int max_distance){

	//Initializion for rand();
	srand(time(NULL));

	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){

			if(j < i){

				//Include max_distance value too
				*(*(adj + i) + j) = min_distance + rand() % (max_distance + 1 - min_distance);
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

	int curr_num; 

	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){

			curr_num = *(*(adj + i) + j);
			if(curr_num < 10){
				printf("%d  ", *(*(adj + i) + j ));
			} else {
				printf("%d ", *(*(adj + i) + j ));

			}

		}
		printf("\n");
	}

}

void write_to_file(int size, int (*adj)[size]){

	FILE *file;
	file = fopen("file01.txt", "w+");

	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j ++){
			fprintf(file,"%d ", *(*(adj + i) + j));
		}
		fprintf(file, "\n");
	}

	fclose(file);


}

void read_from_file(int size, int (*adj)[size]){

	FILE *file;
	file = fopen("file01.txt", "r");

	int num = 0;
	int i = 0;
	int j = 0;

	while (!feof (file)){  
		fscanf (file, "%d", &num);      
		*(*(adj + i) + j) = num;
		printf("%d\n", *(*(adj + i) + j));

		j++;
		
		if(j == size){
			if(i == size){
				break;
			}
			i++;
			j = 0;
		}



	}

}