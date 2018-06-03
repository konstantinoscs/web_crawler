#ifndef UTILITIES_H
#define UTILITIES_H
#include <stdio.h>

int parse_arguments(int argc, char **argv, char **save_dir, int *no_threads,
  int *c_port, int *s_port, char **host, char **start_url);
char *extract_link(char *host, char*link);
void free_2darray(char **array, int size);
#endif
