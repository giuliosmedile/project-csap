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

#define SIZE 1024
#define MAXARGS 64
#define BUF_SIZE 64
#define SEPARATOR " \t\r\n\v\f;"

int connectToSocket();      //function that connects to a socket TODO
int s;
int commandArgs;            //counter that keeps track of how many arguments are in a command

void tokenize(char* input, char*** output) {
    printf("tokenize\n");
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
    commandArgs = i; 

    return;
}

void dowork(char** args, char* buf) {
    char command[BUF_SIZE];
    char str[BUF_SIZE];
    

    strcpy(command, args[0]);
    printf("TEST-COMMAND: %s\n", command);

    if (!strcmp(command, "connect")) {
        s = connectToSocket();
        return;
    } 

    if (!strcmp(command, "login")) {
        str = login();
    }

    if (!strcmp(command, "signup")) {
        str = signup();
    }

    sendToSocket(s, str);

}

void sendToSocket(int socket, char* buf) {
    //char** result = malloc(sizeof(char**));
    char* rcv = (char*)malloc(BUF_SIZE * sizeof(char));
    // char* sentString = malloc(BUF_SIZE * sizeof(char));
    // concatenate(argv, &sentString);

    printf("BUFFER: %s", buf);
    // Write (or send) to socket
    if (write(s, buf, strlen(buf))<0) {
        perror("write");
        exit(1);
    }

    // Read (or recv) from socket
    if (read(s, rcv, sizeof(rcv))<0) {
        perror("read");
        exit(1);
    }
    printf("Received: %s\n", rcv);

    // TODO: HANDLE RECEIVING STUFF
}

void main (int argc, char** argv) {
    char buf[SIZE];
    char **args;

    printf("\t ---------------------------\n");
    printf("\t ---- Voicemail Service ----\n");
    printf("\t ---------------------------\n");

    for (;;) {
        printf("\033[1;31m");
        printf("$: ");

        if (fgets(buf,sizeof(buf),stdin) == NULL) {
            printf("\n");
            exit(0);
        }
        printf("\033[0m");

        char* input = (char*)malloc(BUF_SIZE * sizeof(char));
        strcpy(input, buf);
        printf("TEST-PRETOKEN: %s\n", buf);
        tokenize(buf, &args);

        dowork(args, input);

    }
}

// void main (int argc, char** argv) {
// 	int s,len;
//     struct sockaddr_in saddr;
//     struct hostent *hst;
//     char** ops = malloc(sizeof(char**));

//     if (argc!=4) {
// 	printf("\nUsage:\n\t%s <op1> <op2>\n",argv[0]);
// 	exit(1);
//     }

//     // Create the socket
//     if ((s=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0) {
// 	perror("socket");
// 	exit(1);
//     }
//     puts("socket done");

//     // Determine host address by its network name
//     if ((hst=gethostbyname("localhost"))==NULL) {
// 	perror("gethostbyname");
// 	exit(1);
//     }
//     printf("gethostbyname: %u %d\n",*(int*)hst->h_addr,hst->h_length);

//     // Fill structure
//     bcopy(hst->h_addr,&saddr.sin_addr,hst->h_length);
//     saddr.sin_family=AF_INET;
//     saddr.sin_port=htons(16000);

//     // Connect to other socket
//     if (connect(s,(struct sockaddr *)&saddr,sizeof(saddr))<0){
// 	perror("connect");
// 	exit(1);
//     }
//     puts("connect done");

//     // ops[0]=atoi(argv[1]);
//     // ops[1]=atoi(argv[2]);
//     dowork(s, argv);
    
//     // Write (or send) to socket
//     if (write(s,ops,sizeof(ops))<0) {
// 	perror("write");
// 	exit(1);
//     }

//     // Read (or recv) from socket
//     if (read(s,ops,sizeof(ops))<0) {
// 	perror("read");
// 	exit(1);
//     }

//     // printf("%d + %d = %d\n",ops[0],ops[1],ops[2]);

//     // Close the client socket
//     close(s);
// }
