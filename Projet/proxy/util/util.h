#ifndef UTILS
#define UTILS

#include    <errno.h>
#include <stdio.h>
#include <unistd.h>

int writen (int  fd, char *ptr, int nbytes);
int readn (int  fd, char *ptr, int maxlen);
int readline (int  fd, char *ptr, int maxlen);

#endif