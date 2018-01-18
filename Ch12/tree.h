#include <unistd.h>
#include <stdio.h>
#include <dirent.h>


typedef struct process {
  pid_t pid;
  char *cmd;
  struct process *sibling;
  struct process *child;
} Process;


#define USAGE "Usage:  %s username\n"
#define PROC_STAT_FILENAME_MAX_LEN 19
#define PROC_STAT_CONTENTS_MAX_LEN (9 + LOGIN_NAME_MAX)


Process *newProcess(pid_t pid, char *cmd);
void delProcess(Process *proc);

uid_t userName2UID(const char *username);
int add2Tree(Process *tree, Process *new_proc, pid_t ppid);
Process *buildTree(void);
char *getProcValue(char *key, FILE *proc_status);
void displayTree(Process *tree, unsigned int num_spaces);


void initTree(void);
void freeTree(void);

void procMap(void (*lambda)(struct dirent *proc_entry, FILE *proc_stat_file, va_list vargs), ...);
void file2Node(struct dirent *proc_entry, FILE *proc_stat_file, va_list vargs);
void printProcIfOwner(struct dirent *proc_entry, FILE *proc_stat_file, va_list vargs);

void printUserProcs(uid_t uid);
