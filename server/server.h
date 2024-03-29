#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/wait.h>       /* for waitpid() */
#include <signal.h>         /* for sigaction() */
#include <fcntl.h>      /* for fcntl() */

#define BUF_SIZE 4096
#define MAX_DATAREPO 10
#define CREDENTIALS_REPO "./data/credentials"

// DEBUG STATEMENT
#ifdef DEBUG
  #define DEBUGPRINT(a) printf a
#else
  #define DEBUGPRINT(a) (void)0
#endif

/// SHARED

#include "../shared/structs.h"
#include "../shared/utils.c"
#include "../shared/list.c"
#include "../shared/user.c"
#include "../shared/message.c"
//#include "../shared/connection.c"

/// OWN IMPORTS

#include "signin.c"
#include "connection.c"
#include "readConfig.c"
#include "messages.c"
#include "consensus.c"
#include "dowork.c"

// // From main.c
// void DieWithError(char* str);
// void tokenize(char* input, char*** output);

// // From signin.c
// int signup(char* username, char* password);
// int login(char* username, char* password);

// // From messages.c
// int add(char* user, char* other);
// int record(char* filename, int socket);

// // From readConfig.c
// void readConfig (int* port, int* clients, int* mds, char*** mds_addr);

// // From dowork.c
// void dowork(int servSock, int dataRepoSock);

