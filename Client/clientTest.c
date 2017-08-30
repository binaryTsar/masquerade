

#include "secSock.h"
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
 * Send a block of data
 */
int writeSC(secureConnection con, config cfg, iofd iofds) {

  //set packet size
  int packetSize = cfg->bytes;

  //get data and target
  unsigned int dataLength = packetSize-24;
  char data[dataLength];
  char dst[20];
  if (getData(dst, data, dataLength, iofds) != 0) {
    perror("Unable to read data");
    return 0;
  }

  //send data
  secureWrite(con, dst, 20);
  secureWrite(con, &packetSize, 4);
  secureWrite(con, data, packetSize-24);
  return 1;
}

/*
 * Read a recieved block of memory
 */
void readSC(secureConnection con, unsigned int bytes, iofd iofds) {

  	//read source
    char src[20];
    secureRead(con, src, 20);

    //read data
    unsigned int size = bytes-20;
    char data[size];
    secureRead(con, data, size);

    //show data
    showData(src, data, size, iofds);
}

/*
 * Establish a conection to exchange bytes each way
 */
void session(config cfg, void* clientCtx, iofd iofds) {
  //establish connection
  secureConnection con = makeConnection(clientCtx);

  if (con == NULL) {
    perror("Connection attempt failed");
    return;
  }

  //send data
  if (writeSC(con, cfg, iofds)) {
    //recieve data
    readSC(con, cfg->bytes, iofds);
  }

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

  //check args
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
  iofd ioData = makeIO(cfg->targets, cfg->user);
  if (cfg == NULL) {
    perror("Unable to read config file");
    freeConfig(cfg);
    exit(0);
  }

  //make lifetime context
  void* clientCtx = makeContext(cfg->certs);
  if (clientCtx == NULL) {
    perror("Unable to make context");
    freeConfig(cfg);
    freeIO(ioData);
    exit(0);
  }

  //while running regularly update sesion
  while (terminate(ioData)) {
    session(cfg, clientCtx, ioData);
    sleep(cfg->delay);
  }

  //free memory and return
  freeConfig(cfg);
  freeIO(ioData);
  freeContext(clientCtx);
  return 0;
}
