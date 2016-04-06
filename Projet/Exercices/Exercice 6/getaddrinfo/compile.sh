#!/bin/bash

#On compile le serveur
src='serverTCPIPv6_getaddrinfo.c'
dest="${src/.c/}"
gcc -Wall $src ../util/util.c -o $dest

#On compile le client
src='clientTCPIPv6_getaddrinfo.c'
dest="${src/.c/}"
gcc -Wall $src ../util/util.c -o $dest