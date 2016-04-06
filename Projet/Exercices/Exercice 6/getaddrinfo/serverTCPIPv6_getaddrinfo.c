/*********************************************************************
 *                                                                   *
 * FICHIER: SERVER_TCP                                               *
 *                                                                   *
 * DESCRIPTION: Utilisation de TCP socket par une application serveur*
 *              application client                                   *
 *                                                                   *
 * principaux appels (du point de vue serveur) pour un protocole     *
 * oriente connexion:                                                *
 *     socket()                                                      *
 *                                                                   * 
 *     bind()                                                        *
 *                                                                   * 
 *     listen()                                                      *
 *                                                                   *
 *     accept()                                                      *
 *                                                                   *
 *     read()                                                        *
 *                                                                   *
 *     write()                                                       *
 *                                                                   *
 *********************************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 #include <netdb.h>
#include "../util/util.h"

#define MAXLINE 80

void usage(){
  printf("usage : servecho numero_port_serveur\n");
}


int main (int argc, char *argv[])

{
  int serverSocket, clientSocket; /* declaration socket passive et socket active */

  int n, retread, err_code;
  socklen_t clilen;
  struct sockaddr_in cli_addr;
    char fromClient[MAXLINE];
  char fromUser[MAXLINE];

  /* Verifier le nombre de paramètre en entrée */
  /* serverTCP <numero_port>                   */ 
  if (argc != 2){
    usage();
    exit(1);
  }

  struct addrinfo *res, criteres;

  //On initialise les criteres
  memset(&criteres, 0, sizeof(criteres));

  //On indique qu'on veut uen socket serveur
  criteres.ai_flags = AI_PASSIVE;

  //On veut du TCP
  criteres.ai_socktype = SOCK_STREAM;

  //On veut de l'IPv6
  criteres.ai_family = PF_INET6;

  //On met le node à NULL pour avoir un socket de serveur
  err_code = getaddrinfo(NULL, argv[1], &criteres, &res);
  if(err_code){
    fprintf(stderr, "Erreur dans le getaddreinfo : %s\n", gai_strerror(err_code));
    exit(1);
  }

   /*
   * Ouvrir une socket (a TCP socket)
   */
  if ((serverSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) <0) {
   perror("servecho : Probleme socket\n");
   exit (2);
  }
 
  if (bind(serverSocket,res->ai_addr, res->ai_addrlen) <0) {
   perror ("servecho: erreur bind\n");
   exit (3);
  }
  
  /* Paramètrer le nombre de connexion "pending" */
  if (listen(serverSocket, SOMAXCONN) <0) {
    perror ("servecho: erreur listen\n");
    exit (4);
  }


  clilen = sizeof(cli_addr);
  clientSocket = accept(serverSocket, (struct sockaddr *) &cli_addr, &clilen);
  if (clientSocket < 0) {
    perror("servecho : erreur accept\n");
    exit (1);
  }

  /* Envoyer Bonjour au client */
  if ( (n= writen (clientSocket, "Bonjour\n", strlen("Bonjour\n")) ) != strlen("Bonjour\n"))  {
	printf ("erreur writen");
	exit (1);
     }

  while ( (retread=readline (clientSocket, fromClient, MAXLINE)) > 0)
    {
      printf ("corr: %s", fromClient);
      if (strcmp (fromClient,"Au revoir\n") == 0) 
	     break ; /* fin de la lecture */
      
      /* saisir message utilisateur */
      printf("vous: ");
      if (fgets(fromUser, MAXLINE,stdin) == NULL) {
      	perror ("erreur fgets \n");
      	exit(1);
      }
      
      /* Envoyer le message au client */
      if ( (n= writen (clientSocket, fromUser, strlen(fromUser)) ) != strlen(fromUser))  {
      	printf ("erreur writen");
      	exit (1);
     }

    }

  if(retread < 0 ) {
    perror ("erreur readline \n");
    //   exit(1);
  }
  close(serverSocket);
  close(clientSocket);
  return 0;
}
