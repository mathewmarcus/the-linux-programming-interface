#include <unistd.h>
#include <stdio.h>
#include <dirent.h>


typedef struct process {
  pid_t pid;
  char *cmd;
  struct process *sibling;
  struct process *child;
} Process;


#define PROC_STAT_FILENAME_MAX_LEN 19
#define PROC_STAT_CONTENTS_MAX_LEN (9 + LOGIN_NAME_MAX)

uid_t userName2UID(const char *username);
char *getProcValue(char *key, FILE *proc_status);

void procMap(void (*lambda)(struct dirent *proc_entry, va_list vargs), ...);

void printUserProcs(uid_t uid);
void printPathProcs(char *pathname);

Process *newProcess(pid_t pid, char *cmd);
void delProcess(Process *proc);

int add2Tree(Process *tree, Process *new_proc, pid_t ppid);
Process *buildTree(void);
void displayTree(Process *tree, unsigned int num_spaces);
