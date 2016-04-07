
#include "util/socketutil.h"
#include "util/util.h"

int main(int argc, char const *argv[])
{

	//Déclarations des variables

	//Pour gérer getaddrinfo
	int err_code;
	struct addrinfo criteres;
	struct addrinfo *res = NULL;

	//Socket d'écoute du serveur
	int serverSocket4, serverSocket6;

	//Socket de dialogue
	int clientSocket = -1;

	//Socket pour dialoguer avec le serveur web
	int webSocket = -1;

	int read;
	char requete[MAXREQUEST], type_requete[MAXENTETE], hostname[MAXHOST];
	char response[MAXRESPONSE];

	fd_set init_set, desc_set;
	int maxfdp1, nbfd;
	
	if(argc != 2){
		usage();
		exit(1);
	}

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

	//On ouvre la socket IPv4
	serverSocket4 = createSocket(res);
	
	//On ouvre la socket IPv6
	res = res->ai_next;
	serverSocket6 = createSocket(res);

	//On écoute sur les deux sockets
	if (listen(serverSocket4, SOMAXCONN) <0) {
		perror ("Erreur dans le listen sur la socket IPv4\n");
		exit (4);
	}

	if (listen(serverSocket6, SOMAXCONN) <0) {
		perror ("Erreur dans le listen sur la socket IPv6\n");
		exit (4);
	}

	maxfdp1 = (serverSocket4 < serverSocket6) ? serverSocket6 + 1 : serverSocket4 + 1;

	//On initialise els descripteurs
	FD_ZERO(&init_set);
	FD_ZERO(&desc_set);
	FD_SET(serverSocket4, &init_set);
	FD_SET(serverSocket6, &init_set);

	struct sockaddr_in6 *my_addr = (struct sockaddr_in6 *)res->ai_addr;
	char ip[150];
	inet_ntop(my_addr->sin6_family, my_addr->sin6_addr.s6_addr, ip, sizeof(my_addr->sin6_addr.s6_addr));
	printf("Lancement du serveur sur l'adresse %s sur le port %s \n", ip, argv[1]);
	freeaddrinfo(res);

	//On boucle à l'infini
	while(1){
		desc_set = init_set;

		//On attend une connexion
		nbfd = select(maxfdp1, &desc_set, NULL, NULL, NULL);

		/******* Pn vérifie l'arrivée d'une nouvelle socket d'écoute *******/

		//On a l'arrivée d'un nouveau client en IPv4 et pas d'autres clients en cours
		if(clientSocket == -1 && FD_ISSET(serverSocket4, &desc_set)){

			printf("Arrivée d'un nouveau client en IPv4\n");

			clientSocket = addClient(serverSocket4, &init_set);

			if(clientSocket >= maxfdp1)
				maxfdp1=clientSocket+1;

			nbfd--;

		}else if(clientSocket == -1 && FD_ISSET(serverSocket6, &desc_set)){ //On a reçu un client en IPv6 et pas d'autres clients en cours
			printf("Arrivée d'un nouveau client en IPv6\n");

			clientSocket = addClient(serverSocket6, &init_set);

			if(clientSocket >= maxfdp1)
				maxfdp1=clientSocket+1;

			nbfd--;

		}

		/******* Ensuite on vérifie les sockets de dialogue *******/

		/*** on regarde si l'on a une réponse du serveur web ***/
		if(webSocket >= 0 && FD_ISSET(webSocket, &desc_set)){
			//On lit la réponse du site web
			read = 1;
			memset(response, 0, MAXRESPONSE);
			read = recv(webSocket, response, MAXRESPONSE, 0);

			if(read < 0){
				perror("Erreur dans la lecture de la reponse");
	  			close(webSocket);
	  			exit(5);
			}else if(read == 0){//On regarde si le serveur a fermé la connexion

				//On ferme la socket client
				close(webSocket);
				webSocket = -1;
				FD_CLR(webSocket, &init_set);
				printf("La connexion avec le serveur web a été fermée\n");
			}else{
				//On envoie la requete au client
				printf("Reponse envoyee : \n%s\n", response);
				send(clientSocket, response, MAXRESPONSE, 0);
			}

			nbfd--;
		}

		/*** on regarde si l'on a une requete du client ***/

		if(clientSocket >= 0 && FD_ISSET(clientSocket, &desc_set)){

			//On lit les données tant que le client en envoie. Une requête ne fait pas plus de 5000 octets.
			memset(requete, 0, MAXREQUEST);
			read = recv(clientSocket, requete, MAXREQUEST, 0);

			//On vérifie qu'il n'y ait pas eu d'erreur dans la lecture
			if(read < 0){
				perror("Erreur dans la lecture de la requête");
	  			close(clientSocket);
	  			exit(5);
			}else if(read == 0) { //On regarde si le client a fermé la connexion

				//On ferme la socket client
				close(clientSocket);
				clientSocket = -1;
				FD_CLR(clientSocket, &init_set);
				printf("La connexion avec le client a été fermée\n");

			}else{
				//On récupère le type de la requête
				searchTypeRequest(requete, type_requete);

				printf("la requete est de type : %s\n", type_requete);

				//On ne considère que les requêtes GET
				if(strcmp(type_requete, "GET") == 0){
					searchHostName(requete, hostname);
					printf("Hostname desire : %s\n", hostname);
					//printf("Requete recue : %s\n", requete);

					//On crée la socket de dialogue avec le serveur web
					webSocket = createWebSocket(hostname, "80");

					if(webSocket >= maxfdp1) maxfdp1 = webSocket +1;
					FD_SET(webSocket, &init_set);

					//Puis enfin on envoie la requête au serveur web
					send(webSocket, requete, MAXREQUEST, 0);
				}
			}
			nbfd--;
		}
	}

	close(serverSocket4);
	close(serverSocket6);
	close(clientSocket);

	return 0;
}