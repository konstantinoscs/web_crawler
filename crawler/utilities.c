#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_arguments(int argc, char **argv, char **save_dir, int *no_threads,
  int *c_port, int *s_port, char **host, char **start_url){
  int i=1;
  if(argc !=12 ){
    fprintf(stderr, "Wrong number of arguments given\n");
    exit(-1);
  }
  while(i<argc){
    if(!strcmp(argv[i], "-d")){ //parse root_dir
      *save_dir = argv[++i];
    }
    else if(!strcmp(argv[i], "-t")){  //parse no of threads
      *no_threads = atoi(argv[++i]);
    }
    else if(!strcmp(argv[i], "-c")){  //parse command port
      *c_port = atoi(argv[++i]);
    }
    else if(!strcmp(argv[i], "-p")){  //parse server port
      *s_port = atoi(argv[++i]);
    }
		else if(!strcmp(argv[i], "-h")){  //parse host
      *host = argv[++i];
    }
		else{	//parse starting url
			*start_url = argv[i];
		}
    i++;  //move to the next arg one incremation has already been done
  }
  return 1;
}
