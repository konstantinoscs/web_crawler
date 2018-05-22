#include <stdlib.h>

#include "server.h"
#include "utilities.h"

int main(int argc, char **argv){
  //command line arguments
  int no_threads=0, c_port=0, s_port=0;
  char *root_dir=NULL;
  parse_arguments(argc, argv, &root_dir, &no_threads, &c_port, &s_port);
  server_operate(no_threads, c_port, s_port);
  free(root_dir);
}
