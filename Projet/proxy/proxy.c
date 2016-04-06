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

#include "util/util.h"

#define MAXLINE 2000

void usage(){
	perror("Il faut renseigner le numéro de port\n");
}

int main(int argc, char const *argv[])
{
	
	if(argc != 2){
		usage();
		exit(1);
	}

	//Pour gérer getaddrinfo
	int err_code;
	struct addrinfo *res, criteres;

	///// On crée la socket pour gérer les requêtes IPv4 et IPv6
	//On initialise les criteres
	memset(&criteres, 0, sizeof(criteres));

	//On indique qu'on veut une socket serveur
	criteres.ai_flags = AI_PASSIVE;

	//On veut du TCP
	criteres.ai_socktype = SOCK_STREAM;

	//On veut tout prendre : IPv4 et IPv6
	criteres.ai_family = AF_UNSPEC;

	//On met le node à NULL pour avoir un socket de serveur
	err_code = getaddrinfo(NULL, argv[1], &criteres, &res);
	if(err_code){
	fprintf(stderr, "Erreur dans le getaddreinfo : %s\n", gai_strerror(err_code));
	exit(1);
	}

	//On ouvre la socket
	int serverSocket;
	if ((serverSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) <0) {
		perror("Erreur dans l'ouverture dans la socket\n");
		exit (2);
	}

	//On bind la socket
	if (bind(serverSocket,res->ai_addr, res->ai_addrlen) <0) {
		perror ("Erreur dans le bind\n");
		exit (3);
	}

	if (listen(serverSocket, SOMAXCONN) <0) {
		perror ("Erreur dans le listen\n");
		exit (4);
	}

	//On boucle à l'infini
	int clientSocket;
	while(1){
		socklen_t clilen;
		struct sockaddr_in cli_addr;

		clilen = sizeof(cli_addr);

		//On attend une connexion
		clientSocket = accept(serverSocket, (struct sockaddr *) &cli_addr, &clilen);
		if (clientSocket < 0) {
			perror("Il y a eu une erreur dans l'accept\n");
			exit (1);
		}

		printf("Nouvelle requête reçue !\n");

		//On lit les données tant que le client en envoie
		int read;
		char requete[MAXLINE];
		read = readline (clientSocket, requete, MAXLINE);

		//On vérifie qu'il n'y ait pas eu d'erreur dans la lecture
		if(read < 0){
			perror("Erreur dans la lecture de la requête");
			close(serverSocket);
  			close(clientSocket);
  			exit(2);
		}

		//On affiche la requête demandée
		printf("Requête reçue : %s\n", requete);
	}

	close(serverSocket);
  	close(clientSocket);

	return 0;
}