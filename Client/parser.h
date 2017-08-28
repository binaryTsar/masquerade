

typedef struct configStruct {
  const char* user;
  const char* certs[3];
  const char* target;
  unsigned int delay;
  unsigned int bytes;
}* config;


int parse(const char* configFile, config cfg);
