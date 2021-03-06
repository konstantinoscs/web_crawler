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

int map_day(char*days, int dayd){
	static char *day[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	if(sizeof(days)<4)
		return 0;
	strncpy(days, day[dayd], 3);
	days[3] = '\0';
	return 1;
}

int map_month(char*months, int monthd){
	static char *month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
		"Sep", "Oct", "Nov", "Dec"};
	if(sizeof(months) < 4)
		return 0;
	strncpy(months, month[monthd], 3);
	months[3] = '\0';
	return 1;
}

int read_file(char **message, FILE *fp){
  char *text = malloc(2);
  int wordm = 2, wordc=0; //start from a word with 2 characters
  int ch;
  ch = fgetc(fp);
  while(ch != EOF){
    if(wordc+1 == wordm){  //realloc condition -- save space for '\0'
      wordm *= 2;
      text = realloc(text, wordm);
    }
    text[wordc++] = ch; //save character in text

    ch = fgetc(fp);
  } //text is saved exactly as read --including whitespace
  text[wordc] ='\0';
  text = realloc(text, wordc+1); //shrink to fit
  *message = text;
  return 1;
}

//simple util to free 2d array
void free_2darray(char **array, int size){
  if(!array)
    return;
  for(int i=0; i<size; i++)
    if(array[i]) free(array[i]);
  free(array);
}
