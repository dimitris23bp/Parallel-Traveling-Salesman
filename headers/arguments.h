#ifndef ARGUMENTS_H_INCLUDED
#define ARGUMENTS_H_INCLUDED 

#include <argp.h>

static error_t parse_opt(int, char*, struct argp_state*);

extern int num;

const char *argp_program_version = "Travelling-Salesname 0.1";
const char *argp_program_bug_address = "<dimitris23bp@gmail.com>";
static char doc[] = "My program description.";
static char args_doc[] = "[FILENAME]...";
static struct argp_option options[] = { 
    { "read",	'r',	0, 0, "Read from file" },
    { "write",	'w',	0, 0, "Write to file" },
    { "size",	's',	"da_size", 0, "Specific size of the array" },
    { "file",	'f',	"file_name", 0, "Specific name of the file" },
    { "threads",'t',	"number of threads", 0, "NUmber of threads for parallel solutions"},
    { 0 } 
};

typedef struct arguments {
	char* file_name;
    enum { READ_MODE, WRITE_MODE } mode;
	int size;
    char* option;
    int num_of_threads;
} arguments;


static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

#endif