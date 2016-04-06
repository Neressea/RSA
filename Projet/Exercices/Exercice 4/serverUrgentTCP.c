/*********************************************************************
 *                                                                   *
 * FICHIER: SERVER_URGENT_TCP                                        *
 *                                                                   *
 * DESCRIPTION: Utilisation de la donnee urgente de TCP               *
 *                                                                   *
 *********************************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>

int serverSocket, clientSocket; /* declaration socket passive et socket active */


void sig_urg(int sig)
{
  int n;
  char buff[2048];
  printf ("SIGURG recu \n");


   printf("Recepteur: recu signal SIGURG (%d): caractere urgent\n",sig);
   /* Reception du caractere urgent */

   //DONE
   //On indique à recv qu'on veut lire les données urgentes
   n = recv(clientSocket, buff, 2048,MSG_OOB);

  printf ("read %d OOB octets %s\n", n,buff);
}

usage(){
  printf("usage : servecho numero_port_serveur\n");
}


int main (int argc, char *argv[])

{
  struct sigaction action;

  int n, clilen, servlen, size;
  struct sockaddr_in  serv_addr, cli_addr;
  char buff[2048];
  socklen_t optionlen;

  /* Verifier le nombre de paramètre en entrée */
  /* serverTCP <numero_port>                   */ 
  if (argc != 2){
    usage();
    exit(1);
  }

  // Ouvrir une socket (a TCP socket)
  if ((serverSocket = socket(PF_INET, SOCK_STREAM, 0)) <0) {
   perror("servecho : Probleme socket\n");
   exit (2);
  }

  // Lier l'adresse  locale à la socket
  memset( (char*) &serv_addr, 0, sizeof(serv_addr) );
  serv_addr.sin_family = PF_INET;
  serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
  serv_addr.sin_port = htons(atoi(argv[1]));

  if (bind(serverSocket,(struct sockaddr *)&serv_addr, sizeof(serv_addr) ) <0) {
   perror ("servecho: erreur bind\n");
   exit(1);
  }
 
  // Obtenir taille buffer de reception
  //DONE
  //On récupère des infos sur le socket du serveur : la taille du buffer de réception
  //SOL_SOCKET indique que l'on s'intéresse à la couche socket de la connexion
  getsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, &size, &optionlen);
  printf (" la taille du buffer de reception est de %d \n", size);

 
  // Paramètrer le nombre de connexion "pending" 
  if (listen(serverSocket, SOMAXCONN) <0) {
    perror ("servecho: erreur listen\n");
    exit (1);
  }

  // Attente de la demande de connexion du client
  clilen = sizeof(cli_addr);
  clientSocket = accept(serverSocket, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
  if (clientSocket < 0) {
    perror("servecho : erreur accept\n");
    exit (1);
  }


  // Armer le signal SIGUR avec comme handler sig_urg
  // DONE
  //Lorsque SIGURG est reçu, sig_urg est appelée
  signal(SIGURG, sig_urg);

 
  // Demander au systeme d'envoyer le signal au proprietaire de la socket
  // DONE
  //On indique au système que l'on veut recevoir les signaux SIGURG et SIGIO de clientSocket 
  fcntl(clientSocket, F_SETOWN, getpid());

  for (;;)
    {
      // Reception des donnees non urgents. Si Fin de connexion ou erreur  
      // sortir du serveur
      // Attention au traitement de l'erreur EINTR
      
        //DONE
        //On lit les donénes recues du client
        n = read(clientSocket, buff, sizeof(buff)-1);

        //S'il n'y en a pas : erreur
        if (n < 0){
          perror("Erreur lors de la lecture sur la socket");
          exit(1);
        }else if(n == 0){
          //On ferme la connexion si aucune donnée n'a été reçue sans que cela ne soit une erreur
          printf("Fermeture de la connexion\n");
          close(serverSocket);
          close(clientSocket);
          exit(0);
        }else{
          //Autrement on affiche le message reçu
          buff[n]='\0';
          printf("Message recu : %s\n", buff);
        }
    }
}
