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

void searchRequest(char entete[], char requete[]){
  int i=0;

  //On passe la commande
  while(entete[i] != ' ' && i < MAXENTETE)i++;

  int newi = i;
  int reqi = 0;

  //On élimine le http://
  while(newi - i <= 7) newi++;

  while(entete[newi] != ' ' && i < MAXHOST){
    requete[reqi++] = entete[newi];
    newi++;
  }

  requete[reqi] = '\0';
}

void showMyIp(struct addrinfo *res){
  struct sockaddr_in6 *my_addr = (struct sockaddr_in6 *)res->ai_addr;
  char ip[150];
  inet_ntop(my_addr->sin6_family, my_addr->sin6_addr.s6_addr, ip, sizeof(my_addr->sin6_addr.s6_addr));
  printf("\n==================================\n");
  printf("Lancement du serveur sur l'adresse %s sur le port %d \n", ip, my_addr->sin6_port);
  printf("\n==================================\n");
}

void addRequestLog(int socketClient, char type_req[], char request[]){
  FILE *log_requests;

  //On récupère les infos sur le client à partir du fd
  struct sockaddr_in6 addr;
  socklen_t addr_size = sizeof(struct sockaddr_in);
  int res = getpeername(socketClient, (struct sockaddr *)&addr, &addr_size);

  char ip[150];
  inet_ntop(addr.sin6_family, addr.sin6_addr.s6_addr, ip, sizeof(addr.sin6_addr.s6_addr));

  if((log_requests = fopen("./logs/log_requests", "a+")) == NULL){
    perror("Erreur lors de l'ouverture du fichier logs/log_requests");
    exit(1);
  }

  fprintf(log_requests, "%s %d %s %s\n", ip, addr.sin6_port, type_req, request);

  if(fclose(log_requests) != 0){
    perror("Erreur lors de la fermeture de logs/log_requests");
  }
}

void addVisitLog(char ip[]){
  //On chercher si le client se trouve déjà dans le fichier
  char current_ip[50];
  int nb_visits, cursor = 1, written = 0;
  //Des variables globales pour les pointeurs de fichiers
  FILE *log_visits, *tmp;

  //On replace le curseur au début du fichier
  if((log_visits = fopen("./logs/log_visits", "r+")) == NULL){
    perror("Erreur lors de l'ouverture du fichier logs/log_visits");
    exit(1);
  }

  if((tmp = fopen("./logs/tmp", "w")) == NULL){
    perror("Erreur lors de l'ouverture du fichier logs/tmp");
    exit(1);
  }

  while(cursor != EOF){
    cursor = fscanf(log_visits, "%s %d\n", current_ip, &nb_visits);
    if(strcmp(ip, current_ip) == 0 && !written){
      //On incrémente le compteur et on réécrit sur la ligne
      fprintf(tmp, "%s %d\n", ip, ++nb_visits);
      written = 1;
    }else if(strcmp(ip, current_ip)){
      //On ne retouche pas la ligne
      fprintf(tmp, "%s %d\n", ip, nb_visits);
    }
  }

  //Si on a atteint le fichier jusqu'au bout, on rajoute l'adresse
  if(!written){
    printf("COUCOU\n");
    fprintf(tmp, "%s %d\n", ip, 1);
  }

  if(fclose(log_visits) != 0){
    perror("Erreur lors de la fermeture de logs/log_visits");
  }
  if(fclose(tmp) != 0){
    perror("Erreur lors de la fermeture de logs/tmp");
  }

  remove("./logs/log_visits");
  rename("./logs/tmp", "./logs/log_visits");
}