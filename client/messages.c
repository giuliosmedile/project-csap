// FUNCTIONS TO HANDLE MESSAGES AND ADDRESSBOOK

// #include <stdlib.h>
// #include <time.h>
// #include <stdio.h>
// #include <unistd.h>
// #include <string.h>
// #include <sys/types.h>
// #include <sys/wait.h>
// #include <sys/stat.h>
// #include <malloc.h>

#define true 1
#define false 0

void interceptSigInt(int signum) {
	printf("\n");
}

/**
 * Function to check if this machine can run a certain command 
 * Shamelessly stolen from https://stackoverflow.com/questions/41230547/check-if-program-is-installed-in-c
 * @cmd: the command i am checking
 * @returns true, if current user can run the command, false otherwise
*/
int can_run_command(const char *cmd) {
    if(strchr(cmd, '/')) {
        // if cmd includes a slash, no path search must be performed,
        // go straight to checking if it's executable
        return access(cmd, X_OK)==0;
    }
    const char *path = getenv("PATH");
    if(!path) return false; // something is horribly wrong...
    // we are sure we won't need a buffer any longer
    char *buf = malloc(strlen(path)+strlen(cmd)+3);
    if(!buf) return false; // actually useless, see comment
    // loop as long as we have stuff to examine in path
    for(; *path; ++path) {
        // start from the beginning of the buffer
        char *p = buf;
        // copy in buf the current path element
        for(; *path && *path!=':'; ++path,++p) {
            *p = *path;
        }
        // empty path entries are treated like "."
        if(p==buf) *p++='.';
        // slash and command name
        if(p[-1]!='/') *p++='/';
        strcpy(p, cmd);
        // check if we can execute it
        if(access(buf, X_OK)==0) {
            free(buf);
            return true;
        }
        // quit at last cycle
        if(!*path) break;
    }
    // not found
    free(buf);
    return false;
}

/* Function to add a user to the currently logged in user's addressbook
 * @param result the string that will be sent to the server 
 * @param u_p pointer to the current user
*/
char* add(char* result, t_user** u_p) {
    // the new user i am trying to add
    char* other = (char*)malloc(BUF_SIZE * sizeof(char));
    

    printf("%s", COLOR);

    // Check if i am logged in
    if (*u_p == NULL) {
        printf("[!] Before adding a user to your addressbook, perhaps you should try logging in first.\n");
        strcpy(result, "null");
        return result;
    }

    printf("Username of the user you want to add: ");
	if (fgets(other,BUF_SIZE,stdin) == NULL) {
        strcpy(result, "null");
        return result;
    }
    other[strlen(other)-1] ='\0';

    DEBUGPRINT(("before checks in add\n"));
    // Check if the user is already in the addressbook
    if (isInAddressBook(other, *u_p)) {
        printf("[!] The user you are trying to add is already in your addressbook.\n");
        strcpy(result, "null");
        return result;
    }
    // Check if the user is myself, you never know...
    if (strcmp(other, (*u_p)->username) == 0) {
        printf("[!] You cannot add yourself to your addressbook.\n");
        strcpy(result, "null");
        return result;
    }
    DEBUGPRINT(("after checks in add\n"));

    sprintf(result, "add %s %s", (*u_p)->username, other);
    return result;

}

/**
 * Function to remove a user from the currently logged in user's addressbook
 * @param result the string that will be sent to the server
 * @param u_p pointer to the current user
 * @returns the result string
**/
char* removeUser(char* result, t_user** u_p) {
    // the user i am trying to remove
    char* other = (char*)malloc(BUF_SIZE * sizeof(char));

    printf("%s", COLOR);

    // Check if i am logged in
    if (*u_p == NULL) {
        printf("[!] Before removing a user from your addressbook, perhaps you should try logging in first.\n");
        return "null";
    }

    if ((*u_p)->addressbook_size == 0 || (*u_p)->addressbook == NULL) {
        printf("[!] Your addressbook is empty.\n");
        return "null";
    }

    other = selectUser(*u_p, other);
    DEBUGPRINT(("Selected %s\n", other));

    sprintf(result, "remove;%s;%s", (*u_p)->username, other);
    return result;
}

/** 
 * Client function for message recording
 * @param result the string that will be sent to the server
 * @param u_p pointer to the current user
 * @param file the filename of the file that will be recorded
*/
char* record(char* result, t_user** u_p, char* file) {

    char* path = (char*)malloc(BUF_SIZE * sizeof(char));        // The file path in which the temp recording will be
    char* other = (char*)malloc(BUF_SIZE * sizeof(char));       // user who will receive the file
    char *args[3];                                              // Arguments for rec command
    int pid, status;
    int res;

    // to exit rec, a SIGINT must be sent, so we intercept it and do nothing, or else the program will exit
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = interceptSigInt;
    sigaction(SIGINT, &act, NULL);

    printf("%s", COLOR);
    // Check if i am logged in
    if (*u_p == NULL) {
        printf("[!] Before recording a message, perhaps you should try logging in first.\n");
        sprintf(result, "null");
        return "null";
    }
    // Check if i can run rec
    if (!can_run_command("rec")) {
        printf("[!] Be sure to have installed rec and play through\n\tsudo apt install sox\n");
        sprintf(result, "null");
        return "null";
    }
    // Create the temp directory, it if is not there
    struct stat st = {0};
    if (stat(TMP_DIR, &st) == -1) {
        mkdir(TMP_DIR, 0755);
    }
    // Take input
    selectUser(*u_p, other);
    

    time_t rawtime;
    time (&rawtime);
    struct tm* timeinfo = (struct tm*)malloc(sizeof(struct tm));
    timeinfo = localtime (&rawtime);
    
    // The file name is set as follows:
    // sender_receiver-YYYY-MM-DD-HH:MM:SS.wav
    sprintf(file, "%s-%s-%d:%d:%d:%d:%d:%d.wav", (*u_p)->username, other, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    sprintf(path, "%s/%s", TMP_DIR, file);
    // Fork child to record audio message
    switch(pid=fork()) {
        // Error
        case -1:
            perror("fork");
            exit(1);

        // Child
        case 0:
            ;
            // Exec program
            args[0] = REC;
            args[1] = path;
            args[2] = NULL;     // must be here or else execvp won't work...

            if (res = execvp(REC, args) < 0) {
                // ?? Exec failed
                fprintf(stderr,"%s:", REC);
                perror("execvp");
                exit(1);
            } else break;     

        // Parent
        default:
        	// Let's wait for the child to finish...
            pid = wait(&status);
            printf("[-] Recorded audio at\n\t%s\n", path);
            printf("%s", STD_COL);

            // Reset signal handler to default
            act.sa_handler = SIG_DFL;
            sigaction(SIGINT, &act, NULL);
            

            // Data is then formatted to be sent to the server as
            // record;[filename];[hash]
            char* hash = getFileHash(path);
            sprintf(result, "record;%s;%s", file, hash);
            return result;
    }

    // This won't be reached    
}

/**
 * Function that plays a message received from the server.
 * @param filename the file to be played
**/
void play(char* filename) {

    char* path = (char*)malloc(BUF_SIZE * sizeof(char));        // The file path in which the temp recording will be
    char* other = (char*)malloc(BUF_SIZE * sizeof(char));       // user who will receive the file
    char *args[3];                                              // Arguments for rec command
    int pid, status;
    int res;

    switch(pid=fork()) {
        // Error
        case -1:
            perror("fork");
            exit(1);

        // Child
        case 0:
            ;
            // Exec program
            args[0] = PLAY;
            args[1] = filename;
            args[2] = NULL;     // must be here or else execvp won't work...

            res = execvp(PLAY, args);

            // ?? Exec failed
            fprintf(stderr,"%s:", PLAY);
            perror("execvp");
            exit(1);
            break;       

        // Parent
        default:

        	// Let's wait for the child to finish...
            pid = wait(&status);
            break;
    }
}

/**
 * Function that receives input from the user to ask server to look for messages in a certain time range.
 * @param u_p pointer to the current user
 * @param start_p pointer to the start time
 * @param end_p pointer to the end time
 * @returns both times as pointers, to be used as request for the server
**/
char* search(char* result, t_user** u_p) {
    DEBUGPRINT(("inside search\n"));

    time_t start, end;

    // Start fancy color
    printf("%s", COLOR);

    // Check if i am logged in
    if (*u_p == NULL) {
        printf("[!] Before searching for messages, perhaps you should try logging in first.\n");
        sprintf(result, "null");
        return "null";
    }

    // Take input
    printf("Enter the first time in the range. \n");
    start = getDate();
    DEBUGPRINT(( "start: %s\n", ctime(&start)));

    printf("Enter the second time in the range. \n");
    end = getDate();
    DEBUGPRINT(( "end: %s\n", ctime(&end)));

    // End fancy color
    printf("%s", STD_COL);

    // Check if start is before end
    if (start > end) {
        time_t tmp = start;
        start = end;
        end = tmp;
    }

    // If the times are exactly the same, add 1 second to the end, just to make them different
    if (start == end) {
        end += 1;
    }

    // Format the times to be sent to the server
    sprintf(result, "search;%s;%ld;%ld", (*u_p)->username, start, end);
    return result;
}


/**
 * Function that allows a user to select a message in a list of messages.
 * 
 * @param result the string to be returned to the server
 * @param response the response from the server
 * @returns result
**/
char* askForMessage(char* result, char* response) {
        // Check if there actually are messages to be read
        if (!strcmp(response, "") || !strcmp(response, "0")) {
            printf("[-] No messages to read.\n");
            strcpy(result, "null");
            return result;
        }

        // Parse the response, creating a list of messages
        NODE* messages = get_list_from_string(response);
        
        // Ask the user to choose a message
askForMessage:
        printf("%s", COLOR);
        printf("\n\n%s\n\n", print_list(messages, ""));

        printf("[-] Choose a message to listen to: ");
        char* choice = (char*) malloc(sizeof(char) * BUF_SIZE);
        if (fgets(choice,BUF_SIZE,stdin) == NULL) {
            printf("\n");
            exit(0);
        }
        choice[strlen(choice)-1] = '\0';

        // Check if the user wants to exit
        if (!strcmp("exit", choice) || !strcmp("quit", choice) || !strcmp("cancel", choice)) {
            printf("[-] Cancelling operation.\n");
            strcpy(result, "null");
            return result;
        }

        // Evaluate the user's choice
        int choice_int = atoi(choice);
        if (choice_int < 1 || choice_int > count_messages(messages)) {
            // If the user's choice is not valid, ask again
            printf("[-] \"%s\" is an invalid choice.\n", choice);
            goto askForMessage;
        }
        printf("%s", STD_COL);

        // Get the message
        t_message* m = getMessage(messages, choice_int-1);
        char* filename = (char*) malloc(sizeof(char) * BUF_SIZE);
        strcpy(filename, m->filename);

        // At this point, ask the server to actually send the message
        sprintf(result, "get_message;%s;", filename);
        return result;
}