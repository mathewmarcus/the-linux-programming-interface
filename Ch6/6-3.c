#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


int unsetenv(const char *name) {
  extern char **environ;  
  char **i, **j;
  int len;
  
  if (!(name && *name && !strchr(name, '='))) {
    errno = EINVAL;
    return -1;
  }

  len = strlen(name);  

  i = j = environ; /* can't use environ var as incrementing index because this would globally impact start of env array */
  while (*i=*j) { 
    if (strncmp(*j++, name, len) != 0) {
      i++; 
    }
  }
  
  return 0;
}

int setenv(const char *name , const char *value , int overwrite) {
  char *curr, *new;
  int name_len;

  /*
     Check that
       1. name != NULL
       2. *name != '\0'
       3. value != NULL
  */
  if (!(name && *name && !strchr(name, '='))) {
    errno = EINVAL;
    return -1;
  }

  // handle case where var exists and overwrite == 0
  if ((curr = getenv(name)) && !overwrite)
    return 0;

  name_len = strlen(name);
  if (!(new = malloc(sizeof(char) * (name_len + strlen(value) + 2))))
    return -1; /* failed to allocate space for new variable */
  
  strncpy(new, name, name_len);
  new[name_len] = '=';
  new[name_len + 1] = '\0';
  strcat(new, value);

  if (putenv(new))
    return -1; /* failed to set new variable */

  return 0;
}
