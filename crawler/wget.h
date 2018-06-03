#ifndef WGET_H
#define WGET_H

#include "crawler.h"

int wget(int sock, char *page, char *root_dir, char *host, int s_port,
  char ***links, int *linksize);

#endif
