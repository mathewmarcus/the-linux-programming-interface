#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define BLOCKSIZE 4096

void usage(char*);
void sparse_cp(int, int);
void cp(int, int);

int main(int argc, char *argv[argc])
{
  int arg, sparse, src, dst;
  
  sparse = 0;

  opterr = 0; // get cli arg errors as arg == 'c'
  while ((arg = getopt(argc, argv, "s")) != -1) {
    switch (arg) {
    case 's':
      sparse = 1;
      break;
    case '?':
      fprintf(stderr, "Encountered unexpected opt: %c\n", optopt);
      usage(argv[0]);
      break;
    default:
      usage(argv[0]);
      break;
    }
    
  }

  if (argc - optind != 2)
    usage(argv[0]);

  if ((src = open(argv[optind], O_RDONLY)) == -1) {
    fprintf(stderr, "Failed to open file %s: %s\n", argv[optind], strerror(errno));
    exit(1);
  }

  if ((dst = open(argv[++optind], O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
    fprintf(stderr, "Failed to open file %s: %s\n", argv[optind], strerror(errno));
    exit(1);
  }

  sparse ? sparse_cp(src, dst) : cp(src, dst);

  close(src);
  close(dst);
  
  return 0;
}

void usage(char *programName) {
  fprintf(stderr, "usage: %s [-s] src dest\n", programName);
  exit(1);
}

void cp(int src, int dst) {
  ssize_t bytes;
  char *buffer;

  if (!(buffer = malloc(sizeof(char) * BLOCKSIZE))) {
    fprintf(stderr, "Failed to allocate memory for buffer");
    exit(1);
  }
  
  while ((bytes = read(src, buffer, BLOCKSIZE)) > 0) {
    if (write(dst, buffer, bytes) != bytes) {
      perror("Failed to write all bytes");
      exit(1);
    }
  }

  if (bytes == -1) {
    perror("File read error");
    exit(1);
  }
}

void sparse_cp(int src, int dst) {
  ssize_t byte;
  char *buffer;
  int nullBytes = 0;

  if (!(buffer = malloc(sizeof(char)))) {
    fprintf(stderr, "Failed to allocate memory for buffer");
    exit(1);
  }
  
  while ((byte = read(src, buffer, 1)) > 0) {
    if (*buffer == '\0') {
      nullBytes++;
      continue;
    }
    else if (nullBytes) {
      if (lseek(dst, nullBytes, SEEK_CUR) == -1) {
    	perror("Failed to create file hole");
    	exit(1);
      }
      nullBytes = 0;
    }
    if (write(dst, buffer, byte) != byte) {
      perror("Failed to write byte");
      exit(1);
    }
  }

  if (byte == -1) {
    perror("File read error");
    exit(1);
  }

}
