CC=clang -std=c99 -o3 -Wall -Wextra
OPEN_SSL=-lssl -lcrypto

all: listener ex

clean:
	rm listener ex

listener: listener.c
	$(CC) listener.c -o listener $(OPEN_SSL)

ex: openSSL.c
	$(CC) openSSL.c -o ex $(OPEN_SSL)
