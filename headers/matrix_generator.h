void generator(int size, int (*adj)[size], int, int);
void display(int size, int (*adj)[size]);
void write_to_file(int size, int (*adj)[size], char* file_name);
int read_from_file(int size, int (*adj)[size], char* file_name);
int get_size_of_matrix(char* file_name);