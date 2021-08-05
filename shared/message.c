// Type definition and basic funcions implementation for the type that holds a message

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
//#include "user.c"

//#define BUF_SIZE 4096
#define REPO "../datarepo/data/messages/"
#define USERS_REPO "../datarepo/data/users.txt"

// Type definition
// struct s_message {
//     t_user* sender;     // The user that sent the message
//     t_user* receiver;   // The user that received the message
//     char* filename;     // The file that contains the message
//     time_t timestamp;   // The time the message was sent
//     int is_read;        // The message has been read by the receiver
// };
// typedef struct s_message t_message;


t_message* saveMessage(char* filename) {
    // Create a new message
    t_message* message = (t_message*)malloc(sizeof(t_message*));

    // The message string is composed of the following arguments:
    // 1. The sender's username
    // 2. The receiver's username
    // 3. The year the message was sent
    // 4. The month the message was sent
    // 5. The day the message was sent
    // 6. The hour the message was sent
    // 7. The minute the message was sent
    // 8. The second the message was sent

    // Initialize the arguments and tokenize them
    char** arguments = (char**)malloc(8 * sizeof(char*));
    tokenize(filename, &arguments);

///
    // Debug tokenization
    for (int i = 0; i < 8; i++) {
        printf("%d\t%s\n", i, arguments[i]);
    }
///


    // Initialize the fields
    message->sender = (t_user*)malloc(sizeof(t_user));
    message->receiver = (t_user*)malloc(sizeof(t_user));
    message->filename = (char*)malloc(BUF_SIZE * sizeof(char));
    message->timestamp = (time_t)malloc(sizeof(time_t));
    message->is_read = 0;
    

    // Copy the filename, it's the same as the function parameter
    strcpy(message->filename, filename);

    // Find the users
    message->sender = searchUser(arguments[0], USERS_REPO);
    message->receiver = searchUser(arguments[1], USERS_REPO);

    // Get the timestamp
    struct tm tmdate = {0};
    tmdate.tm_year = atoi(arguments[2]) - 1900;
    tmdate.tm_mon = atoi(arguments[3]) - 1;
    tmdate.tm_mday = atoi(arguments[4]);
    tmdate.tm_hour = atoi(arguments[5]);
    tmdate.tm_min = atoi(arguments[6]);
    tmdate.tm_sec = atoi(arguments[7]);
    message->timestamp = mktime(&tmdate);

    return message;    
} 

/**
 * Function that flags that a message has been read.
 * This must be the only way that the is_read variable can be changed.
*/
void flagMessageRead(t_message* m) {
    m->is_read = 1;
}
    
