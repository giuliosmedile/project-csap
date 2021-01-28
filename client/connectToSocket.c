// CODE THAT ALLOWS THE CLIENT TO CONNECT TO A SERVER

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

int connectToSocket() {
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

    // Determine host address by its network name
    if ((hst=gethostbyname("localhost"))==NULL) {
	perror("gethostbyname");
	exit(1);
    }
    printf("gethostbyname: %u %d\n",*(int*)hst->h_addr,hst->h_length);

    // Fill structure
    bcopy(hst->h_addr,&saddr.sin_addr,hst->h_length);
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