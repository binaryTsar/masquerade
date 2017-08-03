#include "secSock.h"
#include <stdio.h>
#include <string.h>

//test the connection is working
void testConection(secureConnection con) {
    char* id = (char*)"user1";

    //use connection
    char* name = (char*)calloc(20, 1);
    for (unsigned int i = 0; i < strlen(id) + 1; i++) {
      name[i] = id[i];
    }
    //source and target are the same
    secureWrite(con, name, 20);
    secureWrite(con, name, 20);

    char size = 26;
    secureWrite(con, &size, 1);

    char* data = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    secureWrite(con, data, 26);

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
