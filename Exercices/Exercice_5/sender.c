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

usage(){
  printf("usage : cliecho adresseIP_serveur(x.x.x.x)  numero_port_serveur\n");
}

int main (int argc, char *argv[])
{
	
	int groupSocket;
	struct sockaddr_in addr_s;	
	unsigned char ttl = 1;
	char * buff;
	struct timeb tp;

	if (argc != 3){
 		usage();
 		exit(1);
 	}


	if ((groupSocket = socket(AF_INET, SOCK_DGRAM, 0)) <0) {
 		perror ("erreur socket");
 		exit (1);
 	}
	
	//On prepare la structure de donnees pour emettre des paquets sur l'IP multicast entree en argument
	memset(&addr_s,0, sizeof(addr_s));
	addr_s.sin_family = AF_INET;
	addr_s.sin_addr.s_addr = inet_addr(argv[1]);
	addr_s.sin_port = htons(atoi(argv[2]));

	// On lit l'addresse multicast a la socket
	if(setsockopt(groupSocket, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0){
		perror("Probleme setsockopt");
		exit(1);
	}

	ftime(&tp);
	buff = ctime(&tp.time);

	//On envoie les donnees sur le reseau avec pour IP source l'IP multicast
	if(sendto(groupSocket, buff, strlen(buff)+1, 0, (struct sockaddr *)&addr_s, sizeof(addr_s)) < 0){
		perror("Probleme sendto");
		exit(1);
	}

	close(groupSocket);	
	exit(0);

}
