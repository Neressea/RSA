
#include "utils/socketutil.h"
#include "utils/util.h"
#include "utils/secureutil.h"

int main(int argc, char const *argv[])
{

	//Déclarations des variables

	//Pour gérer getaddrinfo
	struct addrinfo *res = NULL;

	//Socket d'écoute du serveur
	int serverSocket4, serverSocket6;

	//Socket de dialogue
	int clientSockets[FD_SETSIZE];

	//Socket pour dialoguer avec le serveur web
	int webSockets[FD_SETSIZE];
	SSL* webSecureCo[FD_SETSIZE];

	int rd;
	char requete[MAXREQUEST], type_requete[MAXENTETE], hostname[MAXHOST];
	char response[MAXRESPONSE];

	fd_set init_set, desc_set;
	int maxfdp1, clientSocket, webSocket, nbfd;
	
	if(argc != 2){
		usage();
		exit(1);
	}

	///// On crée la socket pour gérer les requêtes IPv4 et IPv6
	initSecureConnection();
	initServerSocket(&res, argv[1]);

	//On ouvre la socket IPv4
	serverSocket4 = createSocket(res);
	
	//On ouvre la socket IPv6
	res = res->ai_next;
	serverSocket6 = createSocket(res);

	//On écoute sur les deux sockets
	maxfdp1 = openServer(serverSocket4, serverSocket6);

	//On initialise els descripteurs
	FD_ZERO(&init_set);
	FD_ZERO(&desc_set);
	FD_SET(serverSocket4, &init_set);
	FD_SET(serverSocket6, &init_set);

	//On initialise les tableaux de descripteurs
	int i;
	for(i=0; i<FD_SETSIZE-1; i++){
		clientSockets[i] = -1;
		webSockets[i] = -1;
		webSecureCo[i] = NULL;
	}

	showMyIp(res, argv[1]);
	freeaddrinfo(res);
	//On boucle à l'infini
	while(1){

		desc_set = init_set;

		//On attend une connexion
		nbfd = select(maxfdp1, &desc_set, NULL, NULL, NULL);

		/******* On vérifie l'arrivée d'une nouvelle socket d'écoute *******/
		i = 0;

		//On a l'arrivée d'un nouveau client en IPv4 et pas d'autres clients en cours
		if(FD_ISSET(serverSocket4, &desc_set)){

			printf("\n==================================\n");
			printf("Arrivée d'un nouveau client en IPv4\n");

			clientSocket = addClient(serverSocket4, &init_set);

			if(clientSocket >= maxfdp1)
				maxfdp1=clientSocket+1;

			//On met à jour le tableau
			i=0;
			while(clientSockets[i] >= 0)i++;
			clientSockets[i] = clientSocket;
			nbfd--;

			if(clientSocket >= maxfdp1) maxfdp1 = clientSocket +1;
			FD_SET(clientSocket, &init_set);
		}

		//OPn vérifie qu'on a pas atteint le nombre maximal de client
		if(i == FD_SETSIZE){
			perror("Nombre maximal de clients atteints");
			exit(3);
		}

		if(FD_ISSET(serverSocket6, &desc_set)){ //On a reçu un client en IPv6 et pas d'autres clients en cours
			
			printf("\n==================================\n");
			printf("Arrivée d'un nouveau client en IPv6\n");

			clientSocket = addClient(serverSocket6, &init_set);

			if(clientSocket >= maxfdp1)
				maxfdp1=clientSocket+1;

			//On met à jour le tableau
			i=0;
			while(clientSockets[i] >= 0)i++;
			clientSockets[i] = clientSocket;
			nbfd--;

			if(clientSocket >= maxfdp1) maxfdp1 = clientSocket +1;
			FD_SET(clientSocket, &init_set);
		}

		//On vérifie qu'on a pas atteint le nombre maximal de client
		if(i == FD_SETSIZE){
			perror("Nombre maximal de clients atteints");
			exit(3);
		}

		/******* Ensuite on vérifie les sockets de dialogue *******/
		i = 0;

		while(nbfd > 0 && i<FD_SETSIZE){

			/*** on regarde si l'on a une réponse du serveur web ***/
			webSocket = webSockets[i];
			if(webSocket >= 0 && FD_ISSET(webSocket, &desc_set)){
				memset(response, 0, MAXRESPONSE);

				//Si on a une réponse d'une connexion non sécurisée
				if(webSecureCo[i] == NULL){
					//On lit la réponse du site web
					rd = recv(webSocket, response, MAXRESPONSE, 0);

				}else{ //on a une réponse d'une connexion sécurisée
					rd = SSL_read(webSecureCo[i], response, MAXRESPONSE);
				}

				if(rd < 0){
					perror("Erreur dans la lecture de la reponse");

					if(webSecureCo[i] != NULL){
						printError(SSL_get_error(webSecureCo[i], rd));
					}

		  			close(webSocket);
		  			exit(5);
				}else if(rd == 0){//On regarde si le serveur a fermé la connexion

					//On ferme la connexion sécurisée
					if(webSecureCo[i] != NULL){
						SSL_shutdown(webSecureCo[i]);
						SSL_free(webSecureCo[i]);
						webSecureCo[i] = NULL;
					}

					//On ferme la socket web
					close(webSocket);
					FD_CLR(webSocket, &init_set);
					webSocket = -1;
					printf("\n==================================\n");
					printf("La connexion avec le serveur web a été fermée\n");
					printf("\n==================================\n");
				}else{
					//On envoie la requete au client
					printf("\n==================================\n");
					printf("Réponse envoyée : \n%s\n", response);
					printf("\n==================================\n");
					send(clientSockets[i], response, rd, 0);
				}
				
				//On met à jour le tableau
				webSockets[i] = webSocket;
				nbfd--;
			}

			/*** on regarde si l'on a une requete du client ***/
			clientSocket = clientSockets[i];
			if(clientSocket >= 0 && FD_ISSET(clientSocket, &desc_set)){

				//On lit les données tant que le client en envoie. Une requête ne fait pas plus de 5000 octets.
				memset(requete, 0, MAXREQUEST);
				rd = recv(clientSocket, requete, MAXREQUEST, 0);

				//On vérifie qu'il n'y ait pas eu d'erreur dans la lecture
				if(rd < 0){
					perror("Erreur dans la lecture de la requête");
		  			close(clientSocket);
		  			exit(5);
				}else if(rd == 0) { //On regarde si le client a fermé la connexion

					//On ferme la socket client
					close(clientSocket);
					FD_CLR(clientSocket, &init_set);
					clientSocket = -1;
					printf("\n==================================\n");
					printf("La connexion avec le client a été fermée\n");
					printf("\n==================================\n");


				}else{
					//On récupère le type de la requête
					searchTypeRequest(requete, type_requete);

					printf("\n==================================\n");
					printf("la requete est de type : %s\n", type_requete);

					//On ne considère que les requêtes GET
					if(strcmp(type_requete, "GET") == 0){
						searchHostName(requete, hostname);
						printf("Hostname desire : %s\n", hostname);

						printf("Requete complete : \n%s\n", requete);

						//On crée la socket de dialogue avec le serveur web
						webSocket = createWebSocket(hostname);

						//On ne cherche pas la première case non utilisée du tableau.
						//On utilise l'indice i pour lier les sockets clients aux websockets correspondantes
						webSockets[i] = webSocket;

						if(webSocket >= maxfdp1) maxfdp1 = webSocket +1;
						FD_SET(webSocket, &init_set);

						//Puis enfin on envoie la requête au serveur web
						send(webSocket, requete, rd, 0);

						//On met tout ça dans un fichier de log
						char request[150];
						searchRequest(requete, request);
						addRequestLog(clientSockets[i], type_requete, request);
					}else if(strcmp(type_requete, "CLOSE") == 0){
						close(clientSocket);
						FD_CLR(clientSocket, &init_set);
						clientSocket = -1;
						printf("\n==================================\n");
						printf("La connexion avec le client a été fermée\n");
						printf("\n==================================\n");
					}else if(strcmp(type_requete, "CONNECT") == 0){
						//On a du HTTPS
						searchHostName(requete, hostname);
						printf("Hostname desire : %s\n", hostname);

						printf("Requete complete : \n%s\n", requete);

						//Le numéro de port est celui du HTTPS
						webSocket = createSecureSocket(hostname, i, webSecureCo);
						//On ne cherche pas la première case non utilisée du tableau.
						//On utilise l'indice i pour lier les sockets clients aux websockets correspondantes
						webSockets[i] = webSocket;

						if(webSocket >= maxfdp1) maxfdp1 = webSocket +1;
						FD_SET(webSocket, &init_set);

						//Puis enfin on envoie la requête au serveur web
						SSL_write(webSecureCo[i], requete, rd);

						//On met tout ça dans un fichier de log
						char request[150];
						searchRequest(requete, request);
						addRequestLog(clientSockets[i], type_requete, request);
					}else{
						printf("Type de requete non taité : %s\n", type_requete);
					}
					printf("\n==================================\n");
				}

				nbfd--;
				clientSockets[i] = clientSocket;
			}

			i++;
		}
	}

	close(serverSocket4);
	close(serverSocket6);
	close(clientSocket);

	return 0;
}