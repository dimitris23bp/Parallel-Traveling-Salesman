#include "headers/matrix_generator.h"
#include "headers/common_functions.h"

int main(int argc, char const *argv[]) {

  if(argc != 3){
    printf("Please insert two arguments with min and max size of arrays \n");
    return 0;
  }

  int min = atoi(argv[1]);
  int max = atoi(argv[2]);

  if(max < min){
    printf("Min value is greater than max\n");
    return 0;
  }

  for(int i = 0; i < 3; i++){
    for(int size = min; size <= max; size++){
      int size_of_array = 30;
      int adj[size_of_array][size_of_array];

      char file_name[size_of_array];
      snprintf(file_name, size_of_array, "example-arrays/file%d-%d.txt", size, i); // puts string into buffer
      printf("%s\n",file_name );

      generator(size, adj, 50, 99);
      write_to_file(size, adj, file_name);
    }
  }


  return 0;
}
