#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fsuid.h>


int main(int argc, char *argv[argc])
{
  uid_t *real_id = malloc(sizeof(uid_t));
  uid_t *effective_id = malloc(sizeof(uid_t));
  uid_t *saved_user_id = malloc(sizeof(uid_t));
  int choice;

  /* real=1000 effective=0 saved=0 file-system=0 */
  getresuid(real_id, effective_id, saved_user_id);
  printf("Initial: real=%i effective=%i saved=%i\n\n", *real_id, *effective_id, *saved_user_id);

  printf("a) setuid(2000)\n");
  printf("b) setreuid(-1, 2000)\n");
  printf("c) seteuid(2000)\n");
  printf("d) seteuid(2000)\n");
  printf("e) setresuid(–1, 2000, 3000)\n");

  write(STDOUT_FILENO, "Select permission operation: ", 29);
  
  read(STDIN_FILENO, &choice, 1);
  write(STDOUT_FILENO, "\n", 1);

  switch (choice) {
  case 'a':
    /* real=1000 effective=2000 saved=2000 file-system=2000 */
    setuid(2000);
    getresuid(real_id, effective_id, saved_user_id);
    printf("setuid(2000): real=%i effective=%i saved=%i\n\n", *real_id, *effective_id, *saved_user_id);
    break;
  case 'b':
    /* real=1000 effective=2000 saved=2000 file-systsem=2000 */
    setreuid(-1, 2000);
    getresuid(real_id, effective_id, saved_user_id);
    printf("setuid(-1, 2000): real=%i effective=%i saved=%i\n\n", *real_id, *effective_id, *saved_user_id);
    break;
  case 'c':
    /* real=1000 effective=2000 saved=0 file-system=2000 */
    seteuid(2000);
    getresuid(real_id, effective_id, saved_user_id);
    printf("seteuid(2000): real=%i effective=%i saved=%i\n\n", *real_id, *effective_id, *saved_user_id);
    break;
  case 'd':
    /* real=1000 effective=0 saved=0 file-system=2000 */
    setfsuid(2000);
    getresuid(real_id, effective_id, saved_user_id);
    printf("setfsuid(2000): real=%i effective=%i saved=%i\n\n", *real_id, *effective_id, *saved_user_id);
    break;
  case 'e':
    /* real=1000 effective=2000 saved=3000 file-system=2000*/
    setresuid(-1, 2000, 3000);
    getresuid(real_id, effective_id, saved_user_id);
    printf("setresuid(-1, 2000, 3000): real=%i effective=%i saved=%i\n\n", *real_id, *effective_id, *saved_user_id);
    break;
  default:
    fprintf(stderr, "Must select one of: a b c d e\n");
    exit(1);
  }

  free(real_id);
  free(effective_id);
  free(saved_user_id);
  return 0;
}
