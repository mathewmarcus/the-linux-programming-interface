#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>


#define USAGE "usage: %s [-n lines] [FILE]\n"
struct node {
  char *line;
  struct node *next;
};

struct tail_info {
  char *filename;
  unsigned int num_lines;
};

struct tail_info *parse_args(int argc, char *argv[]) {
  int c;
  static struct tail_info t;
  t.num_lines = 10;

  while ((c = getopt(argc, argv, "n:")) != -1) {
    switch (c) {
    case 'n':
      t.num_lines = atoi(optarg);
      if (!t.num_lines)
	exit(0);
      break;
    }
  }

  if (argc > optind)
    t.filename = argv[optind];

  return &t;
}

struct node *init_circ_buff(unsigned int num_nodes) {
  unsigned int last_index;
  last_index = num_nodes - 1;
  struct node *lines = malloc(num_nodes * sizeof(struct node));
  
  (lines+last_index)->next = lines;

  struct node *curr, *prev;
  prev = lines;
  curr = lines+1;
  for (unsigned int i = 0; i < last_index; i++) {
    prev->line = malloc(1000 * sizeof(char));
    *(prev->line) = '\0';
    prev->next = curr;
    prev++;
    curr++;
  }
  lines[last_index].line =malloc(1000 * sizeof(char));
  lines[last_index].line[0] = '\0';

  return lines;
}

int main(int argc, char *argv[argc])
{
  struct tail_info *t;
  struct node *lines, *curr;
  char *curr_char;
  int fd;

  t = parse_args(argc, argv);

  fd = t->filename ? open(t->filename, O_RDONLY) : STDIN_FILENO;
  if (fd == -1) {
    fprintf(stderr, "Failed to open %s: %s\n", t->filename, strerror(errno));
    exit(errno);
  }

  if (posix_fadvise(fd, SEEK_SET, 0, POSIX_FADV_SEQUENTIAL))
    perror("Failed to advise kernel of sequential file access");

  lines = init_circ_buff(t->num_lines);

  curr = lines;
  curr_char = curr->line;
  while (read(fd, curr_char, 1)) {
    if (*curr_char == '\n') {
      *++curr_char = '\0';
      curr = curr->next;
      curr_char = curr->line;
    }
    else
      curr_char++;
  }

  /*
     If we didn't end on a newline, then we won't have ended on the if block
     in the loop so we must null-terminate the string
  */
  if (curr_char != curr->line) {
    *++curr_char = '\0';
    curr = curr->next;
  }
  
  for (unsigned int i = 0; i < t->num_lines; ++i) {
    if (curr->line[0] != '\0')
      printf("%s", curr->line);
    curr = curr->next;
  }

  close(fd);
  free(lines);
  return 0;
}
