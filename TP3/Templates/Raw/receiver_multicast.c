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

#define GROUP "239.137.194.11"
#define PORT 7666

int main(int argc, char const *argv[])
{
	//On crée la socket
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in  serv_addr;

	 /* 
	  * Remplir la structure  serv_addr avec l'adresse multicast
	  */
	  memset( (char *) &serv_addr,0, sizeof(serv_addr) );
	  serv_addr.sin_family = PF_INET;
	  serv_addr.sin_addr.s_addr = inet_addr(GROUP);
	  serv_addr.sin_port = htons(PORT);

	  struct ip_mreq imr;

	  imr.imr_multiaddr.s_addr = inet_addr(GROUP);
	  imr.imr_interface.s_addr = INADDR_ANY;

	  //On demande à rejoindre le groupe
	  if(setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(imr)) < 0){
	  	perror("Problème lors du setsockopt\n");
	  	exit(1);
	  }

	  unsigned int on = 1;
	  if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0){
	  	perror("Problème dans le setsockopt pour les ports multiples");
	  	exit(10);
	  }

	  if(bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
	  	perror("Probleme lors du bind");
	  	exit(2);
	  }

	  int n, len = sizeof(serv_addr);
	  char rcvbuf[1500];	  

	  n = recvfrom(sock, rcvbuf, sizeof(rcvbuf), 0, (struct sockaddr *) &serv_addr, &len);

	  if(n < 0){
	  	perror("Erreur lors de la réceptin du message");
	  	exit(3);
	  }

	  rcvbuf[n]='\0';
	  printf("Message recu : %s\n", rcvbuf);

	  //On quitte le groupe
	  if(setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, &imr, sizeof(imr)) < 0){
	  	perror("Problème lors du départ du groupe\n");
	  	exit(1);
	  }

	return 0;
}