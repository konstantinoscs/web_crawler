#include <stdlib.h>

#include "utilities.h"

int main(int argc, char **argv){
	char *root_dir = NULL, *host=NULL, *start_url=NULL; 
	int no_threads=0, c_port=0, s_port=0; 
	parse_arguments(argc, argv, &root_dir, &no_threads, &c_port, &s_port, &host, &start_url);
}
