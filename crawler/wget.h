#ifndef WGET_H
#define WGET_H

#include "crawler.h"

int wget(int sock, char *page, char ***links, int *linksize);

#endif
