
#include "secSock.h"
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void readSC(secureConnection con, char* src)  {


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

  //write file to inbox

  //open file
  char path[30];
  sprintf(path, "%s/inbox.txt", dst);
  int target_inbox = open(path, O_APPEND|O_CREAT, S_IRUSR|S_IWUSR);
  if (target_inbox < 0) {
	  perror("Failed to write to inbox.");
	  exit(0);
  }

  //write sender
  int len = 20;
  //TODO check return value type
  if (write(target_inbox, src, len) != len) {
	  perror("Unable to write to file.");
	  exit(0);
  }

  //write size
  //TODO check return value type
  len = 1;
  if (write(target_inbox, &size, len) != len) {
	  perror("Unable to write to file.");
	  exit(0);
  }

  //write data
  len = size;
  if (write(target_inbox, data, len) != len) {
	  perror("Unable to write to file.");
	  exit(0);
  }
}

void writeSC(secureConnection con, char* src) {

    //server response
    char path[30];
    sprintf(path, "%s/inbox.txt", src);
    int target_inbox = open(path, O_APPEND|O_CREAT, S_IRUSR|S_IWUSR);
    if (target_inbox < 0) {
  	  perror("Failed to open inbox.");
  	  exit(0);
    }

    //read sender
    int len = 20;
    char sender[20];
    //TODO check return value type
    if (read(target_inbox, sender, len) != len) {
  	  perror("Unable to read from file.");
  	  exit(0);
    }

    //read size
    char msgSize;
    len = 1;
    //TODO check return value type
    if (read(target_inbox, &msgSize, len) != len) {
  	  perror("Unable to read from file.");
  	  exit(0);
    }

    //read data
    len = msgSize;
    char* msg = (char*)malloc(len);
    if (read(target_inbox, msg, len) != len) {
  	  perror("Unable to read from file.");
  	  exit(0);
    }

      //send sender
      secureWrite(con, sender, 20);

  	//send size
      secureWrite(con, &msgSize, 1);

  	//send data
      secureWrite(con, msg, len);

}

//test the ssl connection
void testConnection(secureConnection con) {
  //read source
  char src[20];
  secureRead(con, src, 20);
  printf("Source: %s\n", src);

  readSC(con, src);
  writeSC(con, src);




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
