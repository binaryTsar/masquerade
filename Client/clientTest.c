#include "secSock.h"
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <limits.h>

#define CONFIG "config.txt"
#define READ "r"

/*
 * Send bytes of data
 */
void writeSC(secureConnection con, unsigned int bytes) {

  char* id = (char*)"user1";

  //send NULL padded username
  char name[20];
  unsigned int i = 0;
  for (; i < strlen(id); i++) {
    name[i] = id[i];
  }
  for (; i<20; i++) {
    name[i] = '\0';
  }

  secureWrite(con, name, 20);

  //for testing use same name
  secureWrite(con, name, 20);



  //write data and size

  char* toSend = (char*)"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char data[bytes-44];
  i = 0;
  for (; i < strlen(toSend); i++) {
    data[i] = toSend[i];
  }
  for (; i<bytes-44; i++) {
    data[i] = '\0';
  }
  unsigned int size = bytes-44;
  char buffer[4];
  memcpy(buffer, &size, 4);
  secureWrite(con, buffer, 4);
  secureWrite(con, data, size);

}

void readSC(secureConnection con, unsigned int bytes) {


    printf("Received mail:\n");
  	//read source
    char src[20];
    secureRead(con, src, 20);
    printf("Sender: %s\n", src);

    //size will be bytes-20
    unsigned int size =bytes-20;

    //read data
    char data[size];
    secureRead(con, data, size);
    printf("Data: %s\n", data);
}

/*
 * Establish a conection to exchange bytes each way
 */
void session(unsigned int bytes) {
  //establish connection
  secureConnection con = makeConnection();

  if (con == NULL) {
    perror("Connection attempt failed.");
    return;
  }

  //send data
  writeSC(con, bytes);

  //recieve data
  readSC(con, bytes);

  //clean up
  closeConnection(con);
}

/*
 * Run a regular connection
 */
int main() {
  if (sizeof(int)!= 4) {
    perror("Int size not compatible.");
    exit(0);
  }


  unsigned int pause;
  unsigned int bytes;

  //get config information
  FILE* cfg = fopen(CONFIG, READ);
  if (
    fscanf(cfg, "WAIT:%u\n", &pause) != 1
    || fscanf(cfg, "BYTES:%u", &bytes) != 1
  ) {
    perror("Error reading config file.");
    exit(0);
  }
  fclose(cfg);


  if (bytes < 128) {
    perror("Block size invalid! Must be between 128 and 4294967295 bytes.");
    exit(0);
  }
  //while running regularly update sesion
  while (1) {
    session(bytes);
    sleep(pause);
  }
  return 0;
}
