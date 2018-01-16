#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <dirent.h>


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
  int result = 0;

  if (!tree)
    return 0;

  fprintf(stderr, "DEBUG: Attempting to add process %u as child of %u and current process is %u\n", new_proc->pid, ppid, tree->pid);
  if (tree->pid == ppid) { /* Found parent */
    fprintf(stderr, "DEBUG: Found parent of process %u\n", new_proc->pid);
    if (!tree->child) /* If parent has no children, set child = new_proc */ {
      fprintf(stderr, "DEBUG: process %u added as FIRST child of %u\n", new_proc->pid, ppid);      
      tree->child = new_proc;
    }
    else { /* Else, add new_proc as last sibling of current child */
      tree = tree->child;
      while (tree->sibling) {
	tree = tree->sibling;
      }
      tree->sibling = new_proc;
      fprintf(stderr, "DEBUG: process %u added as sibling of %u\n", tree->sibling->pid, tree->pid);
    }
    fprintf(stderr, "DEBUG: process %u added as child of %u\n", new_proc->pid, ppid);
    return 1;
  }
  else { /* Did not find parent */
    fprintf(stderr, "DEBUG: In else block process %u and current process is %u\n", new_proc->pid, tree->pid);
    if (tree->child) /* First, search through existing children */ {
      fprintf(stderr, "DEBUG: Searching children process %u and current process is %u\n", new_proc->pid, tree->pid);
      result = add2Tree(tree->child, new_proc, ppid);
      if (result)
	fprintf(stderr, "DEBUG: Found parent of process %u and current process is %u\n", new_proc->pid, tree->pid);
      else
	fprintf(stderr, "DEBUG: Did not find parent of process %u, current process is %u, result is %u\n", new_proc->pid, tree->pid, result);
    }
    if (new_proc->pid == 4 && tree->pid == 1)
      fprintf(stderr, "process %u %u %u FUCK\n", new_proc->pid, tree->sibling->pid, result);
    
    if (!result && tree->sibling) /* Then, if parent still remains to be found, search through siblings */ {
      fprintf(stderr, "DEBUG: Search siblings for parent of process %u\n", new_proc->pid);
      add2Tree(tree->sibling, new_proc, ppid);
    }
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


char *getProcValue(char *key, FILE *proc_status) {
  int key_len = strlen(key);
  char *proc_status_line = calloc(PROC_STAT_CONTENTS_MAX_LEN, sizeof(char));

  if (!proc_status_line) {
    perror("Failed to allocate space for /proc/PID/status in line");
    exit(1);
  }
  
  if (fseek(proc_status, 0, SEEK_SET) == -1) {
    fprintf(stderr, "Failed to find: %s\n", key);
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
  Process *proc_tree, *process;

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

  proc_tree = newProcess(0, "TREE");
  
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

      if (!sscanf(getProcValue("PPid", proc_stat_file), "%u", &ppid))
	continue;
      printf("%u\n", pid);
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
