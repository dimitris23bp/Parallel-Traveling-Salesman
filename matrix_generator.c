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
				printf("%d   ", *(*(adj + i) + j ));
			} else if(curr_num < 100){
				printf("%d  ", *(*(adj + i) + j ));
			} else {
				printf("%d ", *(*(adj + i) + j ));
			}

		}
		printf("\n");
	}

}

void write_to_file(int size, int (*adj)[size], char* file_name){

	FILE *file;
	file = fopen(file_name, "w+");

	//Add size in first row of the file
	fprintf(file,"%d\n", size);

	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j ++){
			fprintf(file,"%d ", *(*(adj + i) + j));
		}
		fprintf(file, "\n");
	}

	fclose(file);


}

void read_from_file(int size, int (*adj)[size], char* file_name){
	FILE *file;
	file = fopen(file_name, "r");

	printf("%s\n",file_name );

	int num = 0;
	int i = 0;
	int j = 0;

	fscanf (file, "%d", &num);      

	while (!feof (file)){  
		fscanf (file, "%d", &num);      
		*(*(adj + i) + j) = num;
		j++;

		if(j == size){
			if(i == size){
				break;
			}
			i++;
			j = 0;
		}



	}
	fclose(file);

}

int get_size_of_matrix(char* file_name){

	FILE *file;
	file = fopen(file_name, "r");
	
	int size = 0;

	fscanf (file, "%d", &size);      

	return size;
}
