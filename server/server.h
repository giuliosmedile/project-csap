#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/wait.h>       /* for waitpid() */
#include <signal.h>         /* for sigaction() */

/// SHARED STRUCTS

typedef struct node NODE;
typedef struct s_user t_user;
typedef struct s_message t_message;

struct s_user {
	char* username;			// the user's username
	int addressbook_size;	// how many users this user has in their addressbook
	int messagesno;			// the number of messages the user has sent
	NODE* messages;			// the user's messages
	char* addressbook[];	// the user's addressbook
};

struct s_message {
    t_user* sender;     // The user that sent the message
    t_user* receiver;   // The user that received the message
    char* filename;     // The file that contains the message
    time_t timestamp;   // The time the message was sent
    int is_read;        // The message has been read by the receiver
};

struct node {
	t_message* message;
	struct node* next;
};

#include "../shared/utils.c"
#include "../shared/list.c"
#include "../shared/user.c"
#include "../shared/message.c"

/// OWN IMPORTS

#include "signin.c"
#include "connection.c"
#include "readConfig.c"
#include "messages.c"
#include "dowork.c"

// From main.c
void DieWithError(char* str);
void tokenize(char* input, char*** output);

// From signin.c
int signup(char* username, char* password);
int login(char* username, char* password);

// From messages.c
int add(char* user, char* other);
int record(char* filename, int socket);

// From readConfig.c
void readConfig (int* port, int* clients, int* mds, char*** mds_addr);

// From dowork.c
void dowork(int servSock, int dataRepoSock);

