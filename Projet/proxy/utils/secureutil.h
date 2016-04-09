#ifndef SECUREUTIL
#define SECUREUTIL

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

//On inclut openssl pour gérer les connexions HTTPS
#include <openssl/ssl.h>
#include <openssl/err.h>

//On crée une variable superglobale
SSL_CTX *ssl_ctx;

void initSecureConnection();
int createSecureSocket(char hostname[], int indice, SSL* webSecureCo[]);
void printError(int errcode);

#endif