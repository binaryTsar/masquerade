CC=gcc -std=c99 -o3 -Wall -Wextra
OPEN_SSL=-lssl -lcrypto

all: client

.PHONY: clean
clean:
	rm -f client .*.o fifo* *.gch

client: .client.o .clientTest.o .parser.o .pipeUI.o
	$(CC) .client.o .clientTest.o .parser.o .pipeUI.o -o client $(OPEN_SSL)

.client.o: client.c
	$(CC) client.c -o .client.o -c

.clientTest.o: clientTest.c
	$(CC) clientTest.c -o .clientTest.o -c

.parser.o: parser.c
	$(CC) parser.c -o .parser.o -c

.pipeUI.o: pipeUI.c
	$(CC) pipeUI.c -o .pipeUI.o -c
