#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>


/*
  Output:
    Normal execution
    Jumped from function a
    Normal execution
    Segmentation fault (core dumped)
 */

static jmp_buf env;
void a();
int main(int argc, char *argv[argc])
{
  a();
  longjmp(env, 'a');  
  return 0;
}

void a() {
  switch (setjmp(env)) {
  case 0:
    printf("Normal execution\n");
    break;
  case 'a':
    printf("Jumped from function a\n");
    break;
  }

}
