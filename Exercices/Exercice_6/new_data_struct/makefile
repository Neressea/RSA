compil: serverTCP clientTCP

serverTCP: serverTCP.o util.o
	gcc -o serverTCP serverTCP.o util.o

clientTCP: clientTCP.o util.o
	gcc -o clientTCP clientTCP.o util.o

serverTCP.o: serverTCP.c util.c
	gcc -c serverTCP.c

clientTCP.o: clientTCP.c util.c
	gcc -c clientTCP.c

util.o: util.c
	gcc -c util.c

clear:
	rm clientTCP serverTCP *.o *~

	
