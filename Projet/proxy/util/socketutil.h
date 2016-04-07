#ifndef SOCKETUTIL
#define SOCKETUTIL

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

#define MAXREQUEST 5000
#define MAXBUFF 1000
#define MAXENTETE 50
#define MAXHOST 100

#define MAXRESPONSE 65536

int createSocket(struct addrinfo *res);
int createWebSocket(char hostname[], char *port);
int addClient(int socket, fd_set *set);

#endif