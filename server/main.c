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
    int* vdr;                        // Sockets for the datarepos
    int* vdr_status;                 // Status of the datarepos

void haltProgram(int signum) {
    printf("[-] Intercepted signal %d. Closing program...\n", signum);
    
    for (int i = 0; i < vdr_no; i++) {
        sendToSocket(vdr[i], "exit");
        close(vdr[i]);
    }

    close(servSock);
    close(clntSock);
    exit(signum);
}

int main(int argc, char** argv) {


    #ifdef DEBUG
        printf("\t\tDEBUG MODE\n");
    #endif

    // Signal handling
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = haltProgram;
    sigaction(SIGSEGV, &act, NULL);
    sigaction(SIGINT, &act, NULL);

    // Read the configuration file
    readConfig(&echoServPort, &clients, &vdr_no, &vdr_addrs, &vdr_ports);

    // Debug print
    for (int i = 0; i<vdr_no; i++) {
        DEBUGPRINT(("[+] Will attempt to connect to %s:%d\n", vdr_addrs[i], vdr_ports[i]));
    }
    
    // Connect to all the VDRs (I'm the "client", they're the servers)
    vdr=malloc(vdr_no * sizeof(int));
    vdr_status=malloc(vdr_no * sizeof(int));
    for (int i = 0; i<vdr_no; i++) {
        printf("%d: ", i);
        // The ports the server will use to connect to the vdrs are
        // sequentially increased from echoServPort
        vdr[i] = connectToSocket(vdr_addrs[i], vdr_ports[i]);
    }

    // Set the status for all the vdrs, so that we know which ones are up
    for (int i = 0; i<vdr_no; i++) {
        if (vdr[i] == -1) {
            printf("[-] Could not connect to VDR %d\n", vdr[i]);
            vdr_status[i] = 0;
        } else {
            printf("[+] Connected to VDR %d\n", vdr[i]);
            vdr_status[i] = 1;
        }
    }

    // Choose the first leader
    int leader = chooseNewLeader(vdr, vdr_status, vdr_no);
    

    // Tell the leader that they're leader, tell slaves that they're not
    for (int i = 0; i<vdr_no; i++) {
        if (vdr[i] == leader && vdr_status[i] == 1) {
            // Tell them they're the leader
            sendToSocket(vdr[i], "LEADER");
        } else if (vdr_status[i] == 1) {
            // Tell them they're not the leader
            sendToSocket(vdr[i], "SLAVE");
        }
    }

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

                // Main program code
                dowork(clntSock, leader, vdr, vdr_status, vdr_no);

                DEBUGPRINT(("Before pinging leader\n"));
                // Check if the leader is still alive
                int leader_status = ping(leader);
                DEBUGPRINT(("After pinging leader\n"));
                if (leader_status == 0) {
                    printf("[!!] Leader %d is down.\n", leader);
                    vdr_status = markRepoDead(leader, vdr, vdr_no, vdr_status);

                    // Check status of other repos
                    for (int i = 0; i<vdr_no; i++) {
                        if (ping(vdr[i]) && vdr_status[i] == 1) 
                            vdr_status[i] = 1; 
                        else 
                            vdr_status[i] = 0;
                    }

                    // Debug print
                    DEBUGPRINT(("Printing status of all repos\n"));
                    DEBUGPRINT(("address\t\tsocket id\tstatus\n"));
                    for (int i = 0; i<vdr_no; i++) {
                        DEBUGPRINT(("%s:%d\t%d\t%d\n", vdr_addrs[i], vdr_ports[i], vdr[i], vdr_status[i]));
                    }

                    // Check if all other repos are dead
                    if (allReposDead(vdr_status, vdr_no)) {
                        printf("[-] All VDRs are down. Exiting.\n");
                        close(clntSock);
                        exit(1);
                    }

                    leader = chooseNewLeader(vdr, vdr_status, vdr_no);
                    sendToSocket(leader, "LEADER");
                }
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