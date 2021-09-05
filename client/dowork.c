int s;  /* Socket I'm interacting with */
t_user* u = NULL;

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
    printf("\tforward\t\tforwards a voice message to another user\n");
    printf("\tdelete\t\tdeletes a voice message you have sent\n");
    printf("\tremove\t\tremoves a user from your addressbook\n");
    printf("\tclear\t\tclears the console screen\n");
    printf("\texit\t\tterminates the program\n");
    printf("%s", STD_COL);

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

        if (u->addressbook_size == 0) {
            printf("[-] You don't have any contacts. Add someone before trying to send messages.\n");
            strcpy(output, "null");
            return output;
        }

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
            goto endOfInterpretInput;
        }

        // Ask server to send the listenable files
        char* request = (char*) malloc(sizeof(char) * BUF_SIZE);
        sprintf(request, "listen;%s;", u->username);
        sendToSocket(s, request);

        // Wait for the server to send the messages
        char* response = (char*) malloc(sizeof(char) * BUF_SIZE);
        response = readFromSocket(s, response);

        // Check if server crashed
        if (!strcmp(response, "null")) {
            printf("[-] Server crashed.\n");
            strcpy(output, "null");
            return output;
        }

        askForMessage(request, response);

        if (!strcmp(output, "null")) {
            free(request);
            free(response);
            return output;
        }

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
            return output;
        }

        // Prepare for the message
        char* filename = (char*) malloc(sizeof(char) * BUF_SIZE);
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

    } else if (!strcmp(command, "forward")) {
        
        // Check if logged in
        if (u == NULL) {
            printf("[-] You're not logged in.\n");
            strcpy(output, "null");
            goto endOfInterpretInput;
        }

        // Ask server to send the listenable files
        char* request = (char*) malloc(sizeof(char) * BUF_SIZE);
        sprintf(request, "listen;%s;", u->username);
        sendToSocket(s, request);

        // Wait for the server to send the messages
        char* response = (char*) malloc(sizeof(char) * BUF_SIZE);
        response = readFromSocket(s, response);
        if (!strcmp(response, "null")) {
            printf("[-] Server crashed.\n");
            strcpy(output, "null");
            return output;
        }

        // Check if there actually are messages to be read
        if (!strcmp(response, "") || !strcmp(response, "0")) {
            printf("[-] No messages to read.\n");
            strcpy(output, "null");
            goto endOfInterpretInput;
        }

        // Parse the response, creating a list of messages
        NODE* messages = get_list_from_string(response);

        // Ask the user to choose a message
askForMessageInForward:
        printf("%s", COLOR);
        printf("\n\n%s\n\n", print_list(messages, ""));

        printf("[-] Choose a message to forward: ");
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
            goto endOfInterpretInput;
        }

        // Evaluate the user's choice
        int choice_int = atoi(choice);
        if (choice_int < 1 || choice_int > count_messages(messages)) {
            // If the user's choice is not valid, ask again
            printf("[-] \"%s\" is an invalid choice.\n", choice);
            goto askForMessageInForward;
        }
        printf("%s", STD_COL);

        // Get the message
        t_message* m = getMessage(messages, choice_int-1);
        char* filename = (char*) malloc(sizeof(char) * BUF_SIZE);
        strcpy(filename, m->filename);

        // Ask user to choose a user in their addressbook to forward the message to
        char* userToForwardTo = (char*) malloc(sizeof(char) * BUF_SIZE);
        userToForwardTo = selectUser(u, userToForwardTo);

        if (!strcmp(userToForwardTo, "null") || !strcmp(userToForwardTo, "")) {
            return "null";
        }

        // Send request to server
        sprintf(output, "forward;%s;%s;%s;", u->username, userToForwardTo, filename);

    } else if (!strcmp(command, "delete")) {

        // Check if logged in
        if (u == NULL) {
            printf("[-] You're not logged in.\n");
            strcpy(output, "null");
            goto endOfInterpretInput;
        }
        
        // Check if there actually are messages to be read
        if (u->messagesno == 0) {
            printf("[-] You can only delete messages you sent. Send some messages before deleting them!\n");
            strcpy(output, "null");
            goto endOfInterpretInput;
        }

        // Get the list of messages from the user struct
        NODE* messages = u->messages;

        // Ask the user to choose a message
askForMessageInDelete:
        printf("%s", COLOR);
        printf("\n\n%s\n\n", print_list(messages, ""));
        
        printf("[-] Choose a message to delete: ");
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
            goto endOfInterpretInput;
        }

        // Evaluate the user's choice
        int choice_int = atoi(choice);
        if (choice_int < 1 || choice_int > count_messages(messages)) {
            // If the user's choice is not valid, ask again
            printf("[-] \"%s\" is an invalid choice.\n", choice);
            goto askForMessageInDelete;
        }
        printf("%s", STD_COL);

        // Get the message
        t_message* m = getMessage(messages, choice_int-1);
        char* filename = (char*) malloc(sizeof(char) * BUF_SIZE);

        // Ask user to confirm the deletion
        printf("[-] Are you sure you want to delete \"%s\"? (y/n): ", m->filename);
        char* confirm = (char*) malloc(sizeof(char) * BUF_SIZE);
        if (fgets(confirm,BUF_SIZE,stdin) == NULL) {
            printf("\n");
            exit(0);
        }
        confirm[strlen(confirm)-1] = '\0';
        if (!strcmp("y", confirm) || !strcmp("yes", confirm)) {
            // If the user confirms, send the request to the server
            sprintf(output, "delete;%s;%s;%s;", m->sender, m->receiver, m->filename);
        } else {
            // If the user doesn't confirm, return null
            printf("[-] Aborting operation.\n");
            strcpy(output, "null");
        }


    } else if (!strcmp(command, "remove")) {
        output = removeUser(output, &u);

    } else if (!strcmp(command, "search")) {
        output = search(output, &u);

        sendToSocket(s, output);

        // Wait for the server to send the messages
        char* response = (char*) malloc(sizeof(char) * BUF_SIZE);
        response = readFromSocket(s, response);

        free(output);
        output = (char*) malloc(sizeof(char) * BUF_SIZE);
        askForMessage(output, response);

        if (!strcmp(output, "null")) {
            free(response);
            return output;
        }

        sendToSocket(s, output);

        // Decode metadata from the server, sent before actual message
        char* listen = (char*) malloc(sizeof(char) * BUF_SIZE);
        listen = readFromSocket(s, listen);
        char** args = (char**) malloc(3 * sizeof(char*));
        tokenize(listen, &args);

        // Check if the message was successfully received
        if (!strcmp(args[0], "ERROR") || strcmp(args[0], "listen")) {
            printf("[-] Error while getting the message.\n");
            strcpy(output, "null");
            goto endOfInterpretInput;
        }

        // Prepare for the message
        char* filename = (char*) malloc(sizeof(char) * BUF_SIZE);
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
        free(listen);
        free(args);
        
        // Return null to the main function
        strcpy(output, "null");


    } else if (!strcmp(command, "exit") || !strcmp(command, "quit")) {
        printf("Thank you for using the voicemail service. \n\t ~ So long and thanks for all the fish!\n\n");

        // Tell server I'm quitting
        char* quit = (char*) malloc(sizeof(char) * BUF_SIZE);
        strcpy(quit, "quit");
        sendToSocket(s, quit);

        // Free structs and close socket
        u = NULL;
        free(u);
        close(s);
        exit(0);
    } else if (!strcmp(command, "help")) {
        help();
        strcpy(output, "null");
    } else if (!strcmp(command, "clear")) {
        // Clear terminal screen
        // tested on gnome-terminal, should work on other terminals
        // found the instruction on stackoverflow 
        system("@cls||clear");
        strcpy(output, "null");
    } else {
    	// Command not found
    	strcpy(output, "null");
	}
    
endOfInterpretInput:
    return output;
}


/**
 * Main client loop.
 * @param u The user object to use.
 * @param socket The server socket the client is connected to.
*/
void dowork(t_user* user, int socket) {
        u = user;
        s = socket;

        // As some function (such as RECORD) may change the signal behavior, I make sure to reset them at each iteration
        signal(SIGQUIT, SIG_DFL);
        signal(SIGKILL, SIG_DFL); 

        // init variables
        char* buf = (char*)malloc(BUF_SIZE * sizeof(char));
        char* command = (char*)malloc(BUF_SIZE * sizeof(char));
        char* output = (char*)malloc(BUF_SIZE * sizeof(char));
        char* response = (char*)malloc(BUF_SIZE * sizeof(char));

        // take the user input
        takeUserInput(buf, u);
        strcpy(command, buf);
        command[strlen(command)-1] = '\0';

        // interpret it basing on the command
        output = interpretInput(buf, output);
        // if input is invalid (null) restart
        if (!strcmp(output, "null")) {
            // free(buf);
            // free(command);
            // free(output);
            // free(response);
        	return;
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
