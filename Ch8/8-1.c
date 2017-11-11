#include <stdio.h>
#include <pwd.h>
#include <shadow.h>

int main(int argc, char *argv[argc])
{
  /*
     Assuming both users are exist on the system
     this actually will return two distinct values because
     pw_uid is of type uid_t and so is passed by value
  */
  printf("%ld %ld\n", (long) (getpwnam("avr")->pw_uid),
                      (long) (getpwnam("tsr")->pw_uid));

  /*
     This, however, will print the same memory address,
     because getpwnam returns a pointer to statically
     allocated memory
  */
  printf("%p %p\n", getpwnam("avr"),
	            getpwnam("tsr"));


  /*
     To further illustrate this, we can see that an attempt to
     print distinct usernames will print the same username,
     because the pw_name member is of type char*
  */
  printf("%s %s\n", getpwnam("avr")->pw_name,
	            getpwnam("tsr")->pw_name);
  
  return 0;
}
