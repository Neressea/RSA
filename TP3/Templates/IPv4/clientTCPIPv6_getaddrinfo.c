/*********************************************************************
 *                                                                   *
 * FICHIER: CLIENT_TCP                                               *
 *                                                                   *
 * DESCRIPTION: Utilisation de TCP socket par une application client *
 *                                                                   *
 * principaux appels (du point de vue client) pour un protocole      *
 * oriente connexion:                                                *
 *     socket()                                                      *
 *                                                                   * 
 *     connect()                                                     *
 *                                                                   *
 *     write()                                                       *
 *                                                                   *
 *     read()                                                        *
 *                                                                   *
 *********************************************************************/
#include     <stdio.h>
#include     <sys/types.h>
#include     <sys/socket.h>
#include     <netinet/in.h>
#include     <arpa/inet.h>
#include     <strings.h>
#include     <string.h>
#include      <netdb.h>
#include      <stdlib.h>
#include <unistd.h>
#include "util.h"


#define MAXLINE 80
void usage(){
  printf("usage : cliecho adresseIP_serveur(x.x.x.x)  numero_port_serveur\n");
}

int main (int argc, char *argv[]){
	
  int serverSocket, n, retread, err_code;
  char fromServer[MAXLINE];
  char fromUser[MAXLINE];

  struct addrinfo *res;
  struct addrinfo criteres;

  /* Verifier le nombre de paramètre en entrée */
  /* clientTCP <hostname> <numero_port>        */
  if (argc != 3){
    usage();
    exit(1);
  }

  //On veut récupérer une adresse IPv6
  criteres.ai_family = PF_INET6;

  //On veut du TCP
  criteres.ai_socktype = SOCK_STREAM;

  //On initialise le reste à 0
  criteres.ai_flags = 0;
  criteres.ai_protocol = 0;
  criteres.ai_addrlen = 0;
  criteres.ai_addr = NULL;
  criteres.ai_canonname = NULL;
  criteres.ai_next = NULL;

  //On recherhce les infos sur le serveur
  err_code = getaddrinfo(argv[1], argv[2], &criteres, &res);

  //On vérifie qu'il n'y ait aps eu d'erreur
  if(err_code){
    fprintf(stderr, "Erreur dans le getaddrinfo : %s", gai_strerror(err_code));
    exit(1);
  }
   
  /*
   * Ouvrir socket (socket STREAM)
   */
  if ((serverSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) <0) {
    freeaddrinfo(res);
    perror("Problème dans l'ouverture de la socket");
    exit(2);
  }

  /*
   * Connect to the serveur 
   */
  if (connect (serverSocket, res->ai_addr, res->ai_addrlen) < 0){
     perror ("Erreur lors de la connexion à la socket");
     exit (1);
  }

  while ( (retread =readline (serverSocket, fromServer, MAXLINE)) > 0)
    {
      printf ("corr: %s", fromServer);
      if (strcmp (fromServer,"Au revoir\n") == 0) 
	break ; /* fin de la lecture */
      
      /* saisir message utilisateur */
      printf("vous: ");
      if (fgets(fromUser, MAXLINE,stdin) == NULL) {
	perror ("erreur fgets \n");
	exit(1);
      }
      
      /* Envoyer le message au serveur */
      if ( (n= writen (serverSocket, fromUser, strlen(fromUser)) ) != strlen(fromUser))  {
	printf ("erreur writen");
	exit (1);
      }
    }
  if(retread < 0 ) {
    	perror ("erreur readline \n");
	//	exit(1);
  }

  close(serverSocket);
  freeaddrinfo(res);

  return 0;
}
