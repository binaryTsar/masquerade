

#include "secSock.h"
#include "transfer.h"
#include "parser.h"
#include "ui.h"

#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <limits.h>


/*
 * Send data in struct form
 */
void sendStruct(secureConnection con, packet data) {

  secureWrite(con, data->fs->target, 20);
  secureWrite(con, &(data->packetSize), sizeof(int));
  secureWrite(con, data->fs->data, (data->packetSize)-24);

}

/*
 * Send bytes of data
 */
void writeSC(secureConnection con, config cfg) {
  //create struct
  packet data = (packet)malloc(sizeof(struct p));

  data->type = F_START|F_END;
  data->packetSize = cfg->bytes;

  //TODO packet fragmentation and recombining
  data->fs = (fileStart)malloc(sizeof(struct fs));

  char dst[20];
  strncpy(dst, cfg->target, 20);

  //get data, but not target (yet)
  unsigned int dataLength = cfg->bytes-28;
  char d[dataLength];
  if (getData(dst, d, dataLength) != 0) {
    perror("unable to read data");
    exit(0);
  }

  data->fs->data = d;
  data->fs->target = dst;

  //send data
  sendStruct(con, data);
}

void readSC(secureConnection con, unsigned int bytes) {

  	//read source
    char src[20];
    secureRead(con, src, 20);

    //size will be bytes-20
    unsigned int size =bytes-20;

    //read data
    char data[size];
    secureRead(con, data, size);
    showData(src, data, size);
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
  writeSC(con, cfg);
  //recieve data
  readSC(con, cfg->bytes);

  //clean up
  closeConnection(con);

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
  }
  return 0;
}
