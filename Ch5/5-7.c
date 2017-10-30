#include <unistd.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

ssize_t writev(int fd , const struct iovec *iov , int iovcnt) {
  char *buffer;
  int len, j;
  ssize_t bytes;

  for (int i = len = 0; i < iovcnt; ++i)
    len += iov[i].iov_len;

  buffer = malloc(sizeof(char) * len);
  for (int i = j = 0; i < iovcnt; ++i)
    for (size_t k = 0; k < iov[i].iov_len; ++k)
      buffer[j++] = ((char*) iov[i].iov_base)[k];

  if ((bytes = write(fd, buffer, len)) == -1) {
    perror("Failed to write bytes");
    free(buffer);
    return errno;
  }

  free(buffer);
  return bytes;
}

ssize_t readv(int fd , const struct iovec *iov , int iovcnt) {
  char *buffer;
  int len, j;
  ssize_t bytes;

  for (int i = len = 0; i < iovcnt; ++i)
    len += iov[i].iov_len;

  buffer = malloc(sizeof(char) * len);
  if ((bytes = read(fd, buffer, len)) == -1) {
    perror("Failed to read bytes");
    free(buffer);
    return errno;
  }

  for (int i = j = 0; i < iovcnt; ++i)
    for (size_t k = 0; k < iov[i].iov_len; ++k)
      ((char*) iov[i].iov_base)[k] = buffer[j++];

  free(buffer);
  return bytes;
}
