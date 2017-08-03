
#include "secSock.h"
#include <stdio.h>


//test the ssl connection
void testConnection(secureConnection con) {

  int nBytes = 1;
  /*loop while connection is live*/
  while(nBytes!=0){
    char buffer[1024];
    nBytes = secureRead(con,buffer,1024);

    secureWrite(con,buffer,nBytes);
  }

  //clean up and end process
  closeConnection(con);
  exit(0);
}


int main() {
  if (startServer(testConnection) == -1) {
    perror("Server failed to start.");
  }
  return 0;
}
