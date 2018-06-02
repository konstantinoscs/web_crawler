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
  printf("sizeof(server): %d\n", t_info->s_size);
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

  if((threads = malloc(no_threads*sizeof(pthread_t))) == NULL){
    perror("threads malloc:"); exit(-2); }

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
