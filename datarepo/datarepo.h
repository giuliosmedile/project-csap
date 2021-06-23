///      LIBRARY IMPORTS

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

///      IMPORTS

#include "../shared/utils.c"
#include "../shared/user.c"
#include "messages.c"
#include "connection.c"
#include "readConfig.c"
#include "processRequest.c"
#include "handleSocketReplies.c"
#include "dowork.c"


#define BUF_SIZE 256

///      FUNCTION DECLARATIONS

// From utils.c
void tokenize(char* input, char*** output);

// Functions from readConfig.c
void readConfig (short unsigned int* serv_port);

// Functions from connection.c
int connectToSocket(char* serv_add, unsigned short port);
void sendToSocket(int s, char* buf);
char* readFromSocket(int s, char* rcv);
void sendfFile(int s, char* filename);
void receiveFile(int s, char* filename);

// Functions from processRequest.c
char* processRequest(int s, char* request);

// Functions from messages.c
void setMessagesRepository(char* input);
char* searchMessage(char* username, int messageno);
void deleteMessage(char* username, int messageno);

