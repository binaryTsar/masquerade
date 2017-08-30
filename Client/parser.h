
//struct for data from config file
typedef struct configStruct {
  const char* user;
  const char* certs[3];
  const char* targets[10]; /* shared with io, NULL padded */
  unsigned int delay;
  unsigned int bytes;
}* config;

//parse config file to struct
config makeConfig(const char* configFile);
void freeConfig(config cfg);
