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
    printf("[-] About to send to %d: \"%s\"\n", s, buf);
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
    printf("[+] received: \"%s\"\n", rcv);
    return rcv;
}

// Function that sends a file through a socket
void sendFile(int s, char* filename, int filesize) {
    int n;
    int i = 0;
    void* data = (void*)malloc(filesize);

    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("[-]Error in reading file.");
        exit(1);
    }

    while(fread(data, 1, filesize, fp) > 0) {
        if (send(s, (void* )data, filesize, 0) == -1) {
            perror("[-]Error in sending file.");
            exit(1);
        }
        i++;
        printf("%s", data);
        bzero(data, BUF_SIZE);
    }
    fclose(fp);
    printf("Sent %d blocks\n", i);
}

void receiveFile(int s, char* filename, int filesize) {
    int n;
    int i = 0;
    FILE *fp;
    char buffer[BUF_SIZE];

    fp = fopen(filename, "wb");
    while (1) {
        n = recv(s, (void*)buffer, BUF_SIZE, 0);
        if (n <= 0){
            break;
            return;
        }

        fwrite(buffer, 1, BUF_SIZE, fp);

        printf("%s", buffer);
        bzero(buffer, BUF_SIZE);
    }
    return;
}