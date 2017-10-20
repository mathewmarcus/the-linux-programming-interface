/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2017.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 5-3 */

/* large_file.c

   Demonstrate the use of the (obsolete) Large File System API.

   This program is Linux-specific.
*/
/* #define _LARGEFILE64_SOURCE */
#define _FILE_OFFSET_BITS 64
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int fd;
    off_t off;

    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
      fprintf(stderr, "%s pathname offset\n", argv[0]);
      exit(1);
    }

    fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open64");
    	exit(1);
    }

    off = atoll(argv[2]);
    if (lseek(fd, off, SEEK_SET) == -1) {
        perror("lseek64");
    	exit(1);
    }

    if (write(fd, "test", 4) == -1) {
        perror("write");
    	exit(1);
    }
    exit(EXIT_SUCCESS);
}
