///			MAIN CLIENT CODE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stddef.h>
#include "client.h"

#define MAXARGS 64
#define BUF_SIZE 256

int s;      // The socket I'm interacting with
t_user u;   // The user that the socket sends after logging in

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
    } else if (!strcmp(command, "signup")) {
        signup(output);
    } else if (!strcmp(command, "exit")) {
        exit(0);
    } else {
    	// Command not found
    	strcpy(output, "null");
	}

    return output;
}

void main (int argc, char** argv) {
    char buf[BUF_SIZE];
    char command[BUF_SIZE];
    char* response = (char*)malloc(BUF_SIZE * sizeof(char));
    char output[BUF_SIZE];
    char **args;
    char* str = (char*)malloc(BUF_SIZE * sizeof(char));

    //TODO: AUTOMATIC CONNECTION TO SERVER

    // Read the configuration file
    char* serv_add = (char*)malloc(sizeof(BUF_SIZE * sizeof(char)));
    readConfig(&serv_add);
    printf("Server address: %s\n", serv_add);

    s = connectToSocket(serv_add);

    printf("\t ---------------------------\n");
    printf("\t ---- Voicemail Service ----\n");
    printf("\t ---------------------------\n");
    int i = 0;
    // Main loop
    for (;;) {
        // take the user input
        takeUserInput(buf);
        strcpy(command, buf);
        command[strlen(command)-1] = '\0';
        // interpret it basing on the command
        interpretInput(buf, output);
        if (!strcmp(output, "null")) {
        	printf("Command not recognized. Please try again.\n");
        	continue;
        }
        // send the command and arguments to socket
        sendToSocket(s, output);
        //wait for reply
        response = readFromSocket(s, buf);
        // Then handle the server response
        handleSocketReplies(command, response);
        i++;
    }
}