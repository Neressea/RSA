#include "socketutil.h"

//Crée une socket, la bind et renvoie son descripteur
int createSocket(struct addrinfo *res){
	int serverSocket;

	if ((serverSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) <0) {
		perror("Erreur dans l'ouverture dans la socket\n");
		exit (2);
	}

	unsigned int on = 1;

	//Pour que les deux sockets serveurs puissent se connecter au même port
	if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) < 0){
		perror("Problème dans le setsockopt pour les ports multiples");
		exit(10);
	}

	//On la bind
	if (bind(serverSocket,res->ai_addr, res->ai_addrlen) <0) {
		perror ("Erreur dans le bind de la socket\n");
		exit (3);
	}

	return serverSocket;
}

int createWebSocket(char hostname[], char *port){
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

	err_code = getaddrinfo(hostname, port, &criteres, &res);
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
	
	freeaddrinfo(res);

	return webSocket;
}

int addClient(int socket, fd_set *set){
	socklen_t clilen;
	struct sockaddr_in6 cli_addr;
	int sockCli;

	clilen = sizeof(cli_addr);
	sockCli = accept(socket, (struct sockaddr *) &cli_addr, &clilen);

	if (sockCli < 0) {
		perror("Il y a eu une erreur dans la création de la socket d'écoute du nouveau client\n");
		exit (1);
	}

	//On ajoute le client aux descripteurs à suivre
	FD_SET(sockCli, set);

	char addr[150];
	inet_ntop(cli_addr.sin6_family, cli_addr.sin6_addr.s6_addr, addr, sizeof(cli_addr.sin6_addr.s6_addr));
	printf("Le client d'adresse %s et de port %d a bien été ajouté\n", addr, cli_addr.sin6_port);

	return sockCli;
}