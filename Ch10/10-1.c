#define _XOPEN_SOURCE

#include <stdio.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>


int main(int argc, char *argv[argc])
{
  struct tms times_buf;
  clock_t times_ret, clock_ret;
  long times_clock_ticks = sysconf(_SC_CLK_TCK);
 
  for (int i = 0; i < 100000000; ++i)
    getpid();

  if (!(times_ret = times(&times_buf))) {
    perror("times call failed: ");
    exit(1);
  }

  
  if (!(clock_ret = clock())) {
    perror("clock call failed: ");
    exit(1);
  }

  printf("sysconf(_SC_CLK_TCK): %li\n", times_clock_ticks);
  printf("times(struct tms *buf) max duration: %.2f\n\n", pow(2, 32)/times_clock_ticks);

  printf("CLOCKS_PER_SEC: %lu\n", CLOCKS_PER_SEC);
  printf("clock() max program duration: %.2f\n\n", pow(2, 32)/CLOCKS_PER_SEC);
  printf("tms_utime: %.2f\ntms_stime: %.2f\ntms_cutime: %.2f\ntms_cstime: %.2f\n",
  	 (double) times_buf.tms_utime / times_clock_ticks,
  	 (double) times_buf.tms_stime / times_clock_ticks,
  	 (double) times_buf.tms_cutime / times_clock_ticks,
  	 (double) times_buf.tms_cstime / times_clock_ticks);
  printf("times return value in seconds: %.2f\n\n", times_ret/(float) times_clock_ticks);

  printf("clock return value in seconds: %.2f\n", clock_ret/(float) CLOCKS_PER_SEC);
  return 0;
}

