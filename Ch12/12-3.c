#include <stdio.h>
#include <stdlib.h>

#include "tree.h"

#define USAGE "Usage:  %s filename\n"

int main(int argc, char *argv[argc])
{
  if (argc != 2) {
    fprintf(stderr, USAGE, argv[0]);
    exit(1);
  }

  printPathProcs(argv[1]);
}
