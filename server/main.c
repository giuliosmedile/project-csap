/// 		MAIN SERVER CODE
#include "server.h"

#define BUF_SIZE 256
#define SEPARATOR " \t\r\n\v\f;"

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
    unsigned short echoServPort;     /* Server port */
    pid_t processID;                 /* Process ID from fork() */
    unsigned int childProcCount = 0; /* Number of child processes */
    int clients;					 // Number of possible clients that the server will allow to connect at the same time
    int mds_no;						 // Number of mds's that the server will connect to
    char** mds_addrs 			 // Array containing the IP addresses of the mds's

    // Read the configuration file
    readConfig(&echoServPort, &clients, &mds_no, &mds_addrs);
    // echoServPort = atoi(argv[1]);  /* First arg:  local port */

    servSock = CreateTCPServerSocket(echoServPort);

    for (;;) /* Run forever */
    {
        clntSock = AcceptTCPConnection(servSock);
        /* Fork child process and report any errors */
        if ((processID = fork()) < 0)
            DieWithError("fork() failed");
        else if (processID == 0)  /* If this is the child process */
        {
            close(servSock);   /* Child closes parent socket */
            for (;;) {
            	dowork(clntSock);
        	}
            exit(0);           /* Child process terminates */
        }

        printf("with child process: %d\n", (int) processID);
        close(clntSock);       /* Parent closes child socket descriptor */
        childProcCount++;      /* Increment number of outstanding child processes */

        while (childProcCount) /* Clean up all zombies */
        {
            processID = waitpid((pid_t) -1, NULL, WNOHANG);  /* Non-blocking wait */
            if (processID < 0)  /* waitpid() error? */
                DieWithError("waitpid() failed");
            else if (processID == 0)  /* No zombie to wait on */
                break;
            else
                childProcCount--;  /* Cleaned up after a child */
        }
    }
}