#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[argc])
{
  int fd = open("/etc/passwd", O_RDONLY);
  char buf[12];

  read(fd, buf, 12);
  write(STDOUT_FILENO, buf, -12);
  
  return 0;
}

