#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"


int main(int argc, char *argv[argc])
{
  Process *tree = buildTree();
  displayTree(tree->child, 0);
  delProcess(tree);
  return 0;
}

