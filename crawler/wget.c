#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

//wget downloads a page
int wget(int sock, char *page, char *root_dir, char *host, int s_port,
  char ***links, int *linksize){

  char *data = NULL;
  //get request here
  send_get(sock, page, host, s_port);
}

void send_get(int sock, char *page, char *host, int s_port){
  write(sock, "GET " ,4);
  write(sock, page, strlen(page));
  write(sock, "HTTP/1.1\r\nHost: ", 16);
  write(sock, host, strlen(host));
  write(sock, "\r\nConnection: close\r\n\r\n", 23);
}

char *parse_response(int sock){
  char *data = NULL;
  char **header;
  int headsize = 0;
  int chunk = 120, nread = 0, len = 0;
  static char buf[120], ch;
  memset(buf, 0, sizeof(buf));
  if(!parse_header(sock, &header, &headsize) || !check_request(header, headsize, &len)){
    free_2darray(header, headsize);
    return NULL;
  }

    
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
  return 1;
}

int check_request(char **header, int headsize, int *len){
  if(strlen(header[0]) < 16 || strncmp(header[0], "HTTP/1.1 200 OK", 16))
    return 0;
  
  for(int i=0; i<headsize; i++){
    //check for Content-Length:
    if(strlen(header[i]) >16 && !strncmp(header[i], "Content-Length: ", 16)){
      *len = atoi(header[i]+16);
       break;
    }
  }
  return 1;
}