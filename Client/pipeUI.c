#include "ui.h"
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>

#include <stdlib.h>



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
  int ret = poll(pollfds, 1, 0);

  //read chars from stdin
  unsigned int count = 0;
  while (ret && count < len-1) {
    char c = getchar();
    if (c == '\n') { break; }
    buffer[count++] = c;
  }
  buffer[count] = '\0';

  if (count == 0) {
    strncpy(target, MASK, 20);
  }
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
  dprintf(fds->outFD, "Data from %s\n", sender);
  dprintf(fds->outFD, "%s\n", data);

  if (size == 0) {

  }

}
