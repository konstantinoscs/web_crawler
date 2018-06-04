#ifndef POOL_H
#define POOL_H

typedef struct{
  char **data;
  int start;
  int end;
  int size;
}pool_t;

typedef struct{
  char **strings;
  int cap;  //capacity
  int size;
}link_set;

pthread_mutex_t mtx;
pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;
int stuck;
pool_t pool;

void initialize(pool_t *pool);
void place(pool_t *pool, char *data);
char *obtain(pool_t *pool);

void initialize_set(link_set *set);
void unsafe_place(link_set *set, char *data);
//test if data is in set
int unsafe_search(link_set *set, char *data);
void delete_set(link_set *set);
#endif
