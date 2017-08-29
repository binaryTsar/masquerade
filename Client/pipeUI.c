#include "ui.h"

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>

#include <string.h>
#include <poll.h>
#include <unistd.h>

#include <stdlib.h>

#define READ "r"

/*
 * Read data from stdin
 */
int getData(char* target, char* buffer, unsigned int len, iofd fds) {

  int fdCount = 0;
  for (; fdCount < 10; fdCount++) {
    if (fds->inputFD[fdCount] == 0) {
      break;
    }
  }

  //select input to use
  if (fdCount == 0) {
    perror("No inputs");
    exit(0);
  }

  struct pollfd pollfds[fdCount];
  for (int i = 0; i < fdCount; i++) {
    pollfds[i].fd = fds->inputFD[i];
    pollfds[i].events = POLLIN;
  }
  int ret = poll(pollfds, fdCount, 0);

  char* path;
  int file = 0;
  for (int i = 0; ret && i < fdCount; i++) {
    if (pollfds[i].revents & POLLIN) {
      file = fds->inputFD[i];
      path = fds->targets[i];
      break;
    }
  }

  if (file == 0) {
    strncpy(target, MASK, 19);
    target[19] = '\0';
    return 0;
  }

  //read chars
  int bytes = read(file, buffer, len-1);
  buffer[bytes] = '\0';

  if (bytes == 0) {
    strncpy(target, MASK, 19);
  }
  else {
    strncpy(target, path, 19);
  }
  target[19] = '\0';

  return 0;

}



/*
 * Output read data
 */
void showData(char* sender, char* data, unsigned int size, iofd fds) {
  //check data is not masking
  if (strcmp(sender, MASK) == 0) {
    return;
  }

  //print data
  //dprintf(fds->outFD, "Data from %s\n", sender);
  //dprintf(fds->outFD, "%s\n", data);
  printf("Data from %s\n", sender);
  printf("%s\n", data);

  if (size == 0 || fds == NULL) {
    //supresses unused warnings
    //need to use with binary data
  }
}
