#include "util.h"

void usage(){
  perror("Il faut renseigner le numéro de port pour les socket !\n");
}

void searchTypeRequest(char entete [], char type[]){
  int i = 0;

  //On récupère la commande
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