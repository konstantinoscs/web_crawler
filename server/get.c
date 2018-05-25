#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int verify_get(char **request, int reqsize);
int parse_get(int fd, char*** paths, int *pathsize);
void get_file_path(char *get_header, char **path, int *size);
int serve_request();


int get(int fd){
  char **request = NULL;
  int reqsize=0, pathsize=0;
	char *path = NULL;

  if(!parse_get(fd, &request, &reqsize) || !verify_get(request, reqsize))
    fprintf(stderr,"Bad request!\n");
  for(int i=0; i<reqsize; i++)
    printf("%s\n", request[i]);
  printf("END\n");
	//get path of file
	get_file_path(request[0], &path, &pathsize);
	serve_request();
  for(int i=0; i<reqsize; i++)
    free(request[i]);
  free(request);
}

int parse_get(int fd, char*** paths, int *pathsize){
  int docm = 8, docc=0; //arbitary start from 8 words
  int wordm = 2, wordc=0; //start from a word with 2 characters
  char ch;
  //FILE * docf = fopen(doc, "r");
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
  //*pathsize = id+1;
  //fclose(docf);
  return 1;
}

int verify_get(char **request, int reqsize){
  int hostok=0;
  //check for GET header
  if(strncmp(request[0], "GET", 3))
    return 0;
  for(int i=0; i<reqsize; i++)
    if(!strncmp(request[i], "Host: ", 6)){
      hostok = 1;
      break;
    }
  return hostok;
}

//returns the file that we want to serve
void get_file_path(char *get_header, char **path, int *size){
	//point to start of the path (after "GET ")
	char *t_path = get_header+4;
	int sz = 0;
	while(t_path[sz++]!=' ') continue;
	*size = sz;
	*path = t_path;
}

int serve_request(int fd, char *file){
  FILE *fp = fopen(file, "r");
  if(fp){
    //all ok, proceed with 200
    //response_200_ok()
  }
  else if(errno == EACCES){
    //server doesn't have permission, send 403
    //response_403_forbidden();
  }
  else if(errno == ENOENT){
    //file doesn't exist, send 404
    //response_404_not_found();
  }
  else{
    //not 403 or 404 so send 500
    //response_500_internal_server_error()
  }
}

int response_200_ok(int fd){
	time_t rtime;
  struct tm *tinfo;
  time(&rtime);
	tinfo = localtime(&rtime);
}

int response_403_forbidden(int fd){
  static char *message = "<html>Trying to access this file but I don't thin I can make it.</html>";
  static char *lines[] = {"HTTP/1.1 403 Forbidden",
    "Date: ",
    "Server: myhttpd/1.0.0",
    "Content-Length: ",
    "Content-Type: text/html",
    "Connection: Closed"};
  time_t rtime;
  struct tm *tinfo;
  time(&rtime);
	tinfo = localtime(&rtime);
}

int response_404_not_found(int fd){
  static char *message = "<html>Sorry dude, couldn't find this file.</html>";
  static char *lines[] = {"HTTP/1.1 404 Not Found",
    "Date: ",
    "Server: myhttpd/1.0.0",
    "Content-Length: ",
    "Content-Type: text/html",
    "Connection: Closed"};
  time_t rtime;
  struct tm *tinfo;
  time(&rtime);
	tinfo = localtime(&rtime);
	if(write(fd, lines[0], strlen(lines[0]))==-1){
		perror("Response failed "); exit(-2); }
	if(write(fd, lines[1], strlen(lines[1]))==-1){
		perror("Response failed "); exit(-2); }

}

int response_500_internal_server_error(int fd){
	time_t rtime;
  struct tm *tinfo;
  time(&rtime);
	tinfo = localtime(&rtime);
}
