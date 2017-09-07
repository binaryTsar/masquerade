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
 * Handshake a connection and delegate to callback
 */
 void initConnection(int socket, SSL_CTX* ctx, void (*serverCB) (secureConnection con)) {

   //create ssl object
   SSL* ssl = SSL_new(ctx);
   if (!ssl) {
     perror("Failed to create SSL object");
     close(socket);
     exit(0);
   }

   //bind ssl to socket
   if (SSL_set_fd(ssl, socket) == 0) {
     perror("SSL Error establishing connection.\n");
     SSL_free(ssl);
     SSL_CTX_free(ctx);
     close(socket);
     exit(0);
   }


   //accept connection
   int accept = SSL_accept(ssl);
   if (accept < 0) {
       perror("Failed to accept SSL connection");
       SSL_free(ssl);
       SSL_CTX_free(ctx);
       close(socket);
       exit(0);
   }
   else if (accept == 0) {
     //handshake failed safely
     SSL_free(ssl);
     SSL_CTX_free(ctx);
     close(socket);
     exit(0);
   }

   //store in struct
   secureConnection con = (secureConnection)malloc(sizeof(struct sConn));
   con->connection = socket;
   con->ctx = ctx;
   con->ssl = ssl;

   //callback
   serverCB(con);
 }

 /*
  * Safely close a connection at end of life
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
      return NULL;
    }

    //load root certificate
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT,NULL);
    if (SSL_CTX_load_verify_locations(ctx, "../root.pem", NULL) == 0) {
      perror("Error loading root certificate");
      SSL_CTX_free(ctx);
      return NULL;
    }

    //load server certificate
    int result = SSL_CTX_use_certificate_file(ctx, "server1.pem", SSL_FILETYPE_PEM);
    if (result <= 0) {
        perror("Error reading certificate");
        SSL_CTX_free(ctx);
        return NULL;
    }

    //load key
    result = SSL_CTX_use_PrivateKey_file(ctx, "server1.key", SSL_FILETYPE_PEM);
    if (result <= 0 ) {
        perror("Error reading server key");
        SSL_CTX_free(ctx);
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
    perror("Unable to make SSL context");
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
int secureRead(const secureConnection in, char* buffer, size_t bytes) {
  secSock con = (secSock) in;
  return SSL_read(con->ssl, buffer, bytes);
;
}

 /*
  * Read bytes to a buffer
  */
int secureWrite(const secureConnection in, const char* buffer, size_t bytes) {
  secSock con = (secSock) in;
  return SSL_write(con->ssl, buffer, bytes);
}

/*
 * Get a client's name from their certificate
 */
 int getName(secureConnection in, char* buffer, size_t bytes) {
   X509* c =  SSL_get_peer_certificate((SSL*)in->ssl);
   X509_NAME* sname = X509_get_subject_name(c);

   //DEPRECATED
   X509_NAME_get_text_by_NID(sname,NID_commonName, buffer, bytes);
   return 0;
 }
