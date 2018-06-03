#ifndef POOL_H
#define POOL_H

typedef struct{
  char **data;
  int start;
  int end;
  int size;
}pool_t;

pthread_mutex_t mtx;
pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;
pool_t pool;

void initialize(pool_t *pool);
void place(pool_t *pool, char *data);
char *obtain(pool_t *pool);
#endif
