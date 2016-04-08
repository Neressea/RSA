all: proxy_v1 proxy_v2 clear

proxy_v1: proxy_v1.o utils/util.o utils/socketutil.o
	gcc -o proxy_v1 proxy_v1.o utils/util.o utils/socketutil.o

proxy_v1.o:	proxy_v1.c ./utils/*.h
	gcc -c proxy_v1.c ./utils/*.h

proxy_v2: proxy_v2.o utils/util.o utils/socketutil.o
	gcc -o proxy_v2 proxy_v2.o utils/util.o utils/socketutil.o

proxy_v2.o:	proxy_v2.c ./utils/*.h
	gcc -c proxy_v2.c ./utils/*.h

socketutil.o: socketutil.c utils/*.h
	gcc -c socketutil.c utils/*.h

util.o: utils/util.c utils/*.h
	gcc -c utils/util.c utils/*.h

clear:
	rm -rf *.o utils/*.o *.gch utils/*.gch
