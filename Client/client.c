#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include <openssl/ssl.h>

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
 * Create ssl context
 */
void* makeContext(const char** certs) {

    //create context
    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
      perror("Unable to create SSL context");
      return NULL;
    }

    //configure verify
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT,NULL);
    if (SSL_CTX_load_verify_locations(ctx, certs[0], NULL) == 0) {
      perror("Error reading root certificate");
      SSL_CTX_free(ctx);
      return NULL;
    }

    //load certificate
    int result = SSL_CTX_use_certificate_file(ctx, certs[1], SSL_FILETYPE_PEM);
    if (result <= 0) {
        perror("Error reading client certificate");
        SSL_CTX_free(ctx);
        return NULL;
    }

    //load key
    result = SSL_CTX_use_PrivateKey_file(ctx, certs[2], SSL_FILETYPE_PEM);
    if (result <= 0 ) {
        perror("Error reading key");
        SSL_CTX_free(ctx);
        return NULL;
    }

    return ctx;
}

/*
 * Free context
 */
 void freeContext(void* clientCtx) {
   SSL_CTX_free((SSL_CTX*)clientCtx);
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
     return 0;
   }
   if (connect(connection, (struct sockaddr*) &serverAddr, ADDRESS_SIZE)) {
     perror("Failed to establish connection");
     return 0;
   }

   return connection;
 }


/*
 * Establish a tls connection
 */
secureConnection makeConnection(void* clientCtx){
  secSock con = (secSock)calloc(sizeof(struct secS), 1);

  //set up socket
  con->connection = makeSocketConnection();
  if (!con->connection) {
    closeConnection(con);
    return NULL;
  }

  //load context
  con->ctx = (SSL_CTX*)clientCtx;

  //create ssl object
  con->ssl = SSL_new(con->ctx);
  if (!(con->ssl)) {
    perror("Failed to create ssl object");
    closeConnection(con);
    return NULL;
  }

  //connect ssl to connectionn fd
  if (!SSL_set_fd(con->ssl, con->connection)) {
    closeConnection(con);
    perror("Failed to link ssl to socket");
    return NULL;
  }

  //make connection
  int err = SSL_connect(con->ssl);
  if (err <= 0) {
    closeConnection(con);
    perror((err == 0)? "Handshake failed" : "SSL error during handshake");
    return NULL;
  }

  return (secureConnection)con;
}

/*
 * Close a connection, but don't free context
 */
void closeConnection(void* in) {
  secSock con = (secSock) in;
  SSL_free(con->ssl);
  close(con->connection);
  free(con);
}

/*
 * Write bytes from a buffer
 */
int secureRead(const secureConnection in, void* buffer, unsigned int bytes) {
  return SSL_read(((secSock) in)->ssl, buffer, bytes);
}

 /*
  * Read bytes to a buffer
  */
int secureWrite(const secureConnection in, const void* buffer, unsigned int bytes) {
  return SSL_write(((secSock) in)->ssl, buffer, bytes);
}
