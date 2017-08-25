

#include "secSock.h"
#include "transfer.h"
#include "parser.h"

#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <limits.h>


/*
 * Send data in struct form
 */
void sendStruct(secureConnection con, packet data) {
  //secureWrite(con, &(data->type), sizeof(int));

  secureWrite(con, data->fs->target, 20);
  secureWrite(con, &(data->packetSize), sizeof(int));
  secureWrite(con, data->fs->data, (data->packetSize)-24);
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
void writeSC(secureConnection con, unsigned int bytes) {
  //create struct
  packet data = (packet)malloc(sizeof(struct p));

  data->type = F_START|F_END;
  data->packetSize = bytes;

  data->fs = (fileStart)malloc(sizeof(struct fs));

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
void session(config cfg, void* clientCtx) {
  //establish connection
  secureConnection con = makeConnection(clientCtx);

  if (con == NULL) {
    perror("Connection attempt failed.");
    return;
  }

  //send data
  writeSC(con, cfg->bytes);
  printf("Written\n");
  //recieve data
  readSC(con, cfg->bytes);

  //clean up
  closeConnection(con);

  printf("Connection closed\n");
}

/*
 * Run a regular connection
 */
int main(int argc, char** argv) {
  //check system compatibility
  if (sizeof(int)!= 4) {
    perror("Int size not compatible.");
    exit(0);
  }

  //open config file
  if (argc < 2) {
    perror("No config file provided.");
    exit(0);
  }

  //read config file
  config cfg = (config)calloc(1, sizeof(struct configStruct));
  if (parse(argv[1], cfg) != 0) {
    exit(0);
  }

  //form lifetime context
  void* clientCtx = makeContext(cfg->certs);

  //while running regularly update sesion
  while (1) {
    session(cfg, clientCtx);
    sleep(cfg->delay);
    printf("Next session\n");
  }
  return 0;
}
