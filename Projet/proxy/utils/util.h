#ifndef UTILS
#define UTILS

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <stdio.h>
#include "socketutil.h"

void usage();
void searchTypeRequest(char entete [], char type[]);
void searchHostName(char entete[], char hostname[]);

#endif