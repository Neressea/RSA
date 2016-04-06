#!/bin/bash

#On compile le serveur
src='serverTCPStructureDonnees.c'
dest="${src/.c/}"
gcc -Wall $src ../util/util.c -o $dest

#On compile le client
src='clientTCPStructureDonnees.c'
dest="${src/.c/}"
gcc -Wall $src ../util/util.c -o $dest