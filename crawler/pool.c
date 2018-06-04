#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "pool.h"

extern pthread_mutex_t mtx;
extern pthread_cond_t cond_nonempty;
extern pthread_cond_t cond_nonfull;

#define POOL_SIZE 30

void initialize(pool_t *pool){
  pool->start = 0;
  pool->end = -1;
  pool->size = 10;
  pool->data = malloc(10*sizeof(char*));
}

void place(pool_t *pool, char *data){
  pthread_mutex_lock(&mtx);
  if(pool->end+1 == pool->size){
    pool->data = realloc(pool->data, (pool->size+1)*sizeof(char*));
    pool->size++;
  }
  pool->end++;
  if(data){
    pool->data[pool->end] = malloc(strlen(data)+1);
    strcpy(pool->data[pool->end], data);
  }
  else
    pool->data[pool->end] = NULL;
  pthread_mutex_unlock(&mtx);
}

char *obtain(pool_t *pool){
  pthread_mutex_lock(&mtx);
  while(pool->end < 0) {
    printf(">> Found  Buffer  Empty \n");
    pthread_cond_wait(&cond_nonempty, &mtx);
  }
  char *data = pool->data[pool->end];
  pool->end--;
  pthread_mutex_unlock(&mtx);
  return data;
}

void initialize_set(link_set *set){
  set->cap = 10;
  set->size = 0;
  set->strings = malloc(10*sizeof(char*));
}

void unsafe_place(link_set *set, char *data){
  if(set->size+1 == set->cap){
    set->cap *=2;
    set->strings = realloc(set->strings, (set->cap)*sizeof(char*));
  }
  set->strings[set->size] = malloc(strlen(data)+1);
  strcpy(set->strings[set->size++], data);
}

//test if data is in set
int unsafe_search(link_set *set, char *data){
  for(int i=0; i<set->size; i++)
    if(!strcmp(set->strings[i], data))
      return 1;
  return 0;
}

void delete_set(link_set *set){
  for(int i=0; i<set->size; i++)
    free(set->strings[i]);
  free(set->strings);
}
