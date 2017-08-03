CC=clang -std=c99 -o3 -Wall -Wextra
OPEN_SSL=-lssl -lcrypto

all: client

.PHONY: clean

clean:
	rm -f client ex *.o

client: client.o clientTest.o
	$(CC) client.c clientTest.o -o client $(OPEN_SSL)

client.o: client.c
	$(CC) client.c -o client.o -c

clientTest.o: clientTest.c
	$(CC) clientTest.c -o clientTest.o -c


