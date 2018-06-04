#ifndef UTILITIES_H
#define UTILITIES_H
#include <stdio.h>

int parse_arguments(int argc, char **argv, char **save_dir, int *no_threads,
  int *c_port, int *s_port, char **host, char **start_url);
  
//check if link contains host in front
//and return pure link
char *extract_link(char *host, char*link);
//simple util to free 2d array
void free_2darray(char **array, int size);
//checks if a directory exists and if not, creates it
int check_dir(char *mixed_path);
void parse_links(char *data, char ***links, int *linksize);
void insert_links(char **links, int linksize);
int is_ip(char *host_or_ip);
#endif
