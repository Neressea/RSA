/* Ceci est le code d'une serveur TCP utilisant getaddrinfo
Il peut a la fois gerer les requetes Ipv4 et Ipv6 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <syslog.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <string.h>

usage(){
  printf("usage : servmulti numero_port_serveur\n");
}

int main(int argc, char **argv){

	if (argc != 2){
    		usage();
    		exit(1);
    	}

	int sock[2], ecode, n = 0;
	struct addrinfo *res, *rres, hints;

	//Dans les structure d'adresse addrinfo, on met les informations necessaires a l'etablissement de nos sockets
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = PF_INET6;

	//getaddrinfo renvoit une liste chainee de addrinfo, contenant chacune les informations necessaires a la creation d'une socket
	if ((ecode = getaddrinfo(NULL, argv[1], &hints, &rres)) > 0) {
		printf("getaddrinfo: %s\n", gai_strerror(ecode));
		exit(1);
	}

	/*On parcourt la liste chainee et on ouvre une socket pour chauqe addrinfo. In y a deux addrinfo.
	L'une pour l'ouverture d'une socket IPv4 		
	l'autre pour IPv6 */
	for (res = rres; res; res = res->ai_next){
		if (n == 2){
			perror("n == 2");
			exit(1);	
		}
		if ((sock[n] = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
			perror("socket");
			exit(1);
		}

		if (bind(sock[n], res->ai_addr, res->ai_addrlen) < 0) {
			perror("bind");
			exit(1);
		}

		listen(sock[n], SOMAXCONN);
		n++;

	}

	freeaddrinfo(rres);

	/*Le reste du code est un select qui gere plusieurs client TCP.
	Il faut gerer deux sockets d'ecoute. Sur le meme port, il y a une socket Ipv4 et une socket IPv6.
	Chacune ecoute ses propres requetes. Il faut observer les requetes de ces deux sockets dans le select.*/
	for (;;) {
		int a, f, len, fd, m = -1;
		struct sockaddr_storage from;
		fd_set fdset;


		FD_ZERO(&fdset);
		for (fd = 0; fd < n; fd++) {
			if (m < sock[fd]) m = sock[fd];
			FD_SET(sock[fd], &fdset);
		}

		if ((ecode = select(m+1, &fdset, NULL, NULL, NULL)) < 0){
			perror("select erreur");
			exit(1);
		}else if(ecode == 0){
			exit(0);
		}

		for (fd = 0; fd < n; fd++) {
			if (FD_ISSET(sock[fd], &fdset)) {
				len = sizeof(from);
				if ((a = accept(sock[fd], (struct sockaddr *)&from, &len)) < 0){
					perror("Problème accept");
					exit(1); 
				}
				
				if (fork()==0){
					char msg[2048];
					printf("Client repéré\n");
					int m=read(a, msg, 2047);
					msg[m]='\0';
					printf("%s\n", msg);
					exit(0);
				}
			}
		}

	}
}
