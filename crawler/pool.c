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
  pool->count = 0;
  //pool->data = malloc(30*sizeof(int));
}

void place(pool_t *pool, int data){
  pthread_mutex_lock(&mtx);
  while(pool->count >= POOL_SIZE){
    printf(">> Found  Buffer  Full \n");
    pthread_cond_wait(&cond_nonfull, &mtx);
  }
  pool->end = (pool->end + 1) % POOL_SIZE;
  pool->data[pool->end] = data;
  pool->count ++;
  pthread_mutex_unlock(&mtx);
}

int obtain(pool_t *pool){
  int data = 0;
  pthread_mutex_lock (&mtx);
  while(pool->count <= 0) {
    printf(">> Found  Buffer  Empty \n");
    pthread_cond_wait(&cond_nonempty, &mtx);
  }
  data = pool ->data[pool->start];
  pool->start = (pool ->start + 1) % POOL_SIZE;
  pool->count --;
  pthread_mutex_unlock (&mtx);
  return data;
}
