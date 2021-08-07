#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

// Standard Definitions
#define BUF_SIZE 4096

// The next two are for handling the color output in the console
#define COLOR "\033[0;33m"
#define STD_COL "\033[0m"

// Shared imports
#include "../shared/structs.h"
#include "../shared/utils.c"
#include "../shared/list.c"
#include "../shared/user.c"
#include "../shared/message.c"
//#include "../shared/connection.c"

// Local imports
#include "signin.c"
#include "connection.c"
#include "readConfig.c"
#include "handleServerReplies.c"
#include "messages.c"

// // From main.c

// //void dowork(char* buf);                      //command interpreter
// char* takeUserInput(char* input);                    //take a user's input from keyboard
// char* interpretInput(char* command, char* output);   //interpret the command

// // From utils.c

// void tokenize(char* input, char*** output);  //splits a string into an array of strings

// // From signin.c

// char* login(char* result, t_user** u);
// char* signup(char* result, t_user** u);
// char* logout(char* result, t_user** u);

// // From connectToSocket.c

// int connectToSocket(char* serv_add, unsigned short serv_port);
// void sendToSocket(int socket, char* str);
// char* readFromSocket(int socket, char* str);
// void sendFile(int s, char* filename, int filesize);
// void receiveFile(int s, char* filename);

// // From handleServerReplies.c

// t_user* handleServerReplies(char* command, char* response, t_user* u);

// // From readConfig.c

// void readConfig (char** serv_add, unsigned short* serv_port);

// //From messages.c

// char* add(char* output, t_user** u_p);
// char* record(char* result, t_user** u_p, char* filename);

