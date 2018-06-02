#ifndef CRAWLER_H
#define CRAWLER_H

int crawler_operate(char *host, char *save_dir, char *start_url, int c_port,
  int s_port, int no_threads);

typedef struct ThreadInfo{
  int s_size;
  struct sockaddr *serverptr;
}ThreadInfo;

#endif