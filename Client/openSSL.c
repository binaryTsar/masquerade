#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

#define PORT_NO 6500
#define SERVER_IP "127.0.0.1"
#define ADDRESS_SIZE sizeof(struct sockaddr_in)

/*
 * My socket code
 */
int create_socket() {
  //set server address
  typedef struct sockaddr_in* socketAddr;
  socketAddr serverAddr = (socketAddr)calloc(ADDRESS_SIZE, 1);
  serverAddr->sin_family = AF_INET;
  serverAddr->sin_port = htons(PORT_NO);
  serverAddr->sin_addr.s_addr = inet_addr(SERVER_IP);

  //connect from new socket
  int connection = socket(PF_INET, SOCK_STREAM, 0);
  connect(connection, (struct sockaddr*) serverAddr, ADDRESS_SIZE);
  free(serverAddr);
  return connection;
}


int main() {

  //TLS_method is used to select the version use. Tinker with this
  //and context configuring to select latest tls version

  //set context and create ssl connection
  SSL_CTX *ctx = SSL_CTX_new(TLS_method());

  SSL *ssl = SSL_new(ctx);

  //create socket
  int server = create_socket();

  //attach socket to ssl connection
  SSL_set_fd(ssl, server);
  //make connection
  SSL_connect(ssl);

  printf("Cipher: %s\n", SSL_get_cipher_name(ssl));

  //get certificate info

  /*
  X509_NAME *certname = NULL;
  X509* cert = SSL_get_peer_certificate(ssl);

  //read cert
  certname = X509_NAME_new();
  X509_NAME* subject = X509_get_subject_name(cert);


  //show cert
  printf("Displaying the certificate subject data:\n");
  char* name = (char*)malloc(40);
  X509_NAME_oneline(subject, name, 35);
  printf("Cert Name: %s\n", name+1);

  free(name);
  X509_NAME_free(certname);
  X509_NAME_free(subject);
  X509_free(cert);

  */

  //read data from server to test connection

  char* b = (char*)calloc(20, 1);
  SSL_read(ssl, b, 20);
  printf("Read from server: %s", b);
  free(b);

  //clean up
  SSL_free(ssl);
  close(server);
  SSL_CTX_free(ctx);

  printf("Finished SSL/TLS connection with server.\n");
  return(0);
}
