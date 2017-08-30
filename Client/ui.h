


//io descriptor struct
typedef struct io {
  int controlFD;
  int outFD;
  int inputFD[10];  /* 0 terminated */
  const char** targets;
}* iofd;

//create and free io struct
iofd makeIO(const char**  targets, const char* user);
void freeIO(iofd io);

//functions to get and output data
int getData(char* target, char* buffer, unsigned int len, const iofd fds);
void showData(const char* sender, const char* data, unsigned int size, const iofd fds);
