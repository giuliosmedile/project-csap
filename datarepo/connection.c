// FUNCTIONS FOR CONNECTING CLIENTS AND SERVER
#define MAXPENDING 10

void DieWithError(char* str) {
    perror(str);
    exit(1);
}

int CreateTCPServerSocket(unsigned short port)
{
    int sock;                        /* socket to create */
    struct sockaddr_in echoServAddr; /* Local address */

    /* Create socket for incoming connections */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Set options */
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        DieWithError("setsockopt(SO_REUSEADDR) failed");
      
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(port);              /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(sock, MAXPENDING) < 0)
        DieWithError("listen() failed");

    return sock;
}

int AcceptTCPConnection(int servSock)
{
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int clntLen;            /* Length of client address data structure */

    /* Set the size of the in-out parameter */
    clntLen = sizeof(echoClntAddr);
    
    /* Wait for a client to connect */
    if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, 
           &clntLen)) < 0)
        DieWithError("accept() failed");
    
    /* clntSock is connected to a client! */
    
    printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

    return clntSock;
}

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

    // Connect to other socket
    if (connect(s,(struct sockaddr *)&saddr,sizeof(saddr))<0){
    perror("connect");
    exit(1);
    }
    puts("connect done");

    return s;
}

void sendToSocket(int s, char* buf) {
    printf("[-] About to send to socket %d: \"%s\"\n", s, buf);
    buf[strlen(buf)] = '\0';
    // Write (or send) to socket
    if (write(s, buf, strlen(buf)+1)<0) {
        perror("write");
        exit(1);
    }
    
    return;
}

char* readFromSocket(int s, char* rcv) {
    free(rcv);
    rcv = (char*)malloc(BUF_SIZE * sizeof(char));
    // Read (or recv) from socket
    if (read(s, rcv, BUF_SIZE)<0) {
        perror("read");
        exit(1);
    }
    //rcv[strlen(rcv)-1] = '\0';
    printf("[+] received from %d: \"%s\"\n", s, rcv);
    return rcv;
}

// Function that sends a file through a socket
void sendFile(int s, char* filename, int filesize) {
    int n;                                  // Number of bytes sent
    int i = 0;                              // Debug counter
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
    while(fread(data, 1, filesize, fp) > 0) {
        // Send the data
        if (send(s, (void* )data, filesize, 0) == -1) {
            perror("[-]Error in sending file.");
            exit(1);
        }

        bzero(data, BUF_SIZE);
    }

    // Close the file
    fclose(fp);
    return;
}

// Function to receive a binary file sent through a socket
void receiveFile(int s, char* filename) {
    int n = 1;                                        // Number of bytes read
    int i = 0;                                        // Debug counter
    FILE *fp;                                         // File pointer
    char buffer[BUF_SIZE];                            // Buffer to store data

    // Open file
    fp = fopen(filename, "wb");

    // While there is data to be read
    while (n > 0) {
        // Read from socket
        n = recv(s, (void*)buffer, BUF_SIZE, 0);
        printf("%d\t%d\n", ++i, n);
        // If data was received less than BUF_SIZE, the file is over
        if (n < BUF_SIZE) {
            // Write the remaining data and save the file
            fwrite(buffer, 1, n, fp);
            
            fclose(fp);
            return;
        }

        // Write to file
        fwrite(buffer, 1, n, fp);
        
        // Reset for next loop
        bzero(buffer, BUF_SIZE);
    }

    // This won't be reached, but still...
    return;
}