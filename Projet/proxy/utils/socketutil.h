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

#define MAXRESPONSE 1500

int createSocket(struct addrinfo *res);
int createWebSocket(char hostname[], char service[]);
int addClient(int socket, fd_set *set);
int openServer(int serverSocket4, int serverSocket6);
void initServerSocket(struct addrinfo **res, const char *num_port);

#endif