#include <stdio.h>
#include <stdlib.h>

#include "tree.h"

int main(int argc, char *argv[argc])
{
  if (argc != 2) {
    fprintf(stderr, USAGE, argv[0]);
    exit(1);
  }

  printUserProcs(userName2UID(argv[1]));
}
