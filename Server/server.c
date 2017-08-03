
#include "secSock.h"
#include <stdio.h>


//test the ssl connection
void testConnection(secureConnection con) {

  //read source
  char src[20];
  secureRead(con, src, 20);
  printf("Source: %s\n", src);

  //read target
  char dst[20];
  secureRead(con, dst, 20);
  printf("Target: %s\n", dst);

  //read size
  char size;
  secureRead(con, &size, 20);
  printf("Size: %d bytes\n", size);

  //read data
  char* data = (char*)malloc(size);
  secureRead(con, data, size);
  printf("Data: %s\n", data);

  //server response

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
