#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/resource.h>
#include <pwd.h>
#include <dirent.h>
#include <limits.h>

#include "tree.h"


Process *newProcess(pid_t pid, char *cmd) {
  Process *new_proc;
  if (!(new_proc = malloc(sizeof(Process)))) {
    fprintf(stderr,
	    "ERROR: failed to allocate space for struct for process with pid %u and cmd %s: %s\n",
	    pid,
	    cmd,
	    strerror(errno));
    exit(errno);
  }

  new_proc->pid = pid;
  new_proc->cmd = strdup(cmd);
  new_proc->sibling = NULL;
  new_proc->child = NULL;
  return new_proc;
}

void delProcess(Process *proc) {
  if (proc->child)
    delProcess(proc->child);

  if (proc->sibling)
    delProcess(proc->sibling);

  fprintf(stderr, "DEBUG: deleting process %u:%s\n", proc->pid, proc->cmd);
  free(proc->cmd);
  free(proc);
}

int add2Tree(Process *tree, Process *new_proc, pid_t ppid) {
  Process *pointer;
  int result;

  if (!tree)
    return 0;
  
  pointer = tree;

  if (pointer->pid == ppid) { /* Found parent */
    if (!pointer->child) /* If parent has no children, set child = new_proc */
      pointer->child = new_proc;
    else { /* Else, add new_proc as last sibling of current child */
      pointer = pointer->child;
      while (pointer->sibling)
	pointer = pointer->sibling;
      pointer->sibling = new_proc;
    fprintf(stderr, "DEBUG, process %u added as sibling of %u\n", new_proc->pid, pointer->pid);
    }
    fprintf(stderr, "DEBUG, process %u added as child of %u\n", new_proc->pid, ppid);
    return 1;
  }
  else { /* Did not find parent */
    if (pointer->child) /* First, search through existing children */
      result = add2Tree(pointer->child, new_proc, ppid);
    if (!result && pointer->sibling) /* Then, if parent still remains to be found, search through siblings */
      add2Tree(pointer->sibling, new_proc, ppid);
  }

  return 0;
  
}


uid_t userName2UID(const char *username) {
  struct passwd *user = getpwnam(username);

  if (!user) {
    if (errno != 0)
      fprintf(stderr, "Failed to get %s from /etc/passwd: %s\n", username, strerror(errno));
    else
      fprintf(stderr, "Failed to get %s from /etc/passwd\n", username);
    exit(1);
  }
  return user->pw_uid;
}


Process *buildTree(void) {
  pid_t pid;
  DIR *proc;
  struct dirent *proc_entry;
  char *endptr, *proc_stat_filename, *proc_stat_contents, *token;
  FILE *proc_stat_file;
  Process *proc_tree, *process;

  proc_stat_filename = calloc(PROC_STAT_FILENAME_MAX_LEN, sizeof(char));
  if (!proc_stat_filename) {
    perror("Failed to allocate space for /proc/PID/status");
    exit(1);
  }

  proc_stat_contents = calloc(PROC_STAT_CONTENTS_MAX_LEN, sizeof(char));
  if (!proc_stat_contents) {
    perror("Failed to allocate space for /proc/PID/status in line");
    free (proc_stat_filename);
    exit(1);
  }


  process = malloc(sizeof(process));
  if (!process) {
    perror("Failed to allocate space for process");
    free (proc_stat_filename);
    free(proc_stat_contents);
    exit(1);
  }

  proc = opendir("/proc");
  if (!proc) {
    perror("Failed to open /proc directory");
    free(proc_stat_contents);
    free(proc_stat_filename);
    free(process);
    exit(1);
  }

  proc_tree = newProcess(0, "head"); /* set head equal to process 0 */
  
  while ((proc_entry = readdir(proc))) {
    if ((proc_entry->d_type & DT_DIR) &&  /* This not portable but, given that the stat(2) system call is not covered until Chapter 15, this will suffice */
	strtol(proc_entry->d_name, &endptr, 10)) { /* PIDs must be natural numbers (aka postive integers) */
      strcpy(proc_stat_filename, "/proc/");
      strcat(proc_stat_filename, proc_entry->d_name);
      strcat(proc_stat_filename, "/status");

      proc_stat_file = fopen(proc_stat_filename, "r");
      if (!proc_stat_file) {
	if (errno == ENOENT) {
	  continue;
	}
	else {
	  fprintf(stderr, "Failed to open %s:%s\n", proc_stat_filename, strerror(errno));
	  continue;
	}
      }

      if (!sscanf(proc_entry->d_name, "%u", &pid))
	continue;

      if (fgets(proc_stat_contents, PROC_STAT_CONTENTS_MAX_LEN, proc_stat_file) && !strncmp(proc_stat_contents, "Name:", 5))
	process = newProcess(pid, proc_stat_contents+5);
      else
	continue;

      // TODO: add process to tree
      fclose(proc_stat_file);
      proc_stat_filename[0] = '\0';
    }
  }
  
  closedir(proc);
  free(proc_stat_filename);
  free(proc_stat_contents);
  free(process);
  return proc_tree;
}


int main(int argc, char *argv[argc])
{
  Process *tree = newProcess(0, "0");
  Process *a = newProcess(1, "a");
  Process *b = newProcess(2, "b");
  Process *c = newProcess(3, "c");
  Process *d = newProcess(4, "d");
  Process *e = newProcess(5, "e");

  add2Tree(tree, a, 0);
  add2Tree(tree, b, 0);
  add2Tree(tree, c, 1);
  add2Tree(tree, d, 1);
  add2Tree(tree, e, 2);
  /* a->sibling = b; */
  /* a->child = c; */
  /* c->sibling = d; */
  /* b->child = e; */

  delProcess(tree);
  return 0;
}

/* int main(int argc, char *argv[argc]) */
/* { */

/*   Process a, b; */
/*   printf("%lu\n", sizeof(a)); */

/*   a.cmd = calloc(12, sizeof(char)); */
/*   b.cmd = calloc(12, sizeof(char)); */

/*   printf("%p\n", a.cmd); */
/*   printf("%p\n", b.cmd); */

/*   for (int i = 0; i < 11; ++i) */
/*     a.cmd[i] = 'a'; */

/*   a.cmd[11] = '\0'; */
/*   for (int i = 0; i < 11; ++i) */
/*     b.cmd[i] = 'b'; */

/*   b.cmd[11] = '\0'; */

/*   printf("%s\n", a.cmd); */
/*   printf("%s\n", b.cmd); */
/*   printf("%s\n", a.cmd); */
/*   return 0; */
/* } */
