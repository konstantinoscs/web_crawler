#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "parent.h"
#include "searchutilities.h"
#include "worker.h"

int searchmain(int argc, char **argv, int sock, int c_sock){
  char *docfile = NULL, **job_to_w=NULL, **w_to_job = NULL;
  int num_workers = 0;
  pid_t *child = NULL;

  if(!s_parse_arguments(argc, argv, &docfile, &num_workers)){
    return -1;
  }
  child = malloc(num_workers*sizeof(pid_t));
  make_fifo_arrays(&job_to_w, &w_to_job, num_workers);
  mkdir("log", 0755);
  //start forking workers
  for(int i=0; i<num_workers; i++){
    if ((child[i] = fork()) == 0){
      //worker process
      worker_operate(job_to_w[i], w_to_job[i]);
      //free everything
      free_worker(child, docfile, num_workers, job_to_w, w_to_job);
      return 0;
    }
  }
  parent_operate(num_workers, child, docfile, job_to_w, w_to_job, sock, c_sock);
}
