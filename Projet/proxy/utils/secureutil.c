#include "secureutil.h"

void initSecureConnection(){
	//On initialise la connexion HTTPS
	//On initialise OpenSSL
	SSL_library_init();
	SSL_load_error_strings();
	ssl_ctx = SSL_CTX_new(TLSv1_1_client_method());
}

int createSecureSocket(char hostname[], int indice, SSL* webSecureCo[]){
	//Pour gérer getaddrinfo
	int err_code;
	struct addrinfo *res, criteres;
	int webSocket;

	//On initialise les criteres
	memset(&criteres, 0, sizeof(criteres));

	//On veut du TCP
	criteres.ai_socktype = SOCK_STREAM;

	//On veut tout prendre : IPv4 et IPv6
	criteres.ai_family = AF_UNSPEC;

	err_code = getaddrinfo(hostname, "443", &criteres, &res);
	if(err_code){
		fprintf(stderr, "Erreur dans le getaddreinfo : %s\n", gai_strerror(err_code));
		exit(1);
	}

	if ((webSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) <0) {
		perror("Erreur dans l'ouverture dans la socket\n");
		exit (2);
	}

	unsigned int on = 1;

	//Pour que les deux sockets serveurs puissent se connecter au même port
	if(setsockopt(webSocket, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) < 0){
		perror("Problème dans le setsockopt pour les ports multiples");
		exit(10);
	}

	if(connect (webSocket, res->ai_addr, res->ai_addrlen)  < 0){
		printf("Problème connect pour la socket web\n");
		exit(1);
	}

	SSL *conn = SSL_new(ssl_ctx);
	if(conn == NULL){
		printf("Problème connect pour la socket web securisee\n");
		exit(1);
	}

	SSL_set_fd(conn, webSocket);

	if ( SSL_connect(conn) != 1 ){   /* perform the connection */
        perror("ERREUR");
        ERR_print_errors_fp(stderr);
    }

	freeaddrinfo(res);
	webSecureCo[indice] = conn;

	return webSocket;
} 

void printError(int errcode){
	switch(errcode){
		case SSL_ERROR_WANT_READ:
			printf("Une erreur SSL a eu lieu : SSL_ERROR_WANT_READ\n");
			break;
		case SSL_ERROR_WANT_WRITE:
			printf("Une erreur SSL a eu lieu : SSL_ERROR_WANT_WRITE\n");
			break;
		case SSL_ERROR_WANT_CONNECT:
			printf("Une erreur SSL a eu lieu : SSL_ERROR_WANT_CONNECT\n");
			break;
		case SSL_ERROR_WANT_ACCEPT:
			printf("Une erreur SSL a eu lieu : SSL_ERROR_WANT_ACCEPT\n");
			break;
		case SSL_ERROR_WANT_X509_LOOKUP:
			printf("Une erreur SSL a eu lieu : SSL_ERROR_WANT_X509_LOOKUP\n");
			break;
		// case SSL_ERROR_WANT_ASYNC:
		// 	printf("Une erreur SSL a eu lieu : SSL_ERROR_WANT_ASYNC\n");
		// 	break;
		case SSL_ERROR_SYSCALL:
			printf("Une erreur SSL a eu lieu : SSL_ERROR_SYSCALL\n");
			break;
		case SSL_ERROR_SSL:
			printf("Une erreur SSL a eu lieu : SSL_ERROR_SSL\n");
			break;
	}
}