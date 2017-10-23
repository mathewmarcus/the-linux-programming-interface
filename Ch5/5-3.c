#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


void atomic_append(char *file, size_t bytes);
void append(char *file, size_t bytes);
int main(int argc, char *argv[argc])
{
  int c, atomic;
  ssize_t bytes;

  opterr = 0;
  atomic = 1;
  while ((c = getopt(argc, argv, "x")) != -1) {
    switch (c) {
      case 'x':
	atomic = 0;
	break;
      case '?':
	fprintf(stderr, "Unexpected option: %c\n", optopt);
      default:
	exit(1);
    }
  }

  if (argc - optind != 2) {
    fprintf(stderr, "usage: %s filename num-bytes [-x]\n", argv[0]);
    exit(1);
  }

  bytes = strtol(argv[optind+1], NULL, 10);
  switch (atomic) {
    case 0:
      append(argv[optind], bytes);
      break;
    default:
      atomic_append(argv[optind], bytes);
      break;
  }
  return 0;
}

void append(char *file, size_t bytes) {
  printf("non-atomic append\n");
  int fd;
  if ((fd = open(file, O_WRONLY | O_CREAT, 0644)) == -1) {
    fprintf(stderr, "Failed opening %s: %s\n", file, strerror(errno));
    exit(1);
  }

  while (bytes--) {
    if (lseek(fd, 0, SEEK_END) == -1) {
      perror("Failed to seek to file end");
      exit(1);
    }
    if (write(fd, "i", 1) != 1) {
      perror("Failed to write byte");
      exit(1);
    }
  }
}

void atomic_append(char *file, size_t bytes) {
  printf("atomic append\n");
  int fd;
  if ((fd = open(file, O_WRONLY | O_APPEND | O_CREAT, 0644)) == -1) {
    fprintf(stderr, "Failed opening %s: %s\n", file, strerror(errno));
    exit(1);
  }

  while (bytes--) {
    if (write(fd, "i", 1) != 1) {
      perror("Failed to write byte");
      exit(1);
    }
  }
}
