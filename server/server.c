#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

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

//thread method that serves GET requests
void *thread_serve(void *i){
  int fd;
  printf("Thread %d!\n", *(int *)i);
  while(1){
    fd = obtain(&pool);
    printf("Got fd:%d\n", fd);
    pthread_cond_signal(&cond_nonfull);
    if(fd==-1)
      break;
  }
  pthread_exit(NULL);
}

int command(int sock, clock_t start, int pages, long bytes){
  static char cmd[9];
  int i=0;
  while((read(sock, cmd+i, 1) > 0) && i<8 && cmd[i]!='\n') i++;
  cmd[i] = '\0';
  printf("Command: %s\n", cmd);
  if(!strcmp(cmd, "STATS")){
    clock_t end = clock();
    float uptime = (float) (end - start) / CLOCKS_PER_SEC;
    printf("Server up for %lf, served %d pages, %ld bytes\n", uptime, pages, bytes);
  }
  else if(!strcmp(cmd, "SHUTDOWN")){
    return 0;
  }
  else{
    printf("Unknown command!\n");
  }
  return 1;
}

void make_fds_array(int c_sock, int s_sock, struct pollfd *fds){
  fds[0].fd = s_sock;
  fds[1].fd = c_sock;
  fds[0].events = fds[1].events = POLLIN;
}

int server_operate(int no_threads, int c_port, int s_port){
  int c_sock, s_sock, newsock, pages=0;
  struct pollfd fds[2];
  pthread_t *threads = NULL;
  long bytes = 0;
  clock_t start = clock();
  //create threads
  if((threads = malloc(no_threads*sizeof(pthread_t))) == NULL){
    perror("threads malloc:"); exit(-2); }
  initialize(&pool);
  pthread_mutex_init(&mtx, 0);
  pthread_cond_init(&cond_nonempty, NULL);
  pthread_cond_init(&cond_nonfull, NULL);
  //create threads
  for(int i=0; i<no_threads; i++){
    if(pthread_create(threads+i, NULL, thread_serve, (void *)&i)){
      perror("thread create:"); exit(-3); }
  }
  //set the two sockets
  set_socket(s_port, &s_sock);
  set_socket(c_port, &c_sock);
  make_fds_array(c_sock, s_sock, fds); //make fds array to use in poll

  while(1){
    printf("loop!\n");
    if(poll(fds, 2, -1) == -1){
        perror("Error in poll "); exit(-4); }

    //request was received
    if(fds[0].revents == POLLIN){
      //insert in pool
      if((newsock = accept(s_sock, NULL, NULL)) < 0){
        perror("accept"); exit(-4);}
      place(&pool, newsock);
      //indicate that poll is for sure nonempty now
      pthread_cond_signal(&cond_nonempty);
    }
    else if(fds[0].revents == POLLHUP){
      //thread died
    }

    //command was received
    if(fds[1].revents == POLLIN){
      printf("command ready\n");
      if((newsock = accept(c_sock, NULL, NULL)) < 0){
        perror("accept"); exit(-4);}
      if(!command(newsock, start, pages, bytes)){
        for(int i=0; i<no_threads; i++){
          place(&pool, -1);
          pthread_cond_signal(&cond_nonempty);
        }
        break;
      }
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
