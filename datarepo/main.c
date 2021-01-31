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

int s;      // The socket I'm interacting with

void main (int argc, char** argv) {

	char* request = (char*)malloc(BUF_SIZE *  sizeof(char));
	char* reply = (char*)malloc(BUF_SIZE * sizeof(char));

    // Read the configuration file
    char* serv_add = (char*)malloc(sizeof(BUF_SIZE * sizeof(char)));
    readConfig(&serv_add);
    printf("Server address: %s\n", serv_add);

    // Connect to the server through the address given in the conf file
    s = connectToSocket(serv_add);

    for (;;) {
    	// Wait for request from socket
    	request = readFromSocket(s, request);
    	// Process request
    	reply = processRequest(request);
    	// Send Reply
    	sendToSocket(s, reply);

    	bzero(request, BUF_SIZE);
    	bzero(reply, BUF_SIZE);
    }
}