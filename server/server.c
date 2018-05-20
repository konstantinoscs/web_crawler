#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

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

  if((*sock = socket(AF_INET , SOCK_STREAM , 0)) ==  -1){
    perror("Socket  creation  failed!"); exit(-1); }

  if(bind(*sock, (struct sockaddr*) &server, sizeof(server))<0){
    perror("bind failed"); exit(-1); }

  if(listen(*sock, 300) < 0){
    perror("listen"); exit(-1); }

  return 1;
}

void *thread_serve(void *i){
  int fd;
  printf("Thread %d!\n", *(int *)i);
  while(1){
    fd = obtain(&pool);
    printf("Got fd:%d\n", fd);
  }
  pthread_exit(NULL);
}

void make_fds_array(int c_sock, int s_sock, struct pollfd *fds){
  fds[0].fd = s_sock;
  fds[1].fd = c_sock;
  fds[0].events = fds[1].events = POLLIN;
}

int server_operate(int no_threads, int c_port, int s_port){
  int c_sock, s_sock, newsock;
  struct pollfd fds[2];
  pthread_t *threads = NULL;
  //create threads
  if((threads = malloc(no_threads*sizeof(pthread_t))) == NULL){
    perror("threads malloc:"); exit(-2); }
  initialize(&pool);
  pthread_mutex_init(&mtx, 0);
  pthread_cond_init(&cond_nonempty, 0);
  pthread_cond_init(&cond_nonfull, 0);
  //create threads
  for(int i=0; i<no_threads; i++){
    if(pthread_create(threads+i, NULL, thread_serve, (void *)&i)){
      perror("thread create:"); exit(-3); }
  }
  //set the two sockets
  set_socket(s_port, &s_sock);
  set_socket(c_port, &c_sock);
  make_fds_array(c_sock, s_sock, fds);
  while(1){
    if(poll(fds, 2, -1) == -1){
        perror("Error in poll "); exit(-4); }

    //request was received
    if(fds[0].revents == POLLIN){
      //insert in pool
      if((newsock = accept(ssock , NULL, NULL)) < 0){
        perror("accept"); exit(-4);}
    }
    else if(fds[0].revents == POLLHUP){

    }

    //command was received
    if(fds[0].revents == POLLIN){

    }
    else if(fds[1].revents == POLLHUP){

    }
  }

  for(int i=0; i<no_threads; i++){
    if(pthread_join(threads[i], NULL)){
      perror("pthread_join"); exit(1);}
  }
  free(threads);
}
