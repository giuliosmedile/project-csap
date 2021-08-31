/// 		MAIN SERVER CODE
#include "server.h"

	int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    int echoServPort;                /* Server port */
    pid_t processID;                 /* Process ID from fork() */
    unsigned int childProcCount;     /* Number of child processes */
    int clients;					 // Number of possible clients that the server will allow to connect at the same time
    int vdr_no;						 // Number of mds's that the server will connect to
    char** vdr_addrs; 			 	 // Array containing the IP addresses of the mds's
    int* vdr_ports;					 // Array containing the ports of the mds's
    int* vdr;                        // Sockets for the datarepo

void haltProgram(int signum) {
    printf("[-] Intercepted signal %d. Closing program...\n", signum);
    close(servSock);
    close(clntSock);
    
    for (int i = 0; i<vdr_no; i++) {
        close(vdr[i]);
    }
    exit(0);
}

int main(int argc, char** argv) {


    #ifdef DEBUG
        printf("\t\tDEBUG MODE\n");
    #endif

    // Override SIGINT's handler to properly close the socket
    signal(SIGINT, haltProgram);

    // Read the configuration file
    readConfig(&echoServPort, &clients, &vdr_no, &vdr_addrs, &vdr_ports);

    // Debug print
    for (int i = 0; i<vdr_no; i++) {
        DEBUGPRINT(("[+] Connecting to %s:%d\n", vdr_addrs[i], vdr_ports[i]));
    }
    
    // Connect to all the VDR (I'm the "client", they're the servers)
    vdr=malloc(vdr_no * sizeof(int));
    for (int i = 0; i<vdr_no; i++) {
        printf("%d", i);
        // The ports the server will use to connect to the vdrs are
        // sequentially increased from echoServPort
        vdr[i] = connectToSocket(vdr_addrs[i], vdr_ports[i]);
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
                // For now, just with the first vdr, later...
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