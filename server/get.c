#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "utilities.h"

int verify_get(char **request, int reqsize);
int parse_get(int fd, char ***paths, int *pathsize);
void get_file_path(char *get_header, char *root_dir, char **path);
int serve_request(int fd, char *file);
int response_200_ok(int fd, FILE *fp);
int response_403_forbidden(int fd);
int response_404_not_found(int fd);
int response_500_internal_server_error(int fd);
void write_response(int fd, char *fline, char *message);


int get(int fd, char *root_dir){
  char **request = NULL;
  int reqsize=0, pathsize=0;
	char *path = NULL;
  int served = 0;

  if(!parse_get(fd, &request, &reqsize) || !verify_get(request, reqsize)){
    fprintf(stderr,"Bad request!\n");
    return 0;
  }
  for(int i=0; i<reqsize; i++)
    printf("%s\n", request[i]);
  printf("END\n");
	//get path of file
	get_file_path(request[0], root_dir, &path);
  //printf("GOT PATH %s\n", path);
	served = serve_request(fd, path);
  for(int i=0; i<reqsize; i++)
    free(request[i]);
  free(request);
  free(path);
  printf("bye request\n");
  return served;  //return no of bytes if a page has been served (stats)
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
void get_file_path(char *get_header, char *root_dir, char **path){
	//point to start of the path (after "GET ")
	char *t_path = get_header+4;
	int sz = 0, len;
	while(t_path[sz]!=' ') sz++;
  len = strlen(root_dir) +sz;
  *path = malloc(len+1);
  strncpy(*path, root_dir, strlen(root_dir));
  strncpy((*path)+strlen(root_dir), t_path, sz);
  (*path)[len] = '\0';
	//*path = t_path;
}

int serve_request(int fd, char *file){
  FILE *fp = fopen(file, "r");
  int serve = 0;
  if(fp){
    //all ok, proceed with 200
    serve = response_200_ok(fd, fp);
  }
  else if(errno == EACCES){
    //server doesn't have permission, send 403
    response_403_forbidden(fd);
  }
  else if(errno == ENOENT){
    //file doesn't exist, send 404
    printf("404\n");
    response_404_not_found(fd);
  }
  else{
    //not 403 or 404 so send 500
    response_500_internal_server_error(fd);
  }
  fclose(fp);
  //return if a page has been served to update stats;
  return serve;
}

int response_200_ok(int fd, FILE *fp){
  char *message = NULL;
  static char *fline = "HTTP/1.1 200 OK\r\n";
  read_file(&message, fp);  //load file to message variable
  write_response(fd, fline, message);
  int bytes = strlen(message);
  if(message)
    free(message);
  //return how many bytes were served for statistics
  return bytes;
}

int response_403_forbidden(int fd){
  static char *message = "<html>Trying to access this file but I don't thin I can make it.</html>";
  static char *fline = "HTTP/1.1 403 Forbidden\r\n";
  write_response(fd, fline, message);
}

int response_404_not_found(int fd){
  static char *message = "<html>Sorry dude, couldn't find this file.</html>";
  static char *fline = "HTTP/1.1 404 Not Found\r\n";
  write_response(fd, fline, message);
}

int response_500_internal_server_error(int fd){
  static char *message = "<html>Something went terribly wrong</html>";
  static char *fline = "HTTP/1.1 500 Internal Server Error\r\n";
  write_response(fd, fline, message);
}

void write_response(int fd, char *fline, char *message){
  static char day[4];
  static char month[4];
  static char stamp[100];
  static char *lines[] = {"Date: ",
    "Server: myhttpd/1.0.0\r\n",
    "Content-Length: ",
    "Content-Type: text/html\r\n",
    "Connection: Closed\r\n"};
  static char *newline = "\r\n";
  static time_t rtime;
  static struct tm *tinfo;
  time(&rtime);
	tinfo = localtime(&rtime);

  //HTTP1.1 <code> <message>
	if(write(fd, fline, strlen(fline)) == -1){
		perror("Response failed "); exit(-2); }

  //Date:
	if(write(fd, lines[0], strlen(lines[0])) == -1){
		perror("Response failed "); exit(-2); }
	//convert int of day to string
	map_day(day, tinfo->tm_wday);
	map_month(month, tinfo->tm_mon);
	//put in stamp the formated timestamp
	sprintf(stamp, "%s, %d %s %d %d:%d:%d GMT\r\n", day, tinfo->tm_mday, month, tinfo->tm_year+1900, tinfo->tm_hour, tinfo->tm_min, tinfo->tm_sec);
	if(write(fd, stamp, strlen(stamp)) == -1){
		perror("Response failed "); exit(-2); }

  //Server: myhttpd/1.0.0
	if(write(fd, lines[1], strlen(lines[1])) == -1){
		perror("Response failed "); exit(-2); }

  //"Content-Length: <strlen(message)>"
  if(write(fd, lines[2], strlen(lines[2])) == -1){
		perror("Response failed "); exit(-2); }
  sprintf(stamp, "%lu\r\n", strlen(message));
  if(write(fd, stamp, strlen(stamp)) == -1){
		perror("Response failed "); exit(-2); }

  //Content-Type: text/html
	if(write(fd, lines[3], strlen(lines[3])) == -1){
		perror("Response failed "); exit(-2); }
  //Connection: Closed
	if(write(fd, lines[4], strlen(lines[4])) == -1){
		perror("Response failed "); exit(-2); }
  if(write(fd, newline, strlen(newline)) == -1){
  	perror("Response failed "); exit(-2); }
  //write the message and we're done!
  if(write(fd, message, strlen(message)) == -1){
  	perror("Response failed "); exit(-2); }
}
