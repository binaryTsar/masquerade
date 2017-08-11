#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "secSock.h"

#define PORT_NO 6500
#define SERVER_IP "127.0.0.1"
#define ADDRESS_SIZE sizeof(struct sockaddr_in)
#define BACKLOG 6

typedef struct secS {
  SSL_CTX* ctx;
  SSL* ssl;
  int connection;
}* secSock;

/*
 * Set up a new established connection
 */
 void initConnection(int socket, SSL_CTX* ctx, void (*serverCB) (secureConnection con)) {

   SSL* ssl = SSL_new(ctx);
   if (!ssl) {
     //abort connection
     SSL_CTX_free(ctx);
     exit(0);
   }
   if (SSL_set_fd(ssl, socket) == 0) {
     perror("SSL Error establishing connection.\n");
     SSL_CTX_free(ctx);
     exit(0);
   }
   int accept = SSL_accept(ssl);
   if (accept < 0) {
       ERR_print_errors_fp(stderr);
       SSL_CTX_free(ctx);
       exit(0);
   }
   else if (accept == 0) {
     //handshake failed safely
     SSL_CTX_free(ctx);
     exit(0);
   }

   secureConnection con = (secureConnection)malloc(sizeof(struct sConn));
   con->connection = socket;
   con->ctx = ctx;
   con->ssl = ssl;

   serverCB(con);
 }

 /*
  * Safely close a connection
  */
void closeConnection(secureConnection in) {
  secSock con = (secSock)in;
  SSL_free(con->ssl);
  SSL_CTX_free(con->ctx);
  close(con->connection);
  free(con);

}

/*
 * Create a socket for the server
 */
int makeSocket() {
  //set up address
  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT_NO);
  serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
  //set padding
  memset(serverAddr.sin_zero, 0, sizeof(serverAddr.sin_zero));

  //bind address to socket
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket < 0) {
    return -1;
  }

  if (bind(serverSocket, (struct sockaddr*) &serverAddr, ADDRESS_SIZE) == -1) {
    return -1;
  }

  //start listening
  if (listen(serverSocket, BACKLOG) == -1) {
    return -1;
  }

  return serverSocket;
}

/*
 * Create ssl context
 */
SSL_CTX* makeContext() {

    //create context
    SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) {
      perror("Unable to create SSL context");
      ERR_print_errors_fp(stderr);
      return NULL;
    }

    //configure ecdh
    SSL_CTX_set_ecdh_auto(ctx, 1);

    //load certificate
    int result = SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM);
    if (result <= 0) {
        perror("Error reading certificate");
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    //load key
    result = SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM);
    if (result <= 0 ) {
        perror("Error reading key");
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    return ctx;
}

/*
 * Start the server
 */
int startServer(void (*serverCB)(secureConnection con)){

  //create socket
  int serverSocket = makeSocket();
  if (serverSocket == -1) {
    perror("Unable to create socket.");
    return -1;
  }

  //create ssl context
  SSL_CTX* ctx = makeContext();
  if (!ctx) {
    close(serverSocket);
    return -1;
  }

  //listen for connections
  struct sockaddr_in serverStorage;
  unsigned int addr_size = ADDRESS_SIZE;
  while(1){
    int newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);

    if(!fork()){
      //child handles connection
      initConnection(newSocket, ctx, serverCB);
    }
    else{
      //parent does not need connection
      close(newSocket);
    }
  }
  return 0;
}

/*
 * Write bytes from a buffer
 */
int secureRead(secureConnection in, char* buffer, size_t bytes) {
  secSock con = (secSock) in;
  return SSL_read(con->ssl, buffer, bytes);
;
}

 /*
  * Read bytes to a buffer
  */
int secureWrite(secureConnection in, char* buffer, size_t bytes) {
  secSock con = (secSock) in;
  return SSL_write(con->ssl, buffer, bytes);
}
