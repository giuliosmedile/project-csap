#define USERS_REPOSITORY "./data/users.txt"         // Users repository
#define MESSAGES_REPO "./data/messages.txt"         // Messages repository

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
    char* sender;     // The user that sent the message
    char* receiver;   // The user that received the message
    char* filename;     // The file that contains the message
    time_t timestamp;   // The time the message was sent
    int is_read;        // The message has been read by the receiver
};

struct node {
	t_message* message;
	struct node* next;
};