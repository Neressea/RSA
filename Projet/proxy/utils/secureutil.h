#ifndef SECUREUTIL
#define SECUREUTIL

//On inclut openssl pour gérer les connexions HTTPS
#include <openssl/ssl.h>
#include <openssl/err.h>

//On crée une variable superglobale
SSL_CTX *ssl_ctx;

void initSecureConnection();
int createSecureSocket(char hostname[], int indice, SSL* webSecureCo[]);

#endif