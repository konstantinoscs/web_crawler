#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utilities.h"

//send_get sends a get request to sock for "page"
void send_get(int sock, char *page, char *host, int s_port);
//parse response of the get request
char *parse_response(int sock);
//parse header reads the header of the response
int parse_header(int fd, char*** paths, int *pathsize);
//check_request checks if request is ok and saves the content length
int check_request(char **header, int headsize, int *len);
//save to dir
void save_to_dir(char *save_dir, char *page, char* data);

//wget downloads a page and returns it as a raw string
int wget(int sock, char *page, char *save_dir, char *host, int s_port, char **down){

  char *data = NULL;
  //get request here
  send_get(sock, page, host, s_port);
  //save page text to pointer
  printf("Will try to get data\n");
  if(!(data = parse_response(sock))){
    fprintf(stderr, "Couldn't download file :(...\n");
    if(down) *down = NULL;
    return 0;
  }
  //printf("Got data:\n %s\n", data);
  //save data to actual file
  save_to_dir(save_dir, page, data);
  if(down) 
    *down = data;
  else 
    free(data);
  return 1;
}

void send_get(int sock, char *page, char *host, int s_port){
  static char port[6] = "00000";
  sprintf(port, "%d", s_port);
  write(sock, "GET " ,4);
  write(sock, page, strlen(page));
  write(sock, " HTTP/1.1\r\nHost: ", 17);
  write(sock, host, strlen(host));
  write(sock, ":", 1);
  write(sock, port, strlen(port));
  write(sock, "\r\nConnection: close\r\n\r\n", 23);
}

char *parse_response(int sock){
  char *data = NULL;
  char **header;
  int headsize = 0;
  int offset=0, nread = 0, len = 0;
  int chunk = 120, left = 0;
  char ch;
  if(!parse_header(sock, &header, &headsize) || !check_request(header, headsize, &len)){
    //free_2darray(header, headsize);
    return NULL;
  }
  printf("Len is %d\n", len);
  left = len;
  data = malloc(len+1);
  while(left){
    if(left <= chunk)
      chunk = left;
    nread = read(sock, data+offset, chunk);
    left -= nread;
    offset += nread;
  }
  data[len] = '\0';
  free_2darray(header, headsize);
  return data;
}

int parse_header(int fd, char*** paths, int *pathsize){
  int docm = 8, docc=0; //arbitary start from 8 words
  int wordm = 2, wordc=0; //start from a word with 2 characters
  char ch;
  //FILE * docf = fopen(doc, "r"); -->legacy from file parsing
  *pathsize = 0;

  *paths = malloc(docm*sizeof(char*));

  while(1){
    if(read(fd, &ch, 1) <= 0) //not a good request
      return 0;

    if(ch==13){    //request ended ok check for CR
      if(read(fd, &ch, 1) <= 0) //not a good request
        return 0;
      if(ch=='\n')
      break;
    }

    if(docc == docm){     //allocate space for more paths
      docm *= 2;
      *paths = realloc(*paths, docm*sizeof(char*));
    }
    (*paths)[docc] = malloc(wordm); //allocate memory for the word
    (*paths)[docc][wordc++] = ch; //store first character of line

    if(read(fd, &ch, 1) <= 0) //not a good request
      return 0;

    while(ch != '\n'){
      if(wordc+1 == wordm){  //realloc condition -- save space for '\0'
        wordm *= 2;
        (*paths)[docc] = realloc((*paths)[docc], wordm);
      }
      (*paths)[docc][wordc++] = ch; //save character in paths

      if(read(fd, &ch, 1) <= 0) //not a good request
        return 0;
    } //document is saved exactly as read --including whitespace

    (*paths)[docc][wordc] = '\0';
    (*paths)[docc] = realloc((*paths)[docc], wordc+1); //shrink to fit
    *pathsize = ++docc;    //necessary update in case of emergency
    wordm = 2;  //re-initialize for next document
    wordc = 0;
  }
  *paths = realloc(*paths, (*pathsize)*sizeof(char*)); //shrink to fit
  printf("Bb parse header\n");
  return 1;
}

int check_request(char **header, int headsize, int *len){
  printf("Trying first check\n");
  printf("%s\n%ld\n", header[0], strlen(header[0]));
  printf("%d\n", strncmp(header[0], "HTTP/1.1 200 OK", 15));
  if(strlen(header[0]) < 15 || strncmp(header[0], "HTTP/1.1 200 OK", 15))
    return 0;
  printf("First check ok\n");
  for(int i=0; i<headsize; i++){
    //check for Content-Length:
    if(strlen(header[i]) >16 && !strncmp(header[i], "Content-Length: ", 16)){
      *len = atoi(header[i]+16);
      printf("Second check ok\n");
      break; 
    }
  }
  return 1;
}

void save_to_dir(char *save_dir, char *page, char* data){
  char *path = NULL;
  FILE *fp;
  //check if save dir is "folder" or "folder/"
  printf("%c\n", save_dir[strlen(save_dir)-1]);
  if(save_dir[strlen(save_dir)-1] == '/'){
    path = malloc(strlen(save_dir)+ strlen(page));
    sprintf(path, "%s%s", save_dir, page+1);
  }
  else{
    path = malloc(strlen(save_dir)+ strlen(page)+1);
    sprintf(path, "%s%s", save_dir, page);
  }
  printf("path is: %s\n", path);
  check_dir(path);
  fp = fopen(path, "w");
  fprintf(fp, "%s", data);
  fclose(fp);
  free(path);
}
