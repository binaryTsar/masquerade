#include <stdlib.h>

//store server data
typedef struct sConn {
  void* ctx;
  void* ssl;
  int connection;
}* secureConnection;

//start and stop a server
int startServer(void (*serverCB)(secureConnection con));
void closeConnection(const secureConnection);

//read and write data, get client name
int secureRead(const secureConnection con, char* buffer, size_t bytes);
int secureWrite(const secureConnection con, const char* buffer, size_t bytes);
int getName(const secureConnection con, char* buffer, size_t bytes);
