#include <stdlib.h>


typedef struct sConn {
  void* ctx;
  void* ssl;
  int connection;
}* secureConnection;



secureConnection makeConnection(void* clientCtx);
void* makeContext(const char** certs);

void closeConnection(void* con);
int secureRead(secureConnection con, void* buffer, size_t bytes);
int secureWrite(secureConnection con, void* buffer, size_t bytes);
