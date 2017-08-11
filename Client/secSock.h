#include <stdlib.h>


typedef struct sConn {
  void* ctx;
  void* ssl;
  int connection;
}* secureConnection;



secureConnection makeConnection();
void closeConnection(void* con);
int secureRead(secureConnection con, char* buffer, size_t bytes);
int secureWrite(secureConnection con, char* buffer, size_t bytes);
