

typedef struct configStruct {
  const char* user;
  const char* certs[3];
  unsigned int delay;
  unsigned int bytes;
}* config;


int parse(const char* configFile, config cfg);
