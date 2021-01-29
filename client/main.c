///			MAIN CLIENT CODE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include "main.h"

#define MAXARGS 64
#define BUF_SIZE 256

int s;

char* takeUserInput(char* input) {
    printf("\033[1;31m");
    printf("$: ");
    if (fgets(input,sizeof(input),stdin) == NULL) {
        printf("\n");
        exit(0);
    }
    printf("\033[0m");

    return input;
}

char* interpretInput(char* command, char* output) {
    command[strlen(command)-1] = '\0';

    if (!strcmp(command, "login")) {
        login(output);
    }

    if (!strcmp(command, "signup")) {
        signup(output);
    }

    return output;
}

void main (int argc, char** argv) {
    char buf[BUF_SIZE];
    char output[BUF_SIZE];
    char **args;
    char* str = (char*)malloc(BUF_SIZE * sizeof(char));

    //TODO: AUTOMATIC CONNECTION TO SERVER
    s = connectToSocket();

    printf("\t ---------------------------\n");
    printf("\t ---- Voicemail Service ----\n");
    printf("\t ---------------------------\n");

    // Main loop
    for (;;) {
        // take the user input
        takeUserInput(buf);
        // interpret it basing on the command
        interpretInput(buf, output);
        // send the command and arguments to socket
        sendToSocket(s, output);
        //wait for reply
        readFromSocket(s, buf);
    }
}