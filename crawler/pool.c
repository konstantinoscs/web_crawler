#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
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
  pool->end = pool->end + 1;
  pool->data[pool->end] = data;
  pthread_mutex_unlock(&mtx);
}

char *obtain(pool_t *pool){
  pthread_mutex_lock(&mtx);
  while(pool->size <= 0) {
    printf(">> Found  Buffer  Empty \n");
    pthread_cond_wait(&cond_nonempty, &mtx);
  }
  char *data = pool->data[pool->end];
  pool->end--;
  pthread_mutex_unlock(&mtx);
  return data;
}
