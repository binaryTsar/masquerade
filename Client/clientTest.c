#include "secSock.h"
#include <stdio.h>
#include <string.h>

void writeSC(secureConnection con) {

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

void readSC(secureConnection con) {


    printf("Received mail:\n");
  	//read source
    char src[20];
    secureRead(con, src, 20);
    printf("Sender: %s\n", src);

    //read size
    char size;
    secureRead(con, &size, 20);
    printf("Size: %d bytes\n", size);

    //read data
    char* data = (char*)malloc(size);
    secureRead(con, data, size);
    printf("Data: %s\n", data);
}

//test the connection is working
void testConection(secureConnection con) {

  writeSC(con);
  readSC(con);



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
