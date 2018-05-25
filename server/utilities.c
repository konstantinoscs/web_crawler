#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_arguments(int argc, char ** argv, char** root_dir, int *no_threads,
  int *c_port, int *s_port){
  int i=1;
  if(argc !=9 ){
    fprintf(stderr, "Wrong number of arguments given\n");
    exit(-1);
  }
  while(i<argc){
    if(!strcmp(argv[i], "-d")){ //parse root_dir
      *root_dir = malloc(strlen(argv[++i])+1);
      strcpy(*root_dir, argv[i]);
    }
    else if(!strcmp(argv[i], "-t")){  //parse no of threads
      *no_threads = atoi(argv[++i]);
    }
    else if(!strcmp(argv[i], "-c")){  //parse command port
      *c_port = atoi(argv[++i]);
    }
    else if(!strcmp(argv[i], "-p")){  //parse serving port
      *s_port = atoi(argv[++i]);
    }
    i++;  //move to the next arg one incremation has already been done
  }
  //printf("w is %d\n", *num_workers);
  return 1;
}

int map_day(int dayd, char**days){
	static char *day[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	if(sizeof(*days)<4){
		*days = NULL;
		return 0;
	}
	strncpy(*days, day[dayd], 3);
	return 1;
}
