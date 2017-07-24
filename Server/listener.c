#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT_NO 6500
#define SERVER_IP "127.0.0.1"
#define ADDRESS_SIZE sizeof(struct sockaddr_in)
#define BACKLOG 6


int makeSocket();
int startListening();
SSL_CTX* makeContext();


int main() {
  return startListening();
}

//test the ssl connection
void testConnection(int client, SSL_CTX* ctx) {


  SSL* ssl = SSL_new(ctx);
  if (SSL_set_fd(ssl, client) == 0) {
    perror("SSL Error\n");
    exit(0);
  }

  if (SSL_accept(ssl) <= 0) {
      ERR_print_errors_fp(stderr);
  }
  else {

      int nBytes = 1;
      /*loop while connection is live*/
      while(nBytes!=0){
        char buffer[1024];
        nBytes = SSL_read(ssl,buffer,1024);

        SSL_write(ssl,buffer,nBytes);
      }
  }

  //clean up and end process
  SSL_free(ssl);
  close(client);
  exit(0);
}


/*
 * Set up the server's listening loop
 * and start it on a new thread.
 */
int startListening(){

  int serverSocket = makeSocket();

  SSL_CTX* ctx = makeContext();


  /*loop to keep accepting new connections*/
  while(1){
    struct sockaddr_in serverStorage;
    unsigned int addr_size = ADDRESS_SIZE;
    int newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
    printf("server: got connection from %s port %d\n",
            inet_ntoa(serverStorage.sin_addr),
            ntohs(serverStorage.sin_port));

    /*fork a child process to handle the new connection*/
    if(!fork()){
      testConnection(newSocket, ctx);
    }
    /*if parent, close the socket and go back to listening new requests*/
    else{
      close(newSocket);
    }
  }


  return 0;
}



/*
 * Set up socket for server to listen on
 */
int makeSocket() {
  //set up address
  typedef struct sockaddr_in* socketAddr;
  socketAddr serverAddr = (socketAddr)calloc(ADDRESS_SIZE, 1);
  serverAddr->sin_family = AF_INET;
  serverAddr->sin_port = htons(PORT_NO);
  serverAddr->sin_addr.s_addr = inet_addr(SERVER_IP);

  //bind address to socket
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  bind(serverSocket, (struct sockaddr*) serverAddr, ADDRESS_SIZE);

  //start listening
  listen(serverSocket, BACKLOG);

  free(serverAddr);
  return serverSocket;
}


//create ssl context
SSL_CTX* makeContext() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = SSLv23_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
      perror("Unable to create SSL context");
      ERR_print_errors_fp(stderr);
      exit(EXIT_FAILURE);
    }
    SSL_CTX_set_ecdh_auto(ctx, 1);

    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
    }
    return ctx;
}
