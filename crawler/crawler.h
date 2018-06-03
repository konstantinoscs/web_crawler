#ifndef CRAWLER_H
#define CRAWLER_H

int crawler_operate(char *host, char *save_dir, char *start_url, int c_port,
  int s_port, int no_threads);

typedef struct ThreadInfo{
  int s_size;     //server size
  struct sockaddr *serverptr;
  char *save_dir;   //directory to save crawled pages
  int s_port;     //service port of server
}ThreadInfo;

#endif
