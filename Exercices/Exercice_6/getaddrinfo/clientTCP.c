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
  printf("usage : cliecho adresseIP_serveur(x.x.x.x)  numero_port_serveur\n");
}

int main(int argc, char **argv){
	
	if (argc != 3){
    		usage();
    		exit(1);
    	}	

	int sock, ecode;
	struct addrinfo *res;

	struct addrinfo hints = {
		0,
		PF_UNSPEC,
		SOCK_STREAM,
		0,
		0,
		NULL,
		NULL,
		NULL
	};
	
	if (getaddrinfo(argv[1], argv[2], &hints, &res) < 0){
		perror("Probleme getaddrinfo");
		exit(1);
	}

	if ((sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0){
		freeaddrinfo(res);		
		perror("Probleme socket");
		exit(1);
	}

	if (connect(sock, res->ai_addr, res->ai_addrlen) < 0){
		close(sock);
		freeaddrinfo(res);
		perror("connect");
		exit(1);
	}

	char msg[] = "Bonjour je suis le cient TCP avec un addrinfo";

	write(sock, msg, strlen(msg)); 

	

	freeaddrinfo(res);
	

}

