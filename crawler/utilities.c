#define _BSD_SOURCE
#include <ctype.h>
#include <dirent.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "pool.h"
extern pool_t pool;
//link set for
link_set set;
pthread_mutex_t l_mut = PTHREAD_MUTEX_INITIALIZER;

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

//check if an string is ip or domain name
int is_ip(char *host_or_ip){
  return isdigit(host_or_ip[strlen(host_or_ip)]);
}

//check if link contains host in front
//and return pure link
char *extract_link(char *host, char*link){
  if(!strncmp(host, link, strlen(host)))
    return link+strlen(host);
  return link;
}

//simple util to free 2d array
void free_2darray(char **array, int size){
  if(!array)
    return;
  for(int i=0; i<size; i++)
    if(array[i]) free(array[i]);
  free(array);
}

//checks if a directory exists and if not, creates it
int check_dir(char *mixed_path){
  static struct stat st = {0};
  char *folder = NULL;
  int i;
  for(i=strlen(mixed_path)-1; i>=0; i--)
    if(mixed_path[i] == '/')
      break;
  if(!i) return 0;
  printf("mixed path: %s, i :%d\n", mixed_path, i);
  folder = malloc(i+1);
  strncpy(folder, mixed_path, i);
  folder[i] = '\0';
  printf("folder: %s\n", folder);
  if (stat(folder, &st) == -1)
    mkdir(folder, 0700); 
  free(folder);
  return 1;
}

void parse_links(char *data, char ***links, int *linksize){
  char *temp = NULL;
  static char *href = "<a href='";
  int sz;
  *linksize = 0;
  *links = malloc(0);
  temp = strstr(data, href);
  while(temp){
    *links = realloc(*links, ((*linksize)+1)*sizeof(char*));
    sz = 0;
    temp +=9;
    while(*temp !='/') temp++;
    while(temp[sz]!='\'') sz++;
    (*links)[*linksize] = malloc(sz+1);
    strncpy( (*links)[*linksize], temp, sz);
    (*links)[*linksize][sz] = '\0';
    (*linksize)++;
    temp = strstr(temp, href);
  }
}

void insert_links(char **links, int linksize){
  for(int i=0; i<linksize; i++){
    //printf("%s\n", links[i]);
    pthread_mutex_lock(&l_mut);
      if(!unsafe_search(&set, links[i])){
        unsafe_place(&set, links[i]);
        place(&pool, links[i]);
        pthread_cond_signal(&cond_nonempty);
      }
    pthread_mutex_unlock(&l_mut);
  }
}

void write_dirs(char *docname, char*save_dir){
  FILE *fp = fopen(docname, "w");
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(save_dir)) != NULL) {
      /* print all the files and directories within directory */
      while ((ent = readdir(dir)) != NULL) {
        if(ent->d_type != DT_DIR || !strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
          continue;
        //initialize new registry
        printf("%s\n", ent->d_name);
        fprintf(fp, "%s%s\n", save_dir, ent->d_name);
      }
  }
}