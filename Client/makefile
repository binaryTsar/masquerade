CC=clang -std=c99 -o3 -Wall -Wextra
OPEN_SSL=-lssl -lcrypto

all: client

clean:
	rm -f client ex *.o

client: client.o verify.o
	$(CC) client.c verify.o -o client $(OPEN_SSL)

client.o: client.c
	$(CC) client.c -o client.o -c

verify.o: verify.c
	$(CC) verify.c -o verify.o -c


ex: openSSL.c	
	$(CC) openSSL.c -o ex $(OPEN_SSL)

