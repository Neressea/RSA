all: proxy_v1 proxy_v2 proxy_v3 clear

ok: proxy_v1 proxy_v2 clear

proxy_v1: proxy_v1.o utils/util.o utils/socketutil.o
	gcc -Wall -Werror -o proxy_v1 proxy_v1.o utils/util.o utils/socketutil.o

proxy_v1.o:	proxy_v1.c ./utils/*.h
	gcc -Wall -Werror -c proxy_v1.c ./utils/*.h

proxy_v2: proxy_v2.o utils/util.o utils/socketutil.o
	gcc -Wall -Werror -o proxy_v2 proxy_v2.o utils/util.o utils/socketutil.o

proxy_v2.o:	proxy_v2.c ./utils/*.h
	gcc -Wall -Werror -c proxy_v2.c ./utils/*.h

proxy_v3: proxy_v3.o utils/util.o utils/socketutil.o utils/secureutil.o
	gcc -Wall -Werror -o proxy_v3 proxy_v3.o utils/util.o utils/socketutil.o utils/secureutil.o  -lssl -lcrypto 

proxy_v3.o:	proxy_v3.c ./utils/*.h
	gcc -Wall -Werror -c proxy_v3.c utils/*.h -lssl -lcrypto  

socketutil.o: socketutil.c utils/*.h
	gcc -Wall -Werror -c socketutil.c utils/*.h

util.o: utils/util.c utils/*.h
	gcc -Wall -Werror -c utils/util.c utils/*.h

secureutil.o: utils/secureutil.o utils/*.h
	gcc -Wall -Werror -c utils/secureutil.c utils/*.h -lssl -lcrypto  

clear:
	rm -rf *.o utils/*.o *.gch utils/*.gch
