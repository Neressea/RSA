#include     <stdio.h>
#include     <sys/types.h>
#include     <sys/socket.h>
#include     <netinet/in.h>
#include     <sys/un.h>
#include     <arpa/inet.h>
#include     <sys/uio.h>
#include     <time.h>
#include     <sys/timeb.h>
#include     <netdb.h>
#include     <stdlib.h>
#include     <strings.h>

#define BUFSIZE 2400

usage(){
  printf("usage : cliecho adresseIP_serveur(x.x.x.x)  numero_port_serveur\n");
}

int main (int argc, char *argv[])
{
	
	int groupSocket;
	struct sockaddr_in addr_r;	
	char buff[BUFSIZE];
	struct ip_mreq imr;
	unsigned int on = 1;
	int len_r = sizeof(addr_r);

	if (argc != 3){
 		usage();
 		exit(1);
 	}


	if ((groupSocket = socket(AF_INET, SOCK_DGRAM, 0)) <0) {
 		perror ("erreur socket");
 		exit (1);
 	}
	
	// On prepare la structure d'adresse pour le groupe multicast
	memset(&imr,0, sizeof(imr));
	imr.imr_multiaddr.s_addr = inet_addr(argv[1]);
	imr.imr_interface.s_addr = INADDR_ANY;

	// On prepare la socket pour une emission sur le port entre en argument
	memset(&addr_r,0, sizeof(addr_r));
	addr_r.sin_family = AF_INET;
	addr_r.sin_addr.s_addr = htons(INADDR_ANY);
	addr_r.sin_port = htons(atoi(argv[2]));


	/*On lie la socket à l'adresse IP du groupe entree en argument
	La machine renvoit cette information au routeur de son reseau local */
	if(setsockopt(groupSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(imr)) < 0){
		perror("probleme setsockopt");
		exit(1);
	}

	//On previent la socket pour le reutilisation de l'adresse IP
	if (setsockopt(groupSocket,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)) < 0){
		perror("Probleme setsockopt");
		exit(1);
	}

	//On lie la socket a son port
	if(bind(groupSocket, (struct sockaddr *) &addr_r, sizeof(addr_r)) < 0){
		perror("Probleme bind");
		exit(1);
	}


	//On reçoit les donnees du groupe et on les affichent
	if(recvfrom(groupSocket, buff, sizeof(buff), 0, (struct sockaddr *)&addr_r, &len_r) <= 0){
		perror("Probleme recvfrom");
		exit(1);
	}

	printf("%s\n", buff);
	
	close(groupSocket);
	exit(0);

}
