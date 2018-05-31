#include <stdlib.h>

#include "crawler.h"
#include "utilities.h"

int main(int argc, char **argv){
	char *save_dir = NULL, *host=NULL, *start_url=NULL;
	int no_threads=0, c_port=0, s_port=0;
	parse_arguments(argc, argv, &save_dir, &no_threads, &c_port, &s_port, &host, &start_url);
	crawler_operate(host, save_dir, start_url, c_port, s_port, no_threads);
}
