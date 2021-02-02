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

int connectToSocket(char* serv_add) {
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
    saddr.sin_port=htons(16000);

    // Connect to other socket
    if (connect(s,(struct sockaddr *)&saddr,sizeof(saddr))<0){
	perror("connect");
	exit(1);
    }
    puts("connect done");

    return s;
}

void sendToSocket(int s, char* buf) {
    printf("BUFFER: %s", buf);
    // Write (or send) to socket
    if (write(s, buf, strlen(buf))<0) {
        perror("write");
        exit(1);
    }
    
    return;
}

char* readFromSocket(int s, char* rcv) {
    rcv = (char*)malloc(BUF_SIZE * sizeof(char));
	// Read (or recv) from socket
    if (read(s, rcv, sizeof(rcv))<0) {
        perror("read");
        exit(1);
    }
    //rcv[strlen(rcv)-1] = '\0';
    printf("[+] received: %s\n", rcv);
    return rcv;
}

// Function that sends a file through a socket
void sendFile(int s, char* filename) {
	char data[BUF_SIZE] = {0};
	FILE* fp = fopen(filename, "r");

	while(fgets(data, BUF_SIZE, fp) != NULL) {
		if (send(s, data, sizeof(data), 0) == -1) {
			perror("[-]Error in sending file.");
			exit(1);
		}
		bzero(data, BUF_SIZE);
	}
	fclose(fp);
}

void receiveFile(int s, char* filename) {
	int n;
	FILE *fp;
	char buffer[BUF_SIZE];

	fp = fopen(filename, "w");
	// Loops until the file has been all received
	while (1) {
		// If the file is over, I stop the loop
		if (recv(s, buffer, BUF_SIZE, 0) <= 0) {
			break;
			return;
		}
		// Else write to a file
		fprintf(fp, "%s", buffer);
		bzero(buffer, BUF_SIZE);
	}
	fclose(fp);
	return;
}