

//connection instance struct
//generic pointers
typedef struct sConn {
  void* ctx;
  void* ssl;
  int connection;
}* secureConnection;

//create and free lifelong context
void* makeContext(const char** certs);
void freeContext(void* clientCtx);

//create and free a TLS connection
secureConnection makeConnection(void* clientCtx);
void closeConnection(void* con);

//read and write data on connection
int secureRead(const secureConnection con, void* buffer, unsigned int bytes);
int secureWrite(const secureConnection con, const void* buffer, unsigned int bytes);
