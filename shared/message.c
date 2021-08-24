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

/**
 * Function that creates a message from a filename
 * The filename must be structured as follows:  
 *         <sender>_<receiver>_<timestamp>.wav
 * @param filename The filename of the message
 * @return A message created from the filename
**/
t_message* saveMessage(char* filename) {
    //printf("Filename: %s\n", filename);
    // Create a new message
    t_message* message = (t_message*)malloc(sizeof(t_message));

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
puts("a");
///
    // Debug tokenization
    printf("String is: %s\n", filename);
    for (int i = 0; i < 8; i++) {
        printf("%d\t\"%s\"\n", i, arguments[i]);
    }
///
    // Initialize the fields and fill in the obvious ones
    message->sender = (char*)malloc(BUF_SIZE * sizeof(char));
    strcpy(message->sender, arguments[0]);

    message->receiver = (char*)malloc(BUF_SIZE * sizeof(char));
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
 * The string is formatted as follows: 
 *      <sender>;<receiver>;<timestamp>;<is_read>;<filename>
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
    for (int i = 0; i < 5; i++) {
        printf("%d\t\"%s\"\n", i, arguments[i]);
    }
///

    // The easiest thing to do here is to just make a string that is readable by saveMessage, then pass it to saveMessage, and read its output
    struct tm* timeinfo = (struct tm*)malloc(sizeof(struct tm));
    time_t timer = atoi(arguments[2]);
    timeinfo = localtime(&timer);

    sprintf(temp, "%s-%s-%d:%d:%d:%d:%d:%d.wav", arguments[0], arguments[1], timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour - 1, timeinfo->tm_min, timeinfo->tm_sec);
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
 * Function that prints a message to a string. 
 * Intended to be used for debugging purpose or user interaction.
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

/**
 * Function that removes duplicates from a repo file.
 * @param filename The name of the file to remove duplicates from
 * @param repository The repository to remove duplicates from
**/
void removeDuplicateMessages(char* filename, char* repository) {
    size_t len;
	FILE *fp, *tmp_fp;
	char* buf = malloc(BUF_SIZE * sizeof(char));
    char** args = (char**)malloc(MAX_ADDRESSBOOK_SIZE * sizeof(char*));
    char temp[] = "temp.txt";

    if ((fp = fopen(repository, "r")) == NULL) return;
	if ((tmp_fp = fopen(temp, "w")) == NULL) return;

    // copy all contents to the temporary file except the specific line
    while (getline(&buf, &len, fp) != -1) {
        // If the username of the line I'm looking at is not the one passed in the function
        // I can copy it over. If it is, I skip it
        printf("removeduplicates: %s", buf);
    	if (strstr(buf, filename) == NULL) {
            printf("copying over: %s\n", buf);
    		fprintf(tmp_fp, "%s", buf);
    	}
    }
    fclose(fp);
    fclose(tmp_fp);
    remove(repository);  		    // remove the original file 
    rename(temp, repository); 	// rename the temporary file to original name

}


// Function that saves a message to a file
void saveInRepository(t_message* m, char* repository) {
	// First of all, remove duplicates, if exist
	removeDuplicateMessages(m->filename, repository);

	// Let's see if the user was passed corretly DEBUG
	// printf("%s\n", print_list(u->messages, ""));

	FILE* fp;

	// Open the file to append this user
	if ((fp = fopen(repository, "a")) == NULL) {
		printf("err: could not open file %s", repository);
		return;
	}
	
	// Call the printUser function to print the user
	char* string = (char*)malloc(BUF_SIZE * sizeof(char)); 
	string = printMessage(m, string);

	fprintf(fp, "%s\n", string);
	fclose(fp);
	free(string);
	return;
}

/**
 * Function that retrieves a message from a file.
 * @param filename The name of the file to read from
 * @param name the filename of the message to retrieve
 * @return A pointer to the message read
*/
t_message* getFromRepository(char* filename, char* name) {
    FILE* fp = fopen(filename, "r");
    char* buf = (char*)malloc(BUF_SIZE * sizeof(char));
    size_t len;

    if (fp == NULL) return NULL;
    while (getline(&buf, &len, fp) != -1) {
		
		// If the string is newline terminated, remove '\n'
		if (buf[strlen(buf)-1] == '\n') {
			buf[strlen(buf)-1] = '\0';
		}

        // If the string is the one we want, read the message
        if (strstr(buf, name) != NULL) {
            t_message* message = readMessage(buf);
            free(buf);
            fclose(fp);
            return message;
        }
    }
    free(buf);
    fclose(fp);
    return NULL;
}


/**
 * Checks if a message exists in a given repository
 * @param filename the name of the file to check
 * @param repository the repository to check in
 * @return 1 if the message exists in repository, 0 if it does not
*/
int checkIfMessageExists(char* filename, char* repository) {
    FILE* file = fopen(repository, "r");
    char* line = (char*)malloc(BUF_SIZE * sizeof(char));

    printf("filename: \"%s\", repository: %s\n", filename, repository);

    if (file == NULL) return 0;
    while ((line = fgets(line, BUF_SIZE, file)) != NULL) {

        line[strlen(line)-1] = '\0';

        printf("line: \"%s\"; strstr: %s\n", line, strstr(line, filename));
        // If the line contains the filename, return 1
        if (strstr(line, filename) != NULL) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}