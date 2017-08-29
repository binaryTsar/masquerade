

//store input and output file descriptors
typedef struct io {
  int controlFD;
  int outFD;
  //0 terminated
  int inputFD[10];
}* iofd;



#define MASK "mask"

int getData(char* target, char* buffer, unsigned int len, iofd fds);
void showData(char* sender, char* data, unsigned int size, iofd fds);
