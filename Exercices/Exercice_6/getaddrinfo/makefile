compil: serverTCP clientTCP

serverTCP: serverTCP.o
	gcc -o serverTCP ./bin/serverTCP.o

clientTCP: clientTCP.o
	gcc -o clientTCP ./bin/clientTCP.o

serverTCP.o: bin serverTCP.c
	gcc -c -o ./bin/serverTCP.o serverTCP.c

clientTCP.o: bin clientTCP.c
	gcc -c -o ./bin/clientTCP.o clientTCP.c 

bin: 
	mkdir bin

clear:
	rm clientTCP serverTCP ./bin/*.o *~
