#ifndef ARGUMENTS_H_INCLUDED
#define ARGUMENTS_H_INCLUDED 

#include <argp.h>

static error_t parse_opt(int, char*, struct argp_state*);

extern int num;

const char *argp_program_version = "1.0";
const char *argp_program_bug_address = "<dimitris23bp@gmail.com>";
static char doc[] = "The arguments for the TSP.";
static char args_doc[] = "[FILENAME]...";
static struct argp_option options[] = { 
    { "read",	'r',	0, 0, "Read from file" },
    { "write",	'w',	0, 0, "Write to file" },
    { "size",	's',	"SIZE", 0, "Specific size of the array" },
    { "file",	'f',	"NAME", 0, "Specific name of the file" },
    { "threads",'t',    "THREADS", 0, "NUmber of threads for parallel solutions"},
    { "minimum",'m',    "MINIMUM", 0, "Minimum distance of nodes"},
    { "maximum",'M',    "MAXIMUM", 0, "Maximum distance of nodes"},
    { 0 } 
};

typedef struct arguments {
	char* file_name;
    enum { READ_MODE, WRITE_MODE } mode;
	int size;
    char* option;
    int num_of_threads;
    int minimum;
    int maximum;
} arguments;


static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

#endif