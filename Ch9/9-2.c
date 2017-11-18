#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fsuid.h>

/* 
   Technically, a process with the user ids real=0 effective=1000 saved=1000 file-system=1000 is not privileged,
   because the effective user id is not equal to 0. However, because the real user id is equal to 0, the process
   could easily regain privileges through a call such as setuid(0), seteuid(0), setreuid(-1, 0), setresuid(-1, 0, 1000).
*/
int main(int argc, char *argv[argc])
{
  uid_t *real_id = malloc(sizeof(uid_t));
  uid_t *effective_id = malloc(sizeof(uid_t));
  uid_t *saved_user_id = malloc(sizeof(uid_t));
  int choice;

  /* real=0 effective=1000 saved=1000 file-system=1000 */
  getresuid(real_id, effective_id, saved_user_id);
  printf("Initial: real=%i effective=%i saved=%i\n\n", *real_id, *effective_id, *saved_user_id);

  printf("a) setuid(0)\n");
  printf("b) seteuid(0)\n");
  printf("c) setreuid(-1, 0)\n");
  printf("d) setresuid(-1, 0, 1000)\n");

  write(STDOUT_FILENO, "Select permission operation: ", 29);
  
  read(STDIN_FILENO, &choice, 1);
  write(STDOUT_FILENO, "\n", 1);

  switch (choice) {
  case 'a':
    /* real=0 effective=0 saved=1000 file-system=0 */
    setuid(0);
    getresuid(real_id, effective_id, saved_user_id);
    printf("setuid(0): real=%i effective=%i saved=%i\n\n", *real_id, *effective_id, *saved_user_id);
    break;
  case 'b':
    /* real=0 effective=0 saved=1000 file-system=0 */
    seteuid(0);
    getresuid(real_id, effective_id, saved_user_id);
    printf("seuid(0): real=%i effective=%i saved=%i\n\n", *real_id, *effective_id, *saved_user_id);
    break;
  case 'c':
    /* real=0 effective=0 saved=1000 file-system=0 */
    setreuid(-1, 0);
    getresuid(real_id, effective_id, saved_user_id);
    printf("setreuid(-1, 0): real=%i effective=%i saved=%i\n\n", *real_id, *effective_id, *saved_user_id);
    break;
  case 'd':
    /* real=0 effective=0 saved=1000 file-system=0 */
    setresuid(-1, 0, -1);
    getresuid(real_id, effective_id, saved_user_id);
    printf("setresuid(-1, 0, -1): real=%i effective=%i saved=%i\n\n", *real_id, *effective_id, *saved_user_id);
    break;
  default:
    fprintf(stderr, "Must select one of: a b c d\n");
    exit(1);
  }

  free(real_id);
  free(effective_id);
  free(saved_user_id);
  return 0;
}
