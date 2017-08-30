

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
  for (; fdCount < 10; fdCount++) {
    if (fds->inputFD[fdCount] == 0) {
      break;
    }
  }

  //select input
  //poll
  struct pollfd pollfds[fdCount];
  for (int i = 0; i < fdCount; i++) {
    pollfds[i].fd = fds->inputFD[i];
    pollfds[i].events = POLLIN;
  }
  int ret = poll(pollfds, fdCount, 0);

  //select
  int select = 0;
  for (; ret && select < fdCount; select++) {
    if (pollfds[select].revents & POLLIN) {
      break;
    }
  }

  //set data to send
  if (ret == 0) {
    //set mask
    strncpy(target, MASK, 19);
  }
  else {
    //read data and set target
    int bytes = read(fds->inputFD[select], buffer, len-1);
    buffer[bytes] = '\0';
    strncpy(target, fds->targets[select], 19);
  }

  //terminate target string
  target[19] = '\0';
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
    if (targets[i] == NULL) {
      iofds->inputFD[i] = 0;
      break;
    }

    //make fifo
    char fifoPath[60];
    sprintf(fifoPath, "fifos/fifo_%s_%s", user, targets[i]);
    mkfifo(fifoPath, RW);

    //open reading end
    iofds->inputFD[i] = open(fifoPath, FIFO);
    if (iofds->inputFD[i] == 0) {
      perror("Unable to open");
      perror(fifoPath);
      exit(0);
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
  for (int i = 0; i < 10; i++) {
    if (iofds->inputFD[i] == 0) {
      break;
    }
    close(iofds->inputFD[i]);
  }

  //free struct
  free(iofds);

}
