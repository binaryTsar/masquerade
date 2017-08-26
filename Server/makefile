CC=clang -std=c99 -o3 -Wall -Wextra
OPEN_SSL=-lssl -lcrypto

all: server echo

.PHONY: clean
clean:
	rm -f echo server *.o

server: listener.o server.o
	$(CC) listener.o server.o -o server $(OPEN_SSL)

listener.o: listener.c
	$(CC) listener.c -o listener.o -c

server.o: server.c
	$(CC) server.c -o server.o -c
