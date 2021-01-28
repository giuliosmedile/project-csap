#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

// From main.c

int tokenize(char* input, char*** output);  //splits a string into an array of strings
void dowork(char** args, char* buf);        //command interpreter

// From signin.c
