#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


/*
  This will no prepend data to the file beacuse when a file is opened
  with the O_APPEND flag, the `write` sytsem call is replaced with an
  atomic operation that seeks to the end of the file and then writes
  the data
 */
int main(int argc, char *argv[argc])
{
  int fd;

  if (argc != 2) {
    fprintf(stderr, "usage: %s filename\n", argv[0]);
    exit(1);
  }

  if ((fd = open(argv[1], O_WRONLY | O_APPEND)) == -1) {
    perror(argv[1]);
    exit(1);
  }

  if (lseek(fd, 0, SEEK_SET) == -1) {
    perror("Failed to seek to beginning of file");
    exit(1);
  }

  if (write(fd, "Goodbye world\n", 14) != 14) {
    perror("Failed to write all bytes to file");
    exit(1);
  }
  return 0;
}
