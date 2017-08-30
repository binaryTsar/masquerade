
#include "secSock.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define READ "r"
#define OVERWRITE "w"

#define MASK "mask"

/*
 * Read data from client. Return size.
 */
int readSC(secureConnection con, char* src)  {

  //TESTING
  //TODO handle enum stuff (last 4 bytes)

  //read target
  char dst[20];
  secureRead(con, dst, 20);

  //read size
  unsigned int size;
  char buffer[4];
  secureRead(con, buffer, 4);
  memcpy(&size, buffer, 4);
  size = size - 44;

  //read data
  char data[size];
  secureRead(con, data, size);

  //write file to inbox

  //nothing to store
  if (strcmp(MASK, dst) == 0) {
    return size+24;
  }


  //open file
  char path[30];

  sprintf(path, "%s/inbox", dst);

  FILE* inbox = fopen(path, READ);
  if (inbox == NULL) {
    perror("Unable to open file");
    perror(path);
    exit(0);
  }
  char line1[30];
  char line2[30];
  fgets(line1, 30, inbox);
  fgets(line2, 30, inbox);

  unsigned int nameNext, openNext;
  if (sscanf(line1, "NAME_NEXT:%u", &nameNext) != 1
    || sscanf(line2, "OPEN_NEXT:%u", &openNext) != 1) {
      perror("Malformed target inbox writing!");
      perror(path);
      exit(0);
  }

  char messagePath[30];
  sprintf(messagePath, "%s/%u", dst, nameNext);
  if (nameNext == openNext-1) {
    perror("Inbox full.");
    perror(path);
    exit(0);
  }

  fclose(inbox);
  inbox = fopen(path, OVERWRITE);
  if (inbox == NULL) {
    perror("Unable to open file");
    perror(path);
    exit(0);
  }

  if (nameNext == UINT_MAX) {
    nameNext = 0;
  }
  fprintf(inbox, "NAME_NEXT:%u\n", nameNext+1);

  if (openNext == 0) {
    openNext = nameNext;
  }
  fprintf(inbox, "OPEN_NEXT:%u", openNext);

  fclose(inbox);

  //write data to inbox
  int target_inbox = open(messagePath, O_WRONLY|O_TRUNC|O_CREAT, S_IRWXU);
  if (target_inbox < 0) {
    perror("Failed to open inbox for writing");
    exit(0);
  }

  //set sender
  ssize_t bytes = write(target_inbox, src, 20);
  if (bytes != 20) {
    perror("Failed to write to file");
    exit(0);
  }

  //set size
  if (write(target_inbox, &size, 4) != 4) {
    perror("Failed to write to file");
    exit(0);
  }

  //set data
  if (write(target_inbox, data, size) != size) {
    perror("Failed to write to file");
    exit(0);
  }


  return size+24;
}

void sendMask(secureConnection con, unsigned int size) {
  //send mask sender
  char name[20];
  strcpy(name, MASK);
  for (int i = strlen(name); i < 20; i++) {
    name[i] = '\0';
  }
  secureWrite(con, name, 20);

  //send junk
  char junk[size];
  secureWrite(con, junk, size);
}

void writeSC(secureConnection con, char* src, unsigned int size) {
  char path[30];
  sprintf(path, "%s/inbox", src);

  FILE* inbox = fopen(path, READ);
  if (inbox == NULL) {
    perror("Unable to open file");
    perror(path);
    exit(0);
  }
  char line1[30];
  char line2[30];
  fgets(line1, 30, inbox);
  fgets(line2, 30, inbox);

  unsigned int nameNext, openNext;
  if (sscanf(line1, "NAME_NEXT:%u", &nameNext) != 1
    || sscanf(line2, "OPEN_NEXT:%u", &openNext) != 1) {
      perror("Malformed target inbox!");
      perror(path);
      exit(0);
  }

  char messagePath[30];
  sprintf(messagePath, "%s/%u", src, openNext);


  fclose(inbox);

  if (openNext == 0) {
    sendMask(con, size);
    exit(0);
  }

  inbox = fopen(path, OVERWRITE);
  if (inbox == NULL) {
    perror("Unable to open file");
    perror(path);
    exit(0);
  }

  if (nameNext == UINT_MAX) {
    nameNext = 0;
  }
  fprintf(inbox, "NAME_NEXT:%u\n", nameNext);

  if (openNext == nameNext-1) {
    openNext = 0;
  }
  else if (openNext == UINT_MAX) {
    openNext = 1;
  }
  else {
    openNext++;
  }
  fprintf(inbox, "OPEN_NEXT:%u", openNext);
  fclose(inbox);

  //server response
  int target_inbox = open(messagePath, O_RDONLY);
  if (target_inbox < 0) {
    perror("Failed to open inbox for reading");
    perror(messagePath);
    exit(0);
  }

  //get sender
  char sender[20];
  ssize_t bytes = read(target_inbox, sender, 20);
  if (bytes == 0) {
    //empty file
    return;
  }
  else if (bytes != 20) {
    perror("Unable to read from file.");
    exit(0);
  }


  //send sender
  for (unsigned int i = strlen(sender)+1; i < 20; i++) {
    sender[i] = '\0';
  }
  secureWrite(con, sender, 20);

  //data size
  unsigned int dataSize;
  if (read(target_inbox, &dataSize, 4) != 4) {
    perror("Unable to read from file.");
    exit(0);
  }

  unsigned int sendBlock = size - 20;

  //read data
  char msg[dataSize];
  if (read(target_inbox, msg, dataSize) != dataSize) {
    perror("Unable to read from file.");
    exit(0);
  }
  //padding
  for (unsigned int i = dataSize; i < sendBlock; i++) {
    msg[i] = '\0';
  }

  //send data
  secureWrite(con, msg, sendBlock);
  close(target_inbox);
}

//test the ssl connection
void testConnection(secureConnection con) {
  //read source
  char src[20];
  getName(con, src, 20);

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
