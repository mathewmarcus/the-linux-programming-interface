#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define BUFFSIZE 4096

int main(int argc, char *argv[argc])
{
  int output_fd;
  ssize_t bytesRead, bytesWritten;
  char* buffer = malloc(sizeof(char) * BUFFSIZE);
  
  if (argc != 2) {
    fprintf(stderr, "usage: %s output_file\n", argv[0]);
    exit(1);
  }

  if ((output_fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 00644)) == -1) {
    perror("Unable to open file");
    exit(1);
  }

  while((bytesRead = read(STDIN_FILENO, buffer, BUFFSIZE)) > 0) {
    if ((bytesWritten = write(STDOUT_FILENO, buffer, bytesRead)) != bytesRead) {
      fprintf(stderr, "Only able to write %li of %li bytes to fd %i: %s\n", bytesWritten, bytesRead, STDOUT_FILENO, strerror(errno));
      exit(1);
    }

    if ((bytesWritten = write(output_fd, buffer, bytesRead)) != bytesRead) {
      fprintf(stderr, "Only able to write %li of %li bytes to fd %i: %s\n", bytesWritten, bytesRead, output_fd, strerror(errno));
      exit(1);      
    }

  }
  if (bytesRead == -1) {
    perror("Unable to read file");
    exit(1);
  }
  
  close(output_fd);
  return 0;
}
