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

  FILE* stream;
  int select = 0;
  for (; select < fdCount; select++) {
    if (pollfds[select].revents & POLLIN) {
      stream = fdopen(select, READ);
      break;
    }
  }


  //read chars from stdin
  unsigned int count = 0;
  while (ret && count < len-1) {
    char c = fgetc(stream);
    if (c == '\n') { break; }
    buffer[count++] = c;
  }
  buffer[count] = '\0';
  fclose(stream);

  if (count == 0) {
    strncpy(target, MASK, 19);
  }
  else {
    strncpy(target, fds->targets[select], 19);
    printf("Sending to %s\n", target);
    printf("Data: %s\n", buffer);
  }
  target[19] = '\0';




  //sucess
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
