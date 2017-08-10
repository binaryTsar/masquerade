
#include "secSock.h"
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * Read data from client. Return size.
 */
int readSC(secureConnection con, char* src)  {

  //read target
  char dst[20];
  secureRead(con, dst, 20);
  printf("Target: %s\n", dst);

  //read size
  unsigned int size;
  char buffer[4];
  secureRead(con, buffer, 20);
  memcpy(&size, buffer, 4);
  printf("Size: %u bytes\n", size);

  //read data
  char data[size];
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

  return size+44;
}

void writeSC(secureConnection con, char* src, unsigned int size) {

  //server response
  char path[35];
  sprintf(path, "%s/inbox.txt", src);
  int target_inbox = open(path, O_APPEND|O_CREAT, S_IRUSR|S_IWUSR);
  if (target_inbox < 0) {
    perror("Failed to open inbox.");
    exit(0);
  }

  //get sender
  int len = 20;
  char sender[20];
  //TODO check return value type
  if (read(target_inbox, sender, len) != len) {
    perror("Unable to read from file.");
    exit(0);
  }
  //padding
  for (unsigned int i = strlen(sender)+1; i < 20; i++) {
    sender[i] = '\0';
  }

  //send sender
  secureWrite(con, sender, 20);

  unsigned int sendBlock = size - 20;

  //read data
  char msg[sendBlock];
  int found = read(target_inbox, msg, sendBlock);
  //padding
  for (unsigned int i = found; i < sendBlock; i++) {
    msg[i] = '\0';
  }

  //send data
  secureWrite(con, msg, sendBlock);

}

//test the ssl connection
void testConnection(secureConnection con) {
  //read source
  char src[20];
  secureRead(con, src, 20);
  printf("Source: %s\n", src);

  unsigned int size = readSC(con, src);
  writeSC(con, src, size);

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
