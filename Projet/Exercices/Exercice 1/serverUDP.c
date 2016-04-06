#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
	int clientSocket, n;
	char rcvbuf[1500];
	struct sockaddr_in serv_addr;

	socklen_t len=sizeof(serv_addr);

	memset( (char*) &serv_addr,0, sizeof(serv_addr) );
	 serv_addr.sin_family = PF_INET;
	 serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
	 serv_addr.sin_port = htons(atoi(argv[1]));

	 if ((clientSocket = socket(PF_INET, SOCK_DGRAM, 0)) <0) {
	   perror ("erreur socket");
	   exit (1);
	 }

	 //On indique à la socket qu'elle ne doit pas quitter le réseau local
	 int ttl=1;
	 if (setsockopt(clientSocket, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) <0){
	   perror ("setsockopt");
	   exit (1);
	 }

	if (bind(clientSocket,(struct sockaddr *)&serv_addr, sizeof(serv_addr) ) <0) {
	   perror ("servmulti : erreur bind\n");
	   exit (1);
	}

	while(1){

		//On attend une donnée d'un client. On le récupère et on le stocke dans le buffer
		if ( (n= recvfrom (clientSocket, rcvbuf, sizeof(rcvbuf)-1,0,(struct sockaddr *)&serv_addr, &len)) < 0 )  {
		   perror ("erreur rcvfrom");
		   exit (1);
		}

		//Le message ne contient pas le caractère de fin de chaine.
		rcvbuf[n]='\0';
		printf ("Message reçu du client %s \n", rcvbuf);

		//On renvoie au client la donnée qu'il nous a envoyée. On vérifie que toutes les données ont bien été envoyées.
		if ((n= sendto (clientSocket, rcvbuf, strlen(rcvbuf)-1,0, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) != strlen(rcvbuf)-1)  {
		   perror ("erreur sendto");
		   exit (1);
		}

		printf("ENVOI OK\n");
	}

	return 0;
}