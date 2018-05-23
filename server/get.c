#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int verify_get(char **request, int reqsize);
int parse_get(int fd, char*** paths, int *pathsize);
//parsedocfile reads all the paths in the file specified by doc

int get(int fd){
  char **request;
  int reqsize;
  if(!parse_get(fd, &request, &reqsize) || !verify_get(request, reqsize))
    fprintf(stderr,"Bad request!\n");
  for(int i=0; i<reqsize; i++)
    printf("%s\n", request[i]);
  printf("END\n");
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

int response_200_ok(){

}

int response_404_not_found(){

}

int response_403_forbidden(){

}

