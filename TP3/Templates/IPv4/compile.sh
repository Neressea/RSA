#!/bin/bash

if [ $# != 1 ] ; then
	echo "Usage : compile.sh file.c"
	exit 1
fi
org=$1
dest="${org/.c/}"
gcc -Wall $1 util.c -o $dest