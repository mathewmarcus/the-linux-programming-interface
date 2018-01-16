#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"




/* int main(int argc, char *argv[argc]) */
/* { */
/*   Process *tree = newProcess(0, "0"); */
/*   Process *a = newProcess(1, "a"); */
/*   Process *b = newProcess(2, "b"); */
/*   Process *c = newProcess(3, "c"); */
/*   Process *d = newProcess(4, "d"); */
/*   Process *e = newProcess(5, "e"); */
/*   printf("Tree: %p\n", tree); */
/*   add2Tree(tree, a, 0); */
/*   add2Tree(tree, b, 0); */
/*   add2Tree(tree, c, 1); */
/*   add2Tree(tree, d, 1); */
/*   add2Tree(tree, e, 2); */
/*   printf("Tree: %p\n", tree); */
/*   delProcess(tree); */

/*   FILE *proc_status = fopen("/proc/1/status", "r"); */
/*   printf("VALUE: %s", getProcValue("PPid", proc_status)); */
/*   return 0; */
/* } */

int main(int argc, char *argv[argc])
{
  buildTree();
  return 0;
}

