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

//my socket code
int create_socket() {
  //set up address
  typedef struct sockaddr_in* socketAddr;
  socketAddr serverAddr = (socketAddr)calloc(ADDRESS_SIZE, 1);
  serverAddr->sin_family = AF_INET;
  serverAddr->sin_port = htons(PORT_NO);
  serverAddr->sin_addr.s_addr = inet_addr(SERVER_IP);

  //bind address to socket
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket < 0) {
    //free(serverAddr);
    //return -1;
  }

  if (!bind(serverSocket, (struct sockaddr*) serverAddr, ADDRESS_SIZE)) {
    //free(serverAddr);
    //return -1;
  }

  free(serverAddr);

  //start listening
  if (!listen(serverSocket, BACKLOG)) {
    //return -1;
  }

  return serverSocket;
}


//create ssl context
SSL_CTX *create_context()
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = SSLv23_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
      perror("Unable to create SSL context");
      ERR_print_errors_fp(stderr);
      exit(EXIT_FAILURE);
    }

    return ctx;
}

//config context
void configure_context(SSL_CTX *ctx)
{
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
}

//run
int main()
{

    printf("1\n");
    //set context
    const SSL_METHOD* method = SSLv23_server_method();
    SSL_CTX* ctx = SSL_CTX_new(method);
    //SSL_CTX* ctx = create_context();
    printf("2\n");

    //config context

    //set algorithm preferance - ???
    SSL_CTX_set_ecdh_auto(ctx, 1);
    /* Set the key and cert */
    SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM);
    //configure_context(ctx);
    printf("3\n");

    //set socket
    int sock = create_socket();

    /* Handle connections */
    printf("4\n");
    while(1) {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        SSL *ssl;
        const char reply[] = "test\n";

        int client = accept(sock, (struct sockaddr*)&addr, &len);

        if (client < 0) {
            perror("Unable to accept");
            exit(EXIT_FAILURE);
        }

        ssl = SSL_new(ctx);
        if (SSL_set_fd(ssl, client) == 0) {
          perror("SSL Error\n");
          exit(0);
        }


        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
        }
        else {
            SSL_write(ssl, reply, strlen(reply));
        }

        SSL_free(ssl);
        close(client);
        printf("a\n");
    }

    close(sock);
    SSL_CTX_free(ctx);

}
