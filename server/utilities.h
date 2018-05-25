#ifndef UTILITIES_H
#define UTILITIES_H

int parse_arguments(int argc, char ** argv, char** root_dir, int *no_threads,
  int *c_port, int *s_port);
int map_day(char*days, int dayd);
int map_month(char*months, int monthd);
#endif
