#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread_h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

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

int server_operate(int no_threads, int c_port, int s_port){
  int c_sock, s_sock;
  pthread_t *threads = NULL;
  //create threads
  if((threads = malloc(no_threads*sizeof(pthread_t))) == NULL){
    perror("threads malloc:"); exit(-2); }
  for(int i=0; i<no_threads; i++){
    if(pthread_create(threads+i, NULL, sleeping, (void *)sl)){
      perror("thread create:"); exit(-3); }
  }
  //set the two sockets
  set_socket(c_port, &c_sock);
  set_socket(c_port, &s_sock);


  free(threads);
}
