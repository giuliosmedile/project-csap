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
//     char* sender;     // The user that sent the message
//     char* receiver;   // The user that received the message
//     char* filename;     // The file that contains the message
//     time_t timestamp;   // The time the message was sent
//     int is_read;        // The message has been read by the receiver
// };
// typedef struct s_message t_message;


t_message* saveMessage(char* filename) {
    //printf("Filename: %s\n", filename);
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
    char* tmp = (char*)malloc(BUF_SIZE * sizeof(char));
    strcpy(tmp, filename);
    tokenizeWithSeparators(tmp, &arguments, "-:");
    
    // The last argument still has a trailing ".wav" that we don't need
    arguments[7][strlen(arguments[7]) - 4] = '\0';

///
    // Debug tokenization
    printf("String is: %s\n", filename);
    for (int i = 0; i < 8; i++) {
        printf("%d\t\"%s\"\n", i, arguments[i]);
    }
///

puts("1");


    // Initialize the fields and fill in the obvious ones
    message->sender = (char*)malloc(sizeof(t_user));
    strcpy(message->sender, arguments[0]);

    message->receiver = (char*)malloc(sizeof(t_user));
    strcpy(message->receiver, arguments[1]);

    message->filename = (char*)malloc(BUF_SIZE * sizeof(char));
    strcpy(message->filename, filename);

    message->timestamp = (time_t)malloc(sizeof(time_t));
    message->is_read = 0;
    
    // Get the timestamp
    struct tm tmdate = {0};
    tmdate.tm_year = atoi(arguments[2]) - 1900;
    tmdate.tm_mon = atoi(arguments[3]) - 1;
    tmdate.tm_mday = atoi(arguments[4]);
    tmdate.tm_hour = atoi(arguments[5]);
    tmdate.tm_min = atoi(arguments[6]);
    tmdate.tm_sec = atoi(arguments[7]);
    message->timestamp = mktime(&tmdate);

    puts("finished message");

    return message;    
} 

/**
 * Function that flags that a message has been read.
 * This must be the only way that the is_read variable can be changed.
*/
void flagMessageRead(t_message* m) {
    m->is_read = 1;
}
    
/**
 * Function that prints a message to a string
 * @param m The message to print
 * @param string the string to print to
 * @return the string
*/
char* formatPrintMessage(t_message* m, char* string) {
    string = (char*)malloc(BUF_SIZE * sizeof(char));
    sprintf(string, "Sender: %s\n", m->sender);
    sprintf(string, "Receiver: %s\n", m->receiver);
    sprintf(string, "Filename: %s\n", m->filename);
    sprintf(string, "Timestamp: %s\n", ctime(&(m->timestamp)));
    sprintf(string, "Is read: %d\n", m->is_read);

    return string;
}
