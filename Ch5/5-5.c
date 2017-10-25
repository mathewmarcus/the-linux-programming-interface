#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


int main(int argc, char *argv[argc])
{
  int fd1, fd2;
  off_t offset1, offset2;
  int flags1, flags2;
  

  if (argc != 2) {
    fprintf(stderr, "usage: %s file\n", argv[0]);
    exit(1);
  }

  if ((fd1 = open(argv[1], O_RDONLY)) == -1) {
    fprintf(stderr, "Failed to open %s: %s\n", argv[1], strerror(errno));
    exit(errno);
  }

  if ((fd2 = dup(fd1)) == -1) {
    fprintf(stderr, "Failed to duplicate file descriptor %i: %s", fd1, strerror(errno));
    exit(errno);
  }

  if ((offset1 = lseek(fd1, 6, SEEK_SET)) == -1) {
    fprintf(stderr, "Failed to seek 6 bytes from file descriptor %i: %s", fd1, strerror(errno));
    exit(errno);
  }

  if ((offset2 = lseek(fd2, 0, SEEK_CUR)) == -1) {
    fprintf(stderr, "Failed to seek 6 bytes from file descriptor %i: %s", fd1, strerror(errno));
    exit(errno);
  }

  if ((flags1 = fcntl(fd1, F_GETFL)) == -1) {
    fprintf(stderr, "Failed to read file status flags from file descriptor %i: %s", fd1, strerror(errno));
    exit(errno);
  }

  if ((flags2 = fcntl(fd2, F_GETFL)) == -1) {
    fprintf(stderr, "Failed to read file status flags from file descriptor %i: %s", fd1, strerror(errno));
    exit(errno);
  }

  printf("File descriptor 1 offset: %li and file status flags: 0x%X\n", offset1, flags1);
  printf("File descriptor 2 offset: %li and file status flags: 0x%X\n", offset2, flags2);

  close(fd1);
  close(fd2);
  
  return 0;
}
