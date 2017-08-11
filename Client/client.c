#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

#include "secSock.h"

#define PORT_NO 6500
#define SERVER_IP "127.0.0.1"
#define ADDRESS_SIZE sizeof(struct sockaddr_in)

typedef struct secS {
  SSL_CTX* ctx;
  SSL* ssl;
  int connection;
}* secSock;

/*
 * Close a connection and tidy up
 */
void closeConnection(void* in) {
  secSock con = (secSock) in;
  SSL_CTX_free(con->ctx);
  SSL_free(con->ssl);
  close(con->connection);
  free(con);
}

/*
 * Create a simple socket connection
 */
int makeSocketConnection() {

  //set server address
  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT_NO);
  serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
  //set padding
  memset(serverAddr.sin_zero, 0, sizeof(serverAddr.sin_zero));

  //connect to server
  int connection = socket(PF_INET, SOCK_STREAM, 0);
  if (!connection) {
    perror("Failed to create socket");
    return -1;
  }
  if (connect(connection, (struct sockaddr*) &serverAddr, ADDRESS_SIZE) == -1) {
    perror("Failed to establish connection");
    return -1;
  }

  return connection;
}

/*
 * Establish a tls connection
 */
secureConnection makeConnection(){
  secSock con = (secSock)malloc(sizeof(struct secS));
  con->ssl = NULL;
  con->ctx = NULL;

  //set up connection
  con->connection = makeSocketConnection();
  if (con->connection < 0) {
    closeConnection(con);
    return NULL;
  }

  //set up ssl context
  con->ctx = SSL_CTX_new(TLS_client_method());
  if (!(con->ctx)) {
    closeConnection(con);
    perror("Failed to create context.\n");
    return NULL;
  }

  //create ssl object
  con->ssl = SSL_new(con->ctx);
  if (!(con->ssl)) {
    closeConnection(con);
    perror("Failed to create ssl object.\n");
    return NULL;
  }

  //connect ssl to connectionn fd
  if (!SSL_set_fd(con->ssl, con->connection)) {
    closeConnection(con);
    perror("Failed to link ssl to socket.\n");
    return NULL;
  }

  //make connection
  int err = SSL_connect(con->ssl);
  if (err <= 0) {
    closeConnection(con);
    perror((err == 0)? "Handshake failed.\n" : "SSL error during handshake.\n");
    return NULL;
  }

  return (secureConnection)con;
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
