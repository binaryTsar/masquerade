

#include "secSock.h"
#include "transfer.h"


#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <limits.h>

#define CONFIG "config.txt"
#define READ "r"

/*
 * Send data in struct form
 */
void sendStruct(secureConnection con, packet data) {
  secureWrite(con, data->sender, 20);
  //secureWrite(con, &(data->type), sizeof(int));

  secureWrite(con, data->fs->target, 20);
  secureWrite(con, &(data->packetSize), sizeof(int));
  secureWrite(con, data->fs->data, (data->packetSize)-44);
  /*
  if (data->type & F_START) {
    secureWrite(con, data->fs->target, 20);
    secureWrite(con, data->fs->data, data->packetSize-48);
  }
  else {
    secureWrite(con, data->fc->data, data->packetSize-28);
  }
  */
}

/*
 * Send bytes of data
 */
void writeSC(secureConnection con, unsigned int bytes, char* id) {
  //create struct
  packet data = (packet)malloc(sizeof(struct p));

  data->sender = id;
  data->type = F_START|F_END;
  data->packetSize = bytes;

  data->fs = (fileStart)malloc(sizeof(struct fs));

  //testing only
  data->fs->target = id;

  //data
  int offset = 48; //will change
  char* toSend = (char*)"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char d[bytes-offset];
  unsigned int i = 0;
  for (; i < strlen(toSend); i++) {
    d[i] = toSend[i];
  }
  for (; i<bytes-44; i++) {
    d[i] = '\0';
  }

  data->fs->data = d;

  //send data
  sendStruct(con, data);
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
void session(unsigned int bytes, char* id) {
  //establish connection
  secureConnection con = makeConnection();

  if (con == NULL) {
    perror("Connection attempt failed.");
    return;
  }

  //send data
  writeSC(con, bytes, id);

  //recieve data
  readSC(con, bytes);

  //clean up
  closeConnection(con);
}

/*
 * Run a regular connection
 */
int main(int argc, char** args) {
  //check compatibility
  if (sizeof(int)!= 4) {
    perror("Int size not compatible.");
    exit(0);
  }

  //get id
  char* id = (char*)calloc(20,1);
  if (argc < 2) {
    perror("No ID given.");
    exit(0);
  }
  if (strlen(args[1]) >= 20) {
    perror("Invalid ID.");
    exit(0);
  }
  else {
    size_t s = 20;
    strncpy(id, args[1], s);
  }

  //get NULL padded id
  char name[20];
  unsigned int i = 0;
  for (; i < strlen(id); i++) {
    name[i] = id[i];
  }
  for (; i<20; i++) {
    name[i] = '\0';
  }

  //get config information
  unsigned int pause;
  unsigned int bytes;
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
    session(bytes, name);
    sleep(pause);
  }
  free(id);
  return 0;
}
