#include "util.h"

void usage(){
  perror("Il faut renseigner le numéro de port pour les socket !\n");
}

void searchTypeRequest(char entete [], char type[]){
  int i = 0;

  //On récupère la commande
  memset(type, 0, sizeof(type));
  while(entete[i] != ' ' && i < MAXENTETE){
    type[i] = entete[i];
    i++;
  }
}

void searchHostName(char entete[], char hostname[]){
  int i=0;

  //On passe la commande
  while(entete[i] != ' ' && i < MAXENTETE)i++;

  //On récupère le hostname
  int newi = i;
  int hosti = 0;

  //On élimine le http://
  while(newi - i <= 7) newi++;

  while(entete[newi] != '/' && i < MAXHOST){
    hostname[hosti++] = entete[newi];
    newi++;
  }

  hostname[hosti]='\0';
}

void showMyIp(struct addrinfo *res){
  struct sockaddr_in6 *my_addr = (struct sockaddr_in6 *)res->ai_addr;
  char ip[150];
  inet_ntop(my_addr->sin6_family, my_addr->sin6_addr.s6_addr, ip, sizeof(my_addr->sin6_addr.s6_addr));
  printf("\n==================================\n");
  printf("Lancement du serveur sur l'adresse %s sur le port %d \n", ip, my_addr->sin6_port);
  printf("\n==================================\n");
}