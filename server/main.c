/// 		MAIN SERVER CODE

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "signin.c"

#define BUF_SIZE 256
#define SEPARATOR " \t\r\n\v\f;"
#define MAXPENDING 5

void DieWithError(char* str) {
	perror(str);
	exit(1);
}

// Takes an input string, and tokenizes it into an output string array
void tokenize(char* input, char*** output) {
	char** tmp = *output;
	
	int i = 0;
	char* token = strtok(input, SEPARATOR);
	while (token) {
		printf("Token %d: %s\n", i, token);
		tmp[i] = (char*)malloc(BUF_SIZE * sizeof(char));
		strcpy(tmp[i], token);
		token = strtok(NULL, SEPARATOR);
		i++;
	} 

	return;
}

void dowork(int socket) {
	char** operations = (char**)malloc(3*BUF_SIZE);
	char rcvString[BUF_SIZE];
 	int result;

 	if (read(socket, rcvString, sizeof(rcvString)) < 0) {
					perror("read");
					exit(1);
	}

	printf("Received: %s\n", rcvString);
	// Insert the string received from the socket into the operations array
	tokenize(rcvString, &operations);

	printf("Inside dowork\n");

	char* command = malloc(BUF_SIZE);
	strcpy(command, operations[0]);
	
	if (strcmp(command, "login") == 0) {
		printf("- Logging in -\n");
		result = login(operations[1], operations[2]);
	}
	if (strcmp(command, "signup") == 0) {
		printf("- Signing up - \n");
		result = signup(operations[1], operations[2]);
	}

	char* output = (char*)malloc(BUF_SIZE * sizeof(char));
	sprintf(output, "%d", result);
	printf("[-]output: %s\n", output);
	if (write(socket, output, sizeof(output)) < 0) {
		perror("write");
		exit(1);
	}

	free(operations);

}

int main(int argc, char** argv) {
	int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort;     /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */

    // if (argc != 2)     /* Test for correct number of arguments */
    // {
    //     fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
    //     exit(1);
    // }

    echoServPort = 16000;
    //echoServPort = atoi(argv[1]);  /* First arg:  local port */

    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");
      
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed");

/* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);

        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, 
                               &clntLen)) < 0)
            DieWithError("accept() failed");

        /* clntSock is connected to a client! */

        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

    for (;;) /* Run forever */
    {
        /* clntSock is connected to a client! */

        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

        dowork(clntSock);
    }
    /* NOT REACHED */
}