#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>


#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

//google for testing
#define PORT_NO 443
#define SERVER_IP "172.217.3.99"

//#define PORT_NO 6500
//#define SERVER_IP "127.0.0.1"
#define ADDRESS_SIZE sizeof(struct sockaddr_in)

//Establish simple connection to server
int makeSocketConnection();
int CB(int a, X509_STORE_CTX* b);

//test the connection is working
void testConection(SSL* ssl) {

    //use connection
    char* buffer = (char*)"Client connection";
    printf("Sending: %s\n",buffer);
    int nBytes = 18;
    SSL_write(ssl, buffer, nBytes);
    char b[20];
    SSL_read(ssl, b, 20);
    printf("Received from server: %s\n\n",b);
}

/*
 * Main method
 */
int main(){

  //set up connection
  int connection = makeSocketConnection();

  //set up ssl context
  SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
  if (!ctx) {
    close(connection);
    perror("Failed to create context.\n");
    exit(0);
  }

  //create ssl object
  SSL* ssl = SSL_new(ctx);
  if (!ssl) {
    SSL_CTX_free(ctx);
    close(connection);
    perror("Failed to create ssl object.\n");
    exit(0);
  }

  //configure ssl
  SSL_set_verify(ssl, SSL_VERIFY_PEER, *CB);

  //connect ssl to connectionn fd
  if (!SSL_set_fd(ssl, connection)) {
    SSL_CTX_free(ctx);
    SSL_free(ssl);
    close(connection);
    perror("Failed to link ssl to socket.\n");
    exit(0);
  }

  //make connection
  int err = SSL_connect(ssl);
  if (err == 0) {
    SSL_CTX_free(ctx);
    SSL_free(ssl);
    close(connection);
    perror("Handshake failed.\n");
    exit(0);
  }
  if (err < 0) {
    SSL_CTX_free(ctx);
    SSL_free(ssl);
    close(connection);
    perror("SSL error during handshake.\n");
    exit(0);
  }

  //test connection
  testConection(ssl);

  //clean up
  SSL_CTX_free(ctx);  //safe
  SSL_free(ssl);      //safe
  close(connection);  //could cause error

  return 0;
}

/*
 * Create a simple socket connection
 */
int makeSocketConnection() {

  //set server address
  typedef struct sockaddr_in* socketAddr;
  socketAddr serverAddr = (socketAddr)calloc(ADDRESS_SIZE, 1);
  serverAddr->sin_family = AF_INET;
  serverAddr->sin_port = htons(PORT_NO);
  serverAddr->sin_addr.s_addr = inet_addr(SERVER_IP);

  //connect to server
  int connection = socket(PF_INET, SOCK_STREAM, 0);
  if (!connection) {
    free(serverAddr);
    perror("Failed to create socket");
    exit(0);
  }
  if (connect(connection, (struct sockaddr*) serverAddr, ADDRESS_SIZE) == -1) {
    free(serverAddr);
    perror("Failed to create connection");
    exit(0);
  }

  //free address struct and return connection
  free(serverAddr);
  return connection;
}



//callback for verification
int CB(int norm, X509_STORE_CTX* ctx) {

  //who knows???

  return 1;
}
