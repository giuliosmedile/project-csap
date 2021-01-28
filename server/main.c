/// 		MAIN SERVER CODE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include "signin.c"

#define BUF_SIZE 64
#define SEPARATOR " \t\r\n\v\f;"

// Functions from signin.c
int signup (char* username, char* password);
int login(char* username, char* password);

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

	char output[BUF_SIZE];
	sprintf(output, "%d", result);
	if (write(socket, output, sizeof(output)) < 0) {
		perror("write");
		exit(1);
	}

	free(operations);

}

int main(int argc, char** argv) {
	int s,c,len;
    struct sockaddr_in saddr;
    //int ops[3];
    int addr;
    int running;
    int maxspawn;

    if ((s=socket(AF_INET,SOCK_STREAM,0))<0) {
		perror("socket");
		exit(1);
    }
    puts("[+]socket done");

    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(16000);
    saddr.sin_addr.s_addr=INADDR_ANY;

    if (bind(s,(struct sockaddr *)&saddr,sizeof(saddr))){
		perror("bind");
		exit(1);
    }
    puts("[+]bind done");

    if (listen(s,5)) {
	perror("listen");
	exit(1);
    }
    puts("[+]listen done");
    
    running=0;
    maxspawn=0;
    
    while (1) {
	len = sizeof (saddr);
	if ((c = accept (s, (struct sockaddr *) &saddr, &len)) < 0) {
	    perror ("accept");
	    exit (1);
	}
	addr = *(int *) &saddr.sin_addr;
	printf ("[+]accept - connection from %d.%d.%d.%d\n",
		(addr & 0xFF), (addr & 0xFF00) >> 8, (addr & 0xFF0000) >> 16,
		(addr & 0xFF000000) >> 24);
	switch (fork()) {
	    case 0:
		//close(s);
			dowork(c);
	    	continue;
	    case -1:
	    	perror("fork");
	    	break;
	    default:
	    	close (c);	    
	    	running +=1;
		if (running > maxspawn) {
		    maxspawn=running;
		    //printf ("== Max:%d\n",maxspawn);
		}
		while (waitpid(-1,NULL, WNOHANG)>0) running -=1;
	    	break;
	}

    }
    close(s);
}