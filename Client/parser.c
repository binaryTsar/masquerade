#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define READ "r"
#define USER "USER_ID"
#define ROOT "ROOT_CERT"
#define CERT "USER_CERT"
#define KEY "USER_KEY"
#define TARGET "TARGET"
#define DELAY "WAIT"
#define SIZE "BYTES"


int parseLine(FILE* file, config cfg) {
  char buffer[20];
  for (int c = 0; c < 19; c++) {
    char next = fgetc(file);
    if (next == ':') {
      buffer[c] = '\0';
      break;
    }
    else if (next == '\n' || next == EOF) {
      //line is meaningless or final
      return 0;
    }
    else {
      buffer[c] = next;
    }
  }


  buffer[19] = '\0';
  char* field = (char*)malloc(40);
  if (fgets(field, 40, file) == NULL) {
    free(field);
    perror("Malformed config file.");
    return 1;
  }

  //remove new line
  field[strlen(field)-1] = '\0';


  //match keys
  if (strncmp(buffer, USER, strlen(USER)) == 0) {
    cfg->user = field;
  }
  else if (strncmp(buffer, TARGET, strlen(TARGET)) == 0) {
    cfg->targets[0] = field;
  }
  else if (strncmp(buffer, ROOT, strlen(ROOT)) == 0) {
    cfg->certs[0] = field;
  }
  else if (strncmp(buffer, CERT, strlen(CERT)) == 0) {
    cfg->certs[1] = field;
  }
  else if (strncmp(buffer, KEY, strlen(KEY)) == 0) {
    cfg->certs[2] = field;
  }
  else if (strncmp(buffer, DELAY, strlen(DELAY)) == 0) {
    unsigned int i;
    if (sscanf(field, "%u", &i) != 1) {
      perror("Malformed config file.");
      return 1;
    }
    cfg->delay = i;
    free(field);
  }
  else if (strncmp(buffer, SIZE, strlen(SIZE)) == 0) {
    unsigned int i;
    if (sscanf(field, "%u", &i) != 1) {
      perror("Malformed config file.");
      return 1;
    }
    cfg->bytes = i;
    free(field);
  }
  else {
    free(field);
  }

  return 0;
}

int valid(config cfg) {
  int v = 0;
  if (cfg->user == NULL) {
    perror("No username given.");
    v = 1;
  }
  if (cfg->targets[0] == NULL) {
    perror("No target given.");
    v = 1;
  }
  if (cfg->certs[0] == NULL
    || cfg->certs[1] == NULL
    || cfg->certs[2] == NULL) {
      perror("Missing certificate.");
      v = 1;
  }
  if (cfg->delay == 0) {
    perror("Invalid delay given.");
    v = 1;
  }
  if (cfg->bytes < 128) {
    perror("Invalid block size.");
    v = 1;
  }
  return v;
}

config makeConfig(const char* configFile) {

  FILE* file  = fopen(configFile, READ);
  if (file  == NULL) {
    perror("Could not open given configuration file:");
    perror(configFile);
    return NULL;
  }

  //arrays are empty, pointers are NULL
  config cfg = (config)calloc(sizeof(struct configStruct), 1);

  while (feof(file) == 0) {
    if (parseLine(file, cfg) != 0) {
      perror("Error reading line");
      fclose(file);
      return NULL;
    }
  }

  fclose(file);

  //check fields were valid
  if (valid(cfg) == 0) {
    return cfg;
  }
  else {
    perror("Invalid config file");
    return NULL;
  }
}
