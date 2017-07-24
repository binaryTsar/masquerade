CC=clang -std=c99 -o3 -Wall -Wextra
OPEN_SSL=-lssl -lcrypto

all: client ex

clean:
	rm client ex

client: client.c
	$(CC) client.c -o client $(OPEN_SSL)

ex: openSSL.c	
	$(CC) openSSL.c -o ex $(OPEN_SSL)

