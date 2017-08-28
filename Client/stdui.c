
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>

#include "ui.h"


/*
 * Read data from stdin
 */
int getData(char* target, char* buffer, unsigned int len) {

  //read chars from stdin
  unsigned int count = 0;

  struct pollfd fds;
  fds.fd = STDIN_FILENO;
  fds.events = POLLIN;
  int ret = poll(&fds, 1, 0);

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
void showData(char* sender, char* data, unsigned int size) {
  //check data is not masking
  if (strcmp(sender, MASK) == 0) {
    return;
  }

  //print data
  printf("Data from %s\n", sender);
  printf("%s\n", data);

  if (size == 0) {

  }

}
