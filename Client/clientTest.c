

#include "secSock.h"
#include "transfer.h"
#include "parser.h"
#include "ui.h"

#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>


/*
 * Send data in struct form
 */
void sendStruct(secureConnection con, packet data) {

  secureWrite(con, data->target, 20);
  secureWrite(con, &(data->packetSize), sizeof(int));
  secureWrite(con, data->data, (data->packetSize)-24);

}

/*
 * Send bytes of data
 */
void writeSC(secureConnection con, config cfg, iofd iofds) {
  //create struct
  packet data = (packet)malloc(sizeof(struct dataPacket));

  data->packetSize = cfg->bytes;

  char dst[20];

  //get data and target
  unsigned int dataLength = cfg->bytes-28;
  char d[dataLength];
  if (getData(dst, d, dataLength, iofds) != 0) {
    perror("unable to read data");
    exit(0);
  }

  data->data = d;
  data->target = dst;

  //send data
  sendStruct(con, data);
}

void readSC(secureConnection con, unsigned int bytes, iofd iofds) {

  	//read source
    char src[20];
    secureRead(con, src, 20);

    //size will be bytes-20
    unsigned int size =bytes-20;

    //read data
    char data[size];
    secureRead(con, data, size);
    showData(src, data, size, iofds);
}

/*
 * Establish a conection to exchange bytes each way
 */
void session(config cfg, void* clientCtx, iofd iofds) {
  //establish connection
  secureConnection con = makeConnection(clientCtx);

  if (con == NULL) {
    perror("Connection attempt failed.");
    return;
  }

  //send data
  writeSC(con, cfg, iofds);
  //recieve data
  readSC(con, cfg->bytes, iofds);

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
  config cfg = makeConfig(argv[1]);
  if (cfg == NULL) {
    perror("Unable to read config file");
    exit(0);
  }

  //set IO stuff
  const char* users[5] = {(char*)"user1", (char*)"user2", (char*)"user3", (char*)"user4", NULL};
  iofd ioData = makeIO(users, cfg->user);

  //form lifetime context
  void* clientCtx = makeContext(cfg->certs);

  //while running regularly update sesion
  while (1) {
    session(cfg, clientCtx, ioData);
    sleep(cfg->delay);
  }
  return 0;
}
