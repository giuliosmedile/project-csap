#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#include "readConfig.c"
#include "processRequest.c"
#include "../shared/utils.c"

#define BUF_SIZE 256
// Functions from readConfig.c
void readConfig(char** serv_add);

// Functions from connection.c
int connectToSocket(char* serv_add);
void sendToSocket(int s, char* buf);
char* readFromSocket(int s, char* rcv);
void sendfFile(int s, char* filename);
void receiveFile(int s, char* filename);

// Functions from processRequest.c
char* processRequest(int s, char* request);

// From utils.c
void tokenize(char* input, char*** output);

