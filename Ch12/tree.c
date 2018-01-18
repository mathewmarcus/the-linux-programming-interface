#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <dirent.h>

#include "tree.h"


static Process *proc_tree;


void initTree() {
  proc_tree = newProcess(0, "TREE\n");
}

void delTree() {
  delProcess(proc_tree);
}


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

  free(proc->cmd);
  free(proc);
}

void displayTree(Process *tree, unsigned int num_spaces) {
  for (unsigned int i = 0; i < num_spaces; i++)
    printf("%c", '\t');

  printf("PID: %u, CMD: %s", tree->pid, tree->cmd);

  if (tree->child)
    displayTree(tree->child, num_spaces+1);

  if (tree->sibling)
    displayTree(tree->sibling, num_spaces);
}


int add2Tree(Process *tree, Process *new_proc, pid_t ppid) {
  int result = 0;

  if (tree->pid == ppid) { /* Found parent */
    if (!tree->child) /* If parent has no children, set child = new_proc */
      tree->child = new_proc;
    else { /* Else, add new_proc as last sibling of current child */
      tree = tree->child;
      while (tree->sibling)
	tree = tree->sibling;
      tree->sibling = new_proc;
    }
    return 1;
  }
  else { /* Did not find parent */
    if (tree->child) /* First, search through existing children */
      result = add2Tree(tree->child, new_proc, ppid);
    if (!result && tree->sibling) /* Then, if parent still remains to be found, search through siblings */
      result = add2Tree(tree->sibling, new_proc, ppid);
    return result;
  }
  
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


char *getProcValue(char *key, FILE *proc_status) {
  int key_len = strlen(key);
  char *proc_status_line = calloc(PROC_STAT_CONTENTS_MAX_LEN, sizeof(char));

  if (!proc_status_line) {
    perror("Failed to allocate space for /proc/PID/status in line");
    exit(1);
  }
  
  if (fseek(proc_status, 0, SEEK_SET) == -1) {
    fprintf(stderr, "Failed to find %s: %s\n", key, strerror(errno));
    return NULL;
  }

  while (fgets(proc_status_line,
	       PROC_STAT_CONTENTS_MAX_LEN,
	       proc_status)
	 && (strncmp(proc_status_line, key, key_len)
	     || proc_status_line[key_len] != ':'));

  return !feof(proc_status) ? (proc_status_line + key_len + 2) : NULL;
}


Process *buildTree(void) {
  pid_t pid, ppid;
  DIR *proc;
  struct dirent *proc_entry;
  char *endptr, *proc_stat_filename;
  FILE *proc_stat_file;
  Process *process;

  proc_stat_filename = calloc(PROC_STAT_FILENAME_MAX_LEN, sizeof(char));
  if (!proc_stat_filename) {
    perror("Failed to allocate space for /proc/PID/status");
    exit(1);
  }

  process = malloc(sizeof(process));
  if (!process) {
    perror("Failed to allocate space for process");
    free (proc_stat_filename);
    exit(1);
  }

  proc = opendir("/proc");
  if (!proc) {
    perror("Failed to open /proc directory");
    free(proc_stat_filename);
    free(process);
    exit(1);
  }
  
  while ((proc_entry = readdir(proc))) {
    if ((proc_entry->d_type & DT_DIR) &&  /* This not portable but, given that the stat(2) system call is not covered until Chapter 15, this will suffice */
	strtol(proc_entry->d_name, &endptr, 10)) { /* PIDs must be natural numbers (aka postive integers) */
      strcpy(proc_stat_filename, "/proc/");
      strcat(proc_stat_filename, proc_entry->d_name);
      strcat(proc_stat_filename, "/status");

      proc_stat_file = fopen(proc_stat_filename, "r");
      if (!proc_stat_file) {
	if (errno == ENOENT)
	  continue;
	else {
	  fprintf(stderr, "Failed to open %s:%s\n", proc_stat_filename, strerror(errno));
	  continue;
	}
      }

      if (!sscanf(proc_entry->d_name, "%u", &pid))
	continue;

      if (!sscanf(getProcValue("PPid", proc_stat_file), "%u", &ppid))
	continue;

      process = newProcess(pid, getProcValue("Name", proc_stat_file));
      add2Tree(proc_tree, process, ppid);

      fclose(proc_stat_file);
      proc_stat_filename[0] = '\0';
    }
  }
  
  closedir(proc);
  free(proc_stat_filename);
  return proc_tree;
}


void procMap(void (*lambda())) {
  DIR *proc;
  struct dirent *proc_entry;
  char *endptr, *proc_stat_filename;
  FILE *proc_stat_file;

  proc_stat_filename = calloc(PROC_STAT_FILENAME_MAX_LEN, sizeof(char));
  if (!proc_stat_filename) {
    perror("Failed to allocate space for /proc/PID/status");
    exit(1);
  }

  proc = opendir("/proc");
  if (!proc) {
    perror("Failed to open /proc directory");
    free(proc_stat_filename);
    exit(1);
  }

  while ((proc_entry = readdir(proc))) {
    if ((proc_entry->d_type & DT_DIR) &&  /* This not portable but, given that the stat(2) system call is not covered until Chapter 15, this will suffice */
	strtol(proc_entry->d_name, &endptr, 10)) { /* PIDs must be natural numbers (aka postive integers) */
      strcpy(proc_stat_filename, "/proc/");
      strcat(proc_stat_filename, proc_entry->d_name);
      strcat(proc_stat_filename, "/status");

      proc_stat_file = fopen(proc_stat_filename, "r");
      if (!proc_stat_file) {
	if (errno == ENOENT)
	  continue;
	else {
	  fprintf(stderr, "Failed to open %s:%s\n", proc_stat_filename, strerror(errno));
	  continue;
	}
      }

      lambda(proc_entry, proc_stat_file);

      fclose(proc_stat_file);
      proc_stat_filename[0] = '\0';
    }
  }
  
  closedir(proc);
  free(proc_stat_filename);
  
}
