CC=clang -std=c99 -o3 -Wall -Wextra
OPEN_SSL=-lssl -lcrypto 

all: client

.PHONY: clean
clean:
	rm -f client *.o

client: client.o clientTest.o parser.o
	$(CC) client.c clientTest.o parser.o -o client $(OPEN_SSL)

client.o: client.c
	$(CC) client.c -o client.o -c

clientTest.o: clientTest.c
	$(CC) clientTest.c -o clientTest.o -c

parser.o: parser.c
	$(CC) parser.c -o parser.o -c
