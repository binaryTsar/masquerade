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
  if (!ssl) {
    exit(0);
  }
  if (SSL_set_fd(ssl, client) == 0) {
    perror("SSL Error\n");
    exit(0);
  }

  if (SSL_accept(ssl) <= 0) {
      ERR_print_errors_fp(stderr);
      exit(0);
  }

  int nBytes = 1;
  /*loop while connection is live*/
  while(nBytes!=0){
    char buffer[1024];
    nBytes = SSL_read(ssl,buffer,1024);

    SSL_write(ssl,buffer,nBytes);
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
  if (serverSocket == -1) {
    exit(0);
  }

  SSL_CTX* ctx = makeContext();
  if (!ctx) {
    close(serverSocket);
    exit(0);
  }

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
 * Return -1 on failure.
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
  if (serverSocket < 0) {
    free(serverAddr);
    return -1;
  }

  if (bind(serverSocket, (struct sockaddr*) serverAddr, ADDRESS_SIZE) == -1) {
    free(serverAddr);
    return -1;
  }

  free(serverAddr);

  //start listening
  if (listen(serverSocket, BACKLOG) == -1) {
    return -1;
  }

  return serverSocket;
}

/*
 * Create ssl context
 * Return NULL on failure
 */
SSL_CTX* makeContext() {

    //create context
    SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) {
      perror("Unable to create SSL context");
      ERR_print_errors_fp(stderr);
      return NULL;
    }

    //configure
    SSL_CTX_set_ecdh_auto(ctx, 1);

    //certificate
    int result = SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM);
    if (result <= 0) {
        perror("Error reading certificate");
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    //key
    result = SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM);
    if (result <= 0 ) {
        perror("Error reading key");
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    return ctx;
}
