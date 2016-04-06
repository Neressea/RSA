#!/bin/bash

#On compile le serveur
src='proxy.c'
dest="${src/.c/}"
gcc -Wall $src ./util/util.c -o $dest