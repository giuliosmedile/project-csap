///			MAIN CLIENT CODE
#include "datarepo.h"

#define MAXARGS 64

int dataRepoSock;      // "My" end of the socket
int servSock;
int clntSock;
int processID;
int childProcCount;

void haltProgram(int signum) {

    printf("Halting program with signal %d.\n", signum);

    // Close all sockets
    close(dataRepoSock);
    close(servSock);
    close(clntSock);
    exit(1);
}

void main (int argc, char** argv) {

    // Override SIGINT's handler to properly close the socket
    signal(SIGINT, haltProgram);
    signal(SIGSTOP, haltProgram);
    signal(SIGSEGV, haltProgram);

	char* request = (char*)malloc(BUF_SIZE *  sizeof(char));
	char* reply = (char*)malloc(BUF_SIZE * sizeof(char));

    // Read the configuration file
    unsigned short servPort;
    readConfig(&servPort);
    printf("My port: %d\n", servPort);

    // Connect to the server through the address given in the conf file
    dataRepoSock = CreateTCPServerSocket(servPort);

    for (;;) /* Run forever */
    {
        clntSock = AcceptTCPConnection(dataRepoSock);
        /* Fork child process and report any errors */
        if ((processID = fork()) < 0)
            DieWithError("fork() failed");
        else if (processID == 0)  /* If this is the child process */
        {
        	printf("[-]Starting to do work for %d\n", clntSock);
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