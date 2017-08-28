CC=gcc -std=c99 -o3 -Wall -Wextra
OPEN_SSL=-lssl -lcrypto

all: server

.PHONY: clean empty
clean: empty
	rm -f server .*.o

empty:
	rm user1/* user2/* user3/* user4/*
	cp user_empty/inbox user1
	cp user_empty/inbox user2
	cp user_empty/inbox user3
	cp user_empty/inbox user4

server: .listener.o .server.o
	$(CC) .listener.o .server.o -o server $(OPEN_SSL)

.listener.o: listener.c
	$(CC) listener.c -o .listener.o -c

.server.o: server.c
	$(CC) server.c -o .server.o -c
