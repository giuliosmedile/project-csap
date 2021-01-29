#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#include "signin.c"
#include "connection.c"
#include "utils.c"
#include "../shared/llist.h"
#include "../shared/user.c"

// Standard Definitions
#define BUF_SIZE 256

// From main.c
//void dowork(char* buf);                      //command interpreter
char* takeUserInput(char* input);                    //take a user's input from keyboard
char* interpretInput(char* command, char* output);   //interpret the command

// From utils.c
void tokenize(char* input, char*** output);  //splits a string into an array of strings

// From signin.c
char* login(char* result);
char* signup(char* result);

// From connectToSocket.c
int connectToSocket();
void sendToSocket(int socket, char* str);
char* readFromSocket(int socket, char* str);