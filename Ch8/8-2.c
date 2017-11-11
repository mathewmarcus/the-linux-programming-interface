#include <pwd.h>
#include <string.h>


struct passwd *my_getpwnam(char *name) {
  struct passwd *my_passwd;
  
  while ((my_passwd = getpwent()) && strcmp(name, my_passwd->pw_name));

  my_passwd ? setpwent() : endpwent();
  return my_passwd;
}
