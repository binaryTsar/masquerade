#include "secSock.h"
#include <stdio.h>

//test the connection is working
void testConection(secureConnection con) {

    //use connection
    char* buffer = (char*)"Client connection";
    printf("Sending: %s\n",buffer);
    secureWrite(con, buffer, 18);
    char buf2[18];
    secureRead(con, buf2, 18);
    printf("Received from server: %s\n\n",buf2);
}



int main() {

  //establish connection
  secureConnection con = makeConnection();

  if (con == NULL) {
    exit(0);
  }

  //test connection
  testConection(con);

  //clean up
  closeConnection(con);

  return 0;
}
