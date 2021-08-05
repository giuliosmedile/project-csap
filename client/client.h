#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

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




//#include "../shared/connection.c"

#include "signin.c"
#include "connection.c"
#include "readConfig.c"
#include "handleServerReplies.c"
#include "messages.c"


// Standard Definitions
#define BUF_SIZE 256

// The next two are for handling the color output in the console
#define COLOR "\033[0;33m"
#define STD_COL "\033[0m"

// From main.c

//void dowork(char* buf);                      //command interpreter
char* takeUserInput(char* input);                    //take a user's input from keyboard
char* interpretInput(char* command, char* output);   //interpret the command

// From utils.c

void tokenize(char* input, char*** output);  //splits a string into an array of strings

// From signin.c

char* login(char* result, t_user** u);
char* signup(char* result, t_user** u);
char* logout(char* result, t_user** u);

// From connectToSocket.c

int connectToSocket(char* serv_add, unsigned short serv_port);
void sendToSocket(int socket, char* str);
char* readFromSocket(int socket, char* str);
void sendFile(int s, char* filename, int filesize);
void receiveFile(int s, char* filename);

// From handleServerReplies.c

t_user* handleServerReplies(char* command, char* response, t_user* u);

// From readConfig.c

void readConfig (char** serv_add, unsigned short* serv_port);

//From messages.c

char* add(char* output, t_user** u_p);
char* record(char* result, t_user** u_p, char* filename);

