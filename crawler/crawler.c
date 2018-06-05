#include <arpa/inet.h>
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
#include "searchmain.h"
#include "utilities.h"
#include "wget.h"

extern pthread_mutex_t mtx;
extern pthread_cond_t cond_nonempty;
extern pthread_cond_t cond_nonfull;
extern pool_t pool;
extern link_set set;
extern int stuck;
pthread_mutex_t p_mut = PTHREAD_MUTEX_INITIALIZER;
int pages_down = 0;
unsigned long bytes_down = 0;

int command(int sock, clock_t start, char *save_dir);

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

void *thread_crawl(void *info){
  ThreadInfo *t_info = (ThreadInfo *) info;
  int sock, linksize;
  char *site = NULL, **links = NULL, *data = NULL;
  
  while(1){
    site = obtain(&pool);
    printf("Got site: %s\n", site);
    if(!site) break;
    if((sock = socket(AF_INET , SOCK_STREAM , 0)) < 0){
    perror("Socket"); exit(-2);}
    if(connect(sock, t_info->serverptr, t_info->s_size) < 0){
      perror("Connect to server: "); exit(-2);}

    if(wget(sock, site, t_info->save_dir, t_info->host, t_info->s_port, &data)){
      //update stats
      pthread_mutex_lock(&p_mut);
        pages_down++;
        bytes_down += strlen(data);
      pthread_mutex_unlock(&p_mut);
      parse_links(data, &links, &linksize);
      printf("parsed links %d\n", linksize);
      //insert new links at pool
      insert_links(links, linksize);
      free_2darray(links, linksize);
    }
    free(site);
    if(data) free(data);
    close(sock);
  }
  if(site) free(site);
}

int crawler_operate(char *host, char *save_dir, char *start_url, int c_port,
  int s_port, int no_threads){

  int c_sock, s_sock, newsock;
  char *start_link = NULL;
  pthread_t *threads = NULL;
  clock_t start = clock();
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
  memcpy(&server.sin_addr, rem->h_addr_list[0], rem->h_length);
  server.sin_port = htons(s_port);
  t_info.s_size = sizeof(server);
  t_info.serverptr = serverptr;
  t_info.save_dir = save_dir;
  t_info.host = host;
  t_info.s_port = s_port;

  if((threads = malloc(no_threads*sizeof(pthread_t))) == NULL){
    perror("threads malloc:"); exit(-2); }
  initialize(&pool);
  initialize_set(&set);
  stuck = 0;
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
  //place first link at pool
  start_link = extract_link(host, start_url);
  printf("start link:%s\n", start_link);
  insert_links(&start_link, 1);
  //place(&pool, start_link);
  //pthread_cond_signal(&cond_nonempty);
  printf("Just waiting!\n");
  //start crawling

  printf("command ready\n");
  while(1){
    if((newsock = accept(c_sock, NULL, NULL)) < 0){
      perror("accept"); exit(-4);}
    //execute command
    if(!command(newsock, start, save_dir)){
    //if command was SHUTDOWN, send message to threads
      for(int i=0; i<no_threads; i++){
        place(&pool, NULL);
        pthread_cond_signal(&cond_nonempty);
      }
      break;
    }
    close(newsock);
  }

  for(int i=0; i<no_threads; i++)
    if(pthread_join(threads[i], NULL)){
      perror("pthread_join"); exit(1);}
  delete_set(&set);
  free(pool.data);
  free(threads);
}

int command(int sock, clock_t start, char *save_dir){
  static char cmd[9];
  int i=0;
  while((read(sock, cmd+i, 1) > 0) && i<8 && cmd[i]!='\n') i++;
  cmd[i] = '\0';
  printf("Command: %s\n", cmd);
  if(!strcmp(cmd, "STATS")){
    clock_t end = clock();
    float uptime = (float) (end - start) / CLOCKS_PER_SEC;
    printf("Crawler up for %lf, downloaded %d pages, %ld bytes\n", uptime, pages_down, bytes_down);
  }
  else if(!strcmp(cmd, "SHUTDOWN")){
    return 0;
  }
  else if(!strcmp(cmd, "SEARCH")){
    write_dirs("./doc", save_dir);
    char *argv[] = { "./JobExecutor", "-d", "./doc", "-w", "4"};
    int arc = 5;
    searchmain(arc, argv, sock);
  }
  else{
    printf("Unknown command!\n");
  }
  return 1;
}