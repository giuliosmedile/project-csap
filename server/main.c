/// 		MAIN SERVER CODE
#include "server.h"

int main(int argc, char** argv) {
	int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    int echoServPort;     /* Server port */
    pid_t processID;                 /* Process ID from fork() */
    unsigned int childProcCount; /* Number of child processes */
    int clients;					 // Number of possible clients that the server will allow to connect at the same time
    int vdr_no;						 // Number of mds's that the server will connect to
    char** vdr_addrs; 			 	 // Array containing the IP addresses of the mds's

    // Read the configuration file
    readConfig(&echoServPort, &clients, &vdr_no, &vdr_addrs);
    
    // Connect to all the VDR (I'm the "client", they're the servers)
    int vdr[vdr_no];
    for (int i = 0; i<vdr_no; i++) {
        printf("%d", i);
        // The ports the server will use to connect to the vdrs are
        // sequentially increased from echoServPort
        vdr[i] = connectToSocket(vdr_addrs[i], echoServPort+i);
    }
    printf("done");

    servSock = CreateTCPServerSocket(echoServPort);
    for (;;) /* Run forever */
    {
        clntSock = AcceptTCPConnection(servSock);
        /* Fork child process and report any errors */
        if ((processID = fork()) < 0)
            DieWithError("fork() failed");
        else if (processID == 0)  /* If this is the child process */
        {
        	printf("[-]Starting to do work for %d\n", clntSock);
            close(servSock);   /* Child closes parent socket */
            for (;;) {
            	dowork(clntSock, vdr[0]);
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