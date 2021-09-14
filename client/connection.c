// CODE THAT ALLOWS THE CLIENT TO CONNECT TO A SERVER

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

int connectToSocket(char* serv_add, unsigned short port) {
	int s,len;
    struct sockaddr_in saddr;
    struct hostent *hst;
    char** ops = malloc(sizeof(char**));

    // Create the socket
    if ((s=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0) {
	perror("socket");
	exit(1);
    }
    puts("socket done");

    // Set the options for the socket
    int t = 1;
    setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&t,sizeof(int));

    saddr.sin_addr.s_addr = inet_addr(serv_add);
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(port);

    // Connect to other socket. If it isn't already online, retry every 2 seconds for a
    // minute, then abort
    printf("[-] Trying to connect to server");

    for (int tries = 0; tries<=30; tries++) {
        if (connect(s,(struct sockaddr *)&saddr,sizeof(saddr))<0) {
            printf(".");
            fflush(stdout);
            sleep(2);
        } else {
            break;
        }
        if (tries==30) {
            perror("connect");
	        exit(1);
        }
    }
    printf("\n");
    return s;
}

void sendToSocket(int s, char* buf) {
    // Make sure the string is null terminated
    buf[strlen(buf)] = '\0';
    DEBUGPRINT(("[~] About to send to %d: \"%s\"\n", s, buf));

    // Make sure the string is null terminated, as some junk characters may be leftover from previous sends
    buf[strlen(buf)] = '\0';

    // Write (or send) to socket
    if (write(s, buf, strlen(buf))<0) {
        perror("write");
        exit(1);
    }
    
    return;
}

/**
 * Function that reads the response from the socket, and stores it in rcv
*/
char* readFromSocket(int s, char* rcv) {
    rcv = (char*)malloc(BUF_SIZE * sizeof(char));
	// Read (or recv) from socket
    if (read(s, rcv, BUF_SIZE)<0) {
        perror("read");
        exit(1);
    }
    // rcv[strlen(rcv)-1] = '\0';
    DEBUGPRINT(("[~] received: \"%s\"\n", rcv));
    return rcv;
}

// Function that sends a file through a socket
void sendFile(int s, char* filename, int filesize) {
    int n;                                  // Number of bytes sent
    int i = 1;                              // debug counter
    void* data = (void*)malloc(filesize);   // Pointer to the data to be sent

    // Open the file
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("[-]Error in reading file.");
        exit(1);
    }

    // Read the file
    // note: even though we are sending the file in one block as big as the file itself, I still prefer to
    // iterate through the file, because it is easier to debug
    while((n = fread(data, 1, BUF_SIZE, fp)) > 0) {
        // Send the data
        if (send(s, (void* )data, n, 0) == -1) {
            perror("[-]Error in sending file.");
            exit(1);
        }
        DEBUGPRINT(("%d\t%d\n", i, n));

        bzero(data, BUF_SIZE);
        i++;
    }

    // Close the file
    fclose(fp);
    return;
}

// Function to receive a binary file sent through a socket
void receiveFile(int s, char* filename) {
    int n = 1;                                        // Number of bytes read
    int i = 0;                                        // DEBUGPRINT counter
    FILE *fp;                                         // File pointer
    char buffer[BUF_SIZE];                            // Buffer to store data

    // Open file
    fp = fopen(filename, "wb");

    // While there is data to be read
    while (n > 0) {
        // Read from socket
        n = recv(s, (void*)buffer, BUF_SIZE, 0);

        // If data was received less than BUF_SIZE, the file is over
        if (n < BUF_SIZE) {
            // Write the remaining data and save the file
            fwrite(buffer, 1, n, fp);
            fclose(fp);
            return;
        }

        // Write to file
        fwrite(buffer, 1, BUF_SIZE, fp);
        
        // Reset for next loop
        bzero(buffer, BUF_SIZE);
    }

    // This won't be reached, but still...
    return;
}