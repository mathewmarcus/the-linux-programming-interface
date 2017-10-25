#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int my_dup(int old_fd) {
  return fcntl(old_fd, F_DUPFD, 0);
}

int my_dup2(int old_fd, int new_fd) {
  if (old_fd == new_fd) { /* old file descriptor is the same as new file descriptor, return this value */
    fprintf(stderr, "File descriptors are identical\n");
    return new_fd;
  }
  else if (fcntl(new_fd, F_GETFD) != -1) { /* new file descriptor already exists, close it */
    if (close(new_fd) == -1) {
      perror("Failed to close already open new file descriptor");
      return -1;
    }
  }

  return fcntl(old_fd, F_DUPFD, new_fd);
}
