#define _DEFAULT_SOURCE
#include <grp.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int my_initgroups(char *user, gid_t group) {
  struct group *g;
  size_t gidsetsize = 0;  
  gid_t *grouplist = calloc(sysconf(_SC_NGROUPS_MAX+1), sizeof(gid_t));

  if (!grouplist)
    return -1;

  while ((g = getgrent())) {
    for (char **group_mem = g->gr_mem; *group_mem; group_mem++)
      if (!strcmp(user, *group_mem)) {
	*(grouplist + gidsetsize++) = g->gr_gid;
      }
  }
  *(grouplist + gidsetsize++) = group;

  if (setgroups(gidsetsize, grouplist) == -1) { /* Unprivileged process */
    free(grouplist);
    return -1;
  }
  return 0;
}
