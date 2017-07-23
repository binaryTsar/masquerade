#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT_NO 6500
#define ADDRESS_SIZE sizeof(struct sockaddr_in)
#define BACKLOG 6

/*
 * Set up the server's listening loop
 * and start it on a new thread.
 */
int startListening(){

  //set up address
  typedef struct sockaddr_in* socketAddr;
  socketAddr serverAddr = (socketAddr)calloc(ADDRESS_SIZE, 1);
  serverAddr->sin_family = AF_INET;
  serverAddr->sin_port = htons(PORT_NO);
  serverAddr->sin_addr.s_addr = inet_addr("127.0.0.1");

  //bind address to socket
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  bind(serverSocket, (struct sockaddr*) serverAddr, ADDRESS_SIZE);

  //start listening
  listen(serverSocket, BACKLOG);

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
      int nBytes = 1;
      /*loop while connection is live*/
      while(nBytes!=0){
        char buffer[1024];
        nBytes = recv(newSocket,buffer,1024,0);

        send(newSocket,buffer,nBytes,0);
      }
      close(newSocket);
      exit(0);
    }
    /*if parent, close the socket and go back to listening new requests*/
    else{
      close(newSocket);
    }
  }

  free(serverAddr);

  return 0;
}



int main() {
  return startListening();
}
