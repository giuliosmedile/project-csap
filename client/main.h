#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#include "signin.c"
#include "connectToSocket.c"

// From main.c
void tokenize(char* input, char*** output);  //splits a string into an array of strings
void dowork(char** args, char* buf);        //command interpreter

// From signin.c
char* login();
char* signup();

// From connectToSocket.c
int connectToSocket();
