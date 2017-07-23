#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

#define PORT_NO 6500
#define NAME "128.0.0.1"
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
  serverAddr->sin_addr.s_addr = inet_addr(NAME);

  //connect from new socket
  int clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  connect(clientSocket, (struct sockaddr*) serverAddr, ADDRESS_SIZE);
  free(serverAddr);
  return clientSocket;
}

//init
void init_openssl() {
  SSL_load_error_strings();
  OpenSSL_add_all_algorithms();
}

int main() {

  //set up
  init_openssl();

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

  //
  char* servername = (char*)"google.com";
  X509_VERIFY_PARAM* param = SSL_get0_param(ssl);

  /* Enable automatic hostname checks */
  X509_VERIFY_PARAM_set_hostflags(param, 0);
  X509_VERIFY_PARAM_set1_host(param, servername, 0);

  /* Configure a non-zero callback if desired */
  SSL_set_verify(ssl, SSL_VERIFY_PEER, 0);
  //

  if (SSL_connect(ssl)  < 0) exit(0);

  printf("Cipher: %s\n", SSL_get_cipher_name(ssl));

  //get certificate info
  X509_NAME *certname = NULL;
  X509* cert = SSL_get_peer_certificate(ssl);

  //read cert
  certname = X509_NAME_new();
  certname = X509_get_subject_name(cert);

  //show cert
  printf("Displaying the certificate subject data:\n");
  //char * X509_NAME_oneline(X509_NAME *a,char *buf,int size);
  char* name = (char*)malloc(40);
  X509_NAME_oneline(certname, name, 35);
  printf("Cert Name: %s\n", name+1);
  free(name);

  //read data from server to test connection
  /*
  char* b = (char*)calloc(20, 1);
  SSL_read(ssl, b, 20);
  printf("Read from server: %s", b);
  */
  //clean up
  SSL_free(ssl);
  close(server);
  X509_free(cert);
  SSL_CTX_free(ctx);
  printf("Finished SSL/TLS connection with server.\n");
  return(0);
}
