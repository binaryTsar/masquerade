#include <stdlib.h>

typedef struct sConn {
  void* ctx;
  void* ssl;
  int connection;
}* secureConnection;



int startServer(void (*serverCB)(secureConnection con));
int secureRead(secureConnection con, char* buffer, size_t bytes);
int secureWrite(secureConnection con, char* buffer, size_t bytes);
void closeConnection(secureConnection);
