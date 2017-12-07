#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>

#define USAGE "Usage:  %s username\n"
#define PROC_STAT_FILENAME_MAX_LEN 19
#define PROC_STAT_CONTENTS_MAX_LEN (9 + LOGIN_NAME_MAX)

unsigned int getPIDMax(void);
uid_t userName2UID(const char *username);
int main(int argc, char *argv[argc])
{
  uid_t userid, ruid;
  DIR *proc;
  struct dirent *proc_entry;
  char *endptr, *proc_stat_filename, *proc_stat_contents, *token;
  FILE *proc_stat_file;

  if (argc != 2) {
    fprintf(stderr, USAGE, argv[0]);
    exit(1);
  }

  userid = userName2UID(argv[1]);

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

  proc = opendir("/proc");
  if (!proc) {
    perror("Failed to open /proc directory");
    free(proc_stat_contents);
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
	if (errno == ENOENT) {
	  continue;
	}
	else {
	  fprintf(stderr, "Failed to open %s:%s\n", proc_stat_filename, strerror(errno));
	  continue;
	}
      }

      /* First, check if user owns process*/
      while (fgets(proc_stat_contents, PROC_STAT_CONTENTS_MAX_LEN, proc_stat_file) && strncmp(proc_stat_contents, "Uid:", 4));
      if (!proc_stat_contents) {
	fprintf(stderr, "Failed to find Uid line in /proc/%s/status", proc_entry->d_name);
	continue;
      }

      token = strtok(proc_stat_contents, "\t");
      token = strtok(NULL, "\t");

      if (sscanf(token, "%u", &ruid) && ruid != userid)
	continue;

      if (fseek(proc_stat_file, 0, SEEK_SET) == -1) {
	fprintf(stderr, "Failed to seek to beginning of %s\n", proc_stat_filename);
      }

      if (fgets(proc_stat_contents, PROC_STAT_CONTENTS_MAX_LEN, proc_stat_file) && !strncmp(proc_stat_contents, "Name:", 5))
	printf("%s %s", proc_entry->d_name, proc_stat_contents + 5);

      fclose(proc_stat_file);
      proc_stat_filename[0] = '\0';
    }
  }
  
  closedir(proc);
  free(proc_stat_filename);
  free(proc_stat_contents);
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
