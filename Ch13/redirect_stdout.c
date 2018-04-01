#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[argc])
{
  int saved_stdout, output_fileno;

  output_fileno = open("redirect_stdout.txt",
		       O_WRONLY | O_CREAT | O_APPEND,
		       0644);
  if (output_fileno == -1) {
    perror("Failed to open redirect_stdout.txt");
  }

  /* setvbuf(stdout, NULL, _IONBF, BUFSIZ); */
  
  saved_stdout = dup(STDOUT_FILENO);
  dup2(output_fileno, STDOUT_FILENO);
  printf("%s\n", "This should be in a file");
  fflush(stdout);
  /* write(STDOUT_FILENO, "This should be in a file\n", 25); */

  dup2(saved_stdout, STDOUT_FILENO);

  close(saved_stdout);
  close(output_fileno);

  printf("%s\n", "This should be in a terminal window");
  /* write(STDOUT_FILENO, "This should be in a terminal window\n", 36); */
  return 0;
}
