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
t_message* flagMessageRead(t_message* m) {
    m->is_read = 1;
    return m;
}

/**
 * Function that prints a message to a string. 
 * This function is intended to use to then save this string to a repository, or to be sent to the client. 
 * @param m The message to print
 * @param string The string to print to
*/
char* printMessage(t_message* m, char* string) {
    char* temp = (char*)malloc(BUF_SIZE * sizeof(char));
    string = (char*)malloc(BUF_SIZE * sizeof(char));

    // I print all the parameters to temp, then to string, followed by a ";"
    sprintf(temp, "%s;", m->sender);
    strcat(string, temp);

    sprintf(temp, "%s;", m->receiver);
    strcat(string, temp);

    sprintf(temp, "%d;", (int)m->timestamp);
    strcat(string, temp);

    sprintf(temp, "%d;", m->is_read);
    strcat(string, temp);

    sprintf(temp, "%s;", m->filename);
    strcat(string, temp);

    free(temp);
    return string;
}

/**
 * Function that reads a message from a string. 
 * String must be formatted as the one printed from printMessage.
 * @param string The string to read from
 * @return A pointer to the message read
*/
t_message* readMessage(char* string) {
    char* temp = (char*)malloc(BUF_SIZE * sizeof(char));
    char** arguments = (char**)malloc(10 * sizeof(char*));
    t_message* message = (t_message*)malloc(sizeof(t_message*));

    // I split the string into arguments
    tokenize(string, &arguments);

///
    // Debug tokenization
    printf("String is: %s\n", string);
    for (int i = 0; i < 10; i++) {
        printf("%d\t\"%s\"\n", i, arguments[i]);
    }
///

    // The easiest thing to do here is to just make a string that is readable by saveMessage, then pass it to saveMessage, and read its output
    struct tm* timeinfo = (struct tm*)malloc(sizeof(struct tm));
    time_t timer = atoi(arguments[2]);
    timeinfo = localtime(&timer);

    sprintf(temp, "%s-%s-%d:%d:%d:%d:%d:%d.wav", arguments[0], arguments[1], timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    message = saveMessage(temp);

    // Then see if the message was marked as read
    if (atoi(arguments[3]) == 1) {
        message = flagMessageRead(message);
    }

    free(temp);
    free(arguments);
    return message;
}
    
/**
 * Function that prints a message to a string
 * @param m The message to print
 * @param string the string to print to
 * @return the string
*/
char* formatPrintMessage(t_message* m, char* string) {

    if (m == NULL) return "";

    string = (char*)malloc(BUF_SIZE * sizeof(char));
    char* temp = (char*)malloc(BUF_SIZE * sizeof(char));
    sprintf(temp, "Sender: \t%s\n", m->sender);
    strcat(string, temp);
    sprintf(temp, "Receiver: \t%s\n", m->receiver);
    strcat(string, temp);
    sprintf(temp, "Filename: \t%s\n", m->filename);
    strcat(string, temp);
    sprintf(temp, "Timestamp: \t%s", ctime(&(m->timestamp)));
    strcat(string, temp);
    sprintf(temp, "Is read: \t%d\n", m->is_read);
    strcat(string, temp);

    return string;
}

// Function that saves a message to a file
void saveInRepository(t_message* m, char* filename) {
    FILE* file = fopen(filename, "a");

    if (file == NULL) return;

    char* string = printMessage(m, NULL);
    fprintf(file, "%s\n", string);

    free(string);
    fclose(file);
}

