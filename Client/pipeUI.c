

//include dprintf
#define _POSIX_C_SOURCE 200809L
//#define _GNU_SOURCE

//implements this header
#include "ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//input polling and file handling
#include <poll.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


#define MASK "mask"
#define RW S_IRUSR|S_IWUSR
#define FIFO O_RDONLY|O_NONBLOCK

/*
 * Ger data to send
 */
int getData(char* target, char* buffer, unsigned int len, const iofd fds) {

  //count file descriptors
  int fdCount = 0;
  for (int i=0;i<10; fds->inputFD[i]?i++:(fdCount=i,i=10));

  //select input
  //poll
  struct pollfd p[fdCount];
  for (int i=0; i<fdCount; p[i].fd=fds->inputFD[i], p[i++].events=POLLIN);

  if (poll(p, fdCount, 0) < 0) {
    perror("Unable to poll inputs");
    return 1;
  }

  //select
  int select = -1;
  for (int i=0; i<fdCount;(p[i].revents&POLLIN)?(select=i,i=fdCount):(i++));

  //set mask if no inputs are ready
  if (select == -1) {
    //set mask
    strncpy(target, MASK, 20);
    return 0;
  }

  //read data and set target
  int bytes = read(fds->inputFD[select], buffer, len-1);
  buffer[bytes] = '\0';
  strncpy(target, fds->targets[select], 20);

  //terminate target string
  return 0;
}

/*
 * Output recieved data
 */
void showData(const char* sender, const char* data, unsigned int size, const iofd fds) {
  //check data is not masking
  if (strcmp(sender, MASK) == 0) {
    return;
  }

  //print data
  dprintf(fds->outFD, "Data from %s\n", sender);
  dprintf(fds->outFD, "%s\n", data);

  if (size == 0) {
    //supresses unused warnings
    //need to use with binary data
  }
}

/*
 * Make iofd struct
 */
iofd makeIO(const char** targets, const char* user) {
  iofd iofds = (iofd)malloc(sizeof(struct io));

  //set out and control
  iofds->controlFD = STDIN_FILENO;
  iofds->outFD = STDOUT_FILENO;

  //set fifos for each communication channel
  int i = 0;
  for (;i < 10; i++) {
    //set last fd to 0
    if (!targets[i]) {
      iofds->inputFD[i] = 0;
      break;
    }

    //make fifo
    char fifoPath[60];
    sprintf(fifoPath, "fifos/fifo_%s_%s", user, targets[i]);
    mkfifo(fifoPath, RW);

    //open reading end
    iofds->inputFD[i] = open(fifoPath, FIFO);
    if (!iofds->inputFD[i]) {
      perror("Unable to open fifo");
      for (int j=0;j<i;close(iofds->inputFD[j++]));
      free(iofds);
      return NULL;
    }
    iofds->targets = targets;
  }

  return iofds;
}

/*
 * Free an iofd struct
 */
void freeIO(iofd iofds) {
  //output is standard out
  //control is standard in
  //targets is a referance
  //leave all of them

  //close fifos
  for (int i=0;i<10;(iofds->inputFD[i])?close(iofds->inputFD[i++]):(i=10));

  //free struct
  free(iofds);
}

/*
 * Check if the program is finished
 */
int terminate(iofd fds) {
  //check if there is data to read
  struct pollfd p;
  p.fd=fds->controlFD;
  p.events=POLLIN;
  if (poll(&p, 1, 0)) {
    //read data
    return 0;
  }
  return 1;
}
