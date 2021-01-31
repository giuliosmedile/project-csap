#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/wait.h>       /* for waitpid() */
#include <signal.h>         /* for sigaction() */

#include "signin.c"
#include "connection.c"
// #include "../shared/user.c"

// From main.c
void DieWithError(char* str);
void tokenize(char* input, char*** output);
void dowork(int socket);

// From signin.c
int signup(char* username, char* password);
int login(char* username, char* password);

//From user.c
struct s_user;
typedef struct s_user user;
user createUser(char* username);
