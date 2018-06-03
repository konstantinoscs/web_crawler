#include <arpa/inet.h>
#include <ctype.h>
#include <pthread.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "crawler.h"
#include "pool.h"

extern pthread_mutex_t mtx;
extern pthread_cond_t cond_nonempty;
extern pthread_cond_t cond_nonfull;
extern pool_t pool;

int set_socket(int port, int *sock){
  //int sock;
  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(port);

  if((*sock = socket(AF_INET, SOCK_STREAM, 0)) ==  -1){
    perror("Socket  creation  failed!"); exit(-1); }

  if(bind(*sock, (struct sockaddr*) &server, sizeof(server))<0){
    perror("bind failed"); exit(-1); }

  if(listen(*sock, 300) < 0){
    perror("listen"); exit(-1); }

  return 1;
}

int is_ip(char *host_or_ip){
  return isdigit(host_or_ip[strlen(host_or_ip)]);
}

void *thread_crawl(void *info){
  ThreadInfo *t_info = (ThreadInfo *) info;
  int sock, linksize;
  char *site = NULL;
  char **links = NULL;
  if((sock = socket(AF_INET , SOCK_STREAM , 0)) < 0){
    perror("Socket"); exit(-2);}
  if(connect(sock, t_info->serverptr, t_info->s_size) < 0){
    perror("Connect to server: "); exit(-2);}
  while(1){
    site = obtain(&pool);
    printf("Got site: %s\n", site);
    //wget(sock, page, &links, &linksize);
    free(site);
  }
}

int crawler_operate(char *host, char *save_dir, char *start_url, int c_port,
  int s_port, int no_threads){

  int c_sock, s_sock, newsock;
  pthread_t *threads = NULL;
  //client variables
  struct sockaddr_in server;
  struct sockaddr *serverptr = (struct sockaddr *)&server;
  struct hostent *rem;
  struct in_addr host_address;
  ThreadInfo t_info;


  //set client
  if(is_ip(host)){
    inet_aton(host, &host_address);
    rem = gethostbyaddr((const char*) &host_address, sizeof(host_address), AF_INET);
  }
  else{
    if((rem = gethostbyname(host)) == NULL){
      perror("Host by name: "); exit(-1);}
  }

  server.sin_family = AF_INET;
  memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
  server.sin_port = htons(s_port);
  t_info.s_size = sizeof(server);
  t_info.serverptr = serverptr;
  t_info.save_dir = save_dir;

  if((threads = malloc(no_threads*sizeof(pthread_t))) == NULL){
    perror("threads malloc:"); exit(-2); }
  initialize(&pool);
  pthread_mutex_init(&mtx, NULL);
  pthread_cond_init(&cond_nonempty, NULL);
  pthread_cond_init(&cond_nonfull, NULL);

  //create threads
  for(int i=0; i<no_threads; i++){
    if(pthread_create(threads+i, NULL, thread_crawl, (void *)&t_info)){
      perror("thread create:"); exit(-3); }
  }
  //set the socket
  set_socket(c_port, &c_sock);
  //set client part


  //start crawling

  for(int i=0; i<no_threads; i++)
    if(pthread_join(threads[i], NULL)){
      perror("pthread_join"); exit(1);}
  free(threads);
}
