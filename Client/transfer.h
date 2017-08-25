

/*
 * This header defines structs and protocols used in transfers
 */

//define what the packet does
#define F_START 1     //starts a file. If false, continues
#define F_END 2       //contains end of file


//file start payload struct
typedef struct fs {
  char* target;
  char* data;
}* fileStart;

//continue a file
typedef struct fc {
  char* data;
}* fileContinue;


//describe a packet

//id will be done by certs at TLS layer
 typedef struct p {
   int type;
   int packetSize;
   union {
     fileStart fs;
     fileContinue fc;
   };
 }* packet;
