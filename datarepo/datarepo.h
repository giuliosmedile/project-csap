///      LIBRARY IMPORTS

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdbool.h>

/// SHARED IMPORTS

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


///      IMPORTS
#include "messages.c"
#include "connection.c"
#include "readConfig.c"
#include "processRequest.c"
#include "handleSocketReplies.c"
#include "dowork.c"


#define BUF_SIZE 256

///      FUNCTION DECLARATIONS

// From utils.c
void tokenize(char* input, char*** output);

// Functions from readConfig.c
void readConfig (short unsigned int* serv_port);

// Functions from connection.c
int connectToSocket(char* serv_add, unsigned short port);
void sendToSocket(int s, char* buf);
char* readFromSocket(int s, char* rcv);
void sendfFile(int s, char* filename);
void receiveFile(int s, char* filename);

// Functions from processRequest.c
char* processRequest(int s, char* request);

// Functions from messages.c
void setMessagesRepository(char* input);
char* searchMessage(char* username, int messageno);
void deleteMessage(char* username, int messageno);

