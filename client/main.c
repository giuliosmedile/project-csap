///			MAIN CLIENT CODE
#include "client.h"

#define MAXARGS 64

// The next two are for handling the color output in the console
#define COLOR "\033[0;33m"
#define STD_COL "\033[0m"

int s;              // The socket I'm interacting with (server)
t_user* u = NULL;   // The user that the socket sends after logging in

void restartOnError(int);  //definition of signal handler

// Shows info about the current user
void info() {
    if (u==NULL) {
        printf("[-] You're not logged in.\n");
    } else {
        printf("%s", formatPrintUser(u, ""));
    }
}

// Shows all commands a user can do
void help() {
    printf("Currently supported commands:\n");
    printf("%s", COLOR);
    printf("\tlogin\t\tallows to log in with credentials\n");
    printf("\tlogout\t\tlogs out from the service\n");
    printf("\tinfo\t\tdisplays information about the logged in user\n");
    printf("\tsignup\t\tallows to sign in to the service\n");
    printf("\tadd\t\tadds a user to your addressbook\n");
    printf("\trecord\t\trecords a new voice message\n");
    printf("\tlisten\t\tlistens to incoming voice messages\n");
    printf("\tclear\t\tclears the console screen\n");
    printf("\texit\t\tterminates the program\n");
    printf("%s", STD_COL);

}

char* takeUserInput(char* input) {
    printf("%s", COLOR);

    if (u == NULL) {
        printf("[]: ");
    } else {
        printf("[%s]: ", u->username);
    }
    if (fgets(input,BUF_SIZE,stdin) == NULL) {
        printf("\n");
        exit(0);
    }
    printf("%s", STD_COL);

    return input;
}

// Function that handles and interprets the user's input, to then pass the request to the server
char* interpretInput(char* command, char* output) {
    command[strlen(command)-1] = '\0';

    if (!strcmp(command, "login")) {
        login(output, &u);
    } else if (!strcmp(command, "signup")) {
        signup(output, &u);
    } else if (!strcmp(command, "logout")) {
        logout(output, &u);
    } else if (!strcmp(command, "info")) {
        info();
        strcpy(output, "null");
    } else if (!strcmp(command, "add")) {
        add(output, &u);
    } else if (!strcmp(command, "record")) {
        // Set a filename for the recording
        char* filename = (char*) malloc(sizeof(char) * BUF_SIZE);
        record(output, &u, filename);

        // Send to the socket the instruction to wait for the file
        sendToSocket(s, output);
        sleep(1);

        // To send the file, I need to know the whole path
        char* path = (char*)malloc(BUF_SIZE * sizeof(char));
        sprintf(path, "%s/%s", TMP_DIR, filename);

        // Finally send to the socket the file
        printf("[-] Sending file to %d\n", s);
        sendFile(s, path, get_file_size(path));

        // Finally free "local" variables
        free(filename);
        free(path);

        // I tell the client to skip the next server send, as it's already been done in this function
        strcpy(output, "skipsend");

    } else if (!strcmp(command, "listen")) {
        // Check if logged in
        if (u == NULL) {
            printf("[-] You're not logged in.\n");
            strcpy(output, "null");
            goto endOfTakeUserInput;
        }

        // Ask server to send the listenable files
        char* request = (char*) malloc(sizeof(char) * BUF_SIZE);
        sprintf(request, "listen;%s;", u->username);
        sendToSocket(s, request);

        // Wait for the server to send the messages
        char* response = (char*) malloc(sizeof(char) * BUF_SIZE);
        response = readFromSocket(s, response);

        // Parse the response, creating a list of messages
        NODE* messages = get_list_from_string(response);

        // Check if there actually are messages to be read
        if (count_messages(messages) == 0) {
            printf("[-] No messages to read.\n");
            strcpy(output, "null");
            goto endOfTakeUserInput;
        }
        
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
            strcpy(output, "null");
            goto endOfTakeUserInput;
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
        free(request);
        request = (char*) malloc(sizeof(char) * BUF_SIZE);
        sprintf(request, "get_message;%s;", filename);
        sendToSocket(s, request);

        // Decode metadata from the server, sent before actual message
        char* listen = (char*) malloc(sizeof(char) * BUF_SIZE);
        listen = readFromSocket(s, listen);
        char** args = (char**) malloc(3 * sizeof(char*));
        tokenize(listen, &args);

        // Check if the message was successfully received
        if (!strcmp(args[0], "ERROR") || strcmp(args[0], "listen")) {
            printf("[-] Error while getting the message.\n");
            strcpy(output, "null");
            goto endOfTakeUserInput;
        }

        // Prepare for the message
        free(filename);
        filename = (char*) malloc(sizeof(char) * BUF_SIZE);
        strcpy(filename, args[1]);
        int filesize = atoi(args[2]); 
        char* path = (char*)malloc(BUF_SIZE * sizeof(char));
        sprintf(path, "%s/%s", TMP_DIR, filename);

        // Receive the message
        receiveFile(s, path);

        // Play the message
        play(path);

        // Free the memory
        free(filename);
        free(path);
        free(request);
        free(listen);
        free(args);
        
        // Return null to the main function
        strcpy(output, "null");

    } else if (!strcmp(command, "exit") || !strcmp(command, "quit")) {
        close(s);
        exit(0);
    } else if (!strcmp(command, "help")) {
        help();
        strcpy(output, "null");
    } else if (!strcmp(command, "clear")) {
        system("@cls||clear");
        strcpy(output, "null");
    } else {
    	// Command not found
    	strcpy(output, "null");
	}
    
endOfTakeUserInput:
    return output;
}

void main (int argc, char** argv) {
    char* buf = (char*)malloc(BUF_SIZE * sizeof(char));
    char* command = (char*)malloc(BUF_SIZE * sizeof(char));
    char* response = (char*)malloc(BUF_SIZE * sizeof(char));
    char* output = (char*)malloc(BUF_SIZE * sizeof(char));
    char **args;
    char* str = (char*)malloc(BUF_SIZE * sizeof(char));

    signal(SIGSEGV, restartOnError);

    // Read the configuration file
    char* serv_add = (char*)malloc(sizeof(BUF_SIZE * sizeof(char)));
    unsigned short serv_port;
    readConfig(&serv_add, &serv_port);
    printf("Server address: %s\n", serv_add);

    s = connectToSocket(serv_add, serv_port);

    printf("\t -------------------------------\n");
    printf("\t -------------------------------\n");
    printf("\t ------ Voicemail Service ------\n");
    printf("\t -------------------------------\n");
    printf("\t -------------------------------\n");
    // Main loop
    for (;;) {
        // As some function (such as RECORD) may change the signal behavior, I make sure to reset them at each iteration
        signal(SIGQUIT, SIG_DFL);
        signal(SIGKILL, SIG_DFL); 

        // init variables
        buf = (char*)malloc(BUF_SIZE * sizeof(char));
        command = (char*)malloc(BUF_SIZE * sizeof(char));
        output = (char*)malloc(BUF_SIZE * sizeof(char));
        response = (char*)malloc(BUF_SIZE * sizeof(char));

        // take the user input
        takeUserInput(buf);
        strcpy(command, buf);
        command[strlen(command)-1] = '\0';

        // interpret it basing on the command
        interpretInput(buf, output);
        // if input is invalid (null) restart
        if (!strcmp(output, "null")) {
            free(buf);
            free(command);
            free(output);
            free(response);
        	continue;
        }

        // send the response to the server only if the output is NOT skipsend
        if (strcmp(output, "skipsend")) {
            // send the command and arguments to socket
            sendToSocket(s, output);
        }

        //wait for reply
        free(response);
        response = (char*)malloc(BUF_SIZE * sizeof(char));
        response = readFromSocket(s, buf);

        // Then handle the server response
        u = handleServerReplies(command, response, u);

        // finally reset all variables
        free(buf);
        free(command);
        free(output);
        free(response);

    }
}


// Function that restarts the client from main when fatal signals get caught
// Set to handle most fatal signals (SIGQUIT, SIGKILL, SIGSEGV)
void restartOnError(int signum) {
	printf("\033[0;33mAn error %d has occurred. Restarting software.\033[0m\n", signum);
    close(s);
    u = NULL;
	main(0, NULL);
}