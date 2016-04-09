!! Pour compiler la version 3 du proxy, OpenSSL doit être installé !!

Trois versions du proxy :
- v1 : un seul client autorisé
- v2 : multiclients
- v3 : HTTPS

La v3 a été commencée mais n'est pas encore fonctionnelle pour la partie HTTPS.

Pour compiler les 3 versions et nettoyer les fichiers intermédiaires :
make all

pour compiler les deux versions fonctionnelles et nettoyer les fichiers intermédiaires :
make ok

Pour lancer une version du proxy :
./proxy_vX NUM_PORT      où X est le numéro de la version et NUM_PORT une constate entière représentant le numéro du port.