// Function(s) to decode the client requests and potentially send new ones to
// the data repository.
// This function represent the main client-server-dataserver interaction
// of the system.

int counter = 0;
char* rcvString;
char** ops;
char* command;
char* output;
int result;

void dowork(int clientSock, int dataRepoSock) {
    startLoop: printf("-- Dowork-- \n");

	rcvString = (char*)malloc(BUF_SIZE * sizeof(char));
 	result = 0;

    /* --------------- WAIT FOR INPUT ----------------- */

    printf("[-] Waiting for client...\n");
    // Wait for requests
    readFromSocket(clientSock, rcvString);

    ops = (char**)malloc(MAX_ARGS*BUF_SIZE);
    for (int i = 0; i < MAX_ARGS; i++) {
        ops[i] = NULL;
    }

    command = (char*)malloc(BUF_SIZE * sizeof(char));
    output = (char*)malloc(BUF_SIZE * sizeof(char));

    printf("---------------------- [+] Counter: %d\n", counter);

	printf("[-] Received: \"%s\"\n", rcvString);

    /* --------------- PROCESS INPUT ----------------- */

	// Insert the string received from the socket into the ops array
	tokenize(rcvString, &ops);

	strcpy(command, ops[0]);

     /* --------------- FIRST OPERATIONS ----------------- */
	
	if (!strcmp(command, "login")) {
		printf("- Logging in -\n");
		result = login(ops[1], ops[2]);
	} else if (!strcmp(command, "signup")) {
		printf("- Signing up - \n");
		result = signup(ops[1], ops[2]);
	} else if (!strcmp(command, "add")) {
        printf("- Adding to addressbook - \n");
        result = add(ops[1], ops[2]);
    } else if (!strcmp(command, "record")) {
        result = record(ops[1], atoi(ops[2]), clientSock);
    }

     /* --------------- COMMUNICATIONS WITH DATA REPO ----------------- */

    // Ask data to the data repository, using the same command string
    if (!strcmp(command, "login")) {
        if (result) {
            // If I logged in correctly, ask the mdr for the user (ops[1])
            char* tmp = malloc(BUF_SIZE * sizeof(char));
            sprintf(tmp, "login;%s", ops[1]);
            printf("tmp: %s\n", tmp);
            sendToSocket(dataRepoSock, tmp);
            // Wait for response from the data repo
            output = readFromSocket(dataRepoSock, output);
        } else {
            // Didn't correctly log in
            output = "NOLOGIN";
        }
    } else if (!strcmp(command, "signup")) {
        if (result) {
            // If I managed to sign up correctly, ask the mdr to register
            // the new user in the database
            char* tmp = malloc(BUF_SIZE * sizeof(char));
            sprintf(tmp, "signup;%s", ops[1]);
            sendToSocket(dataRepoSock, tmp);
            // Wait for response from the data repo
            output = readFromSocket(dataRepoSock, output);
            free(tmp);
        } else {
            // Didn't correctly sign up
            output = "NOSIGNUP";
        }
    } else if (!strcmp(command, "add")) {
        if (result) {
            // If the other user exists, I can safely ask
            // the mdr to add it into the user struct
            char* tmp = malloc(BUF_SIZE * sizeof(char));
            sprintf(tmp, "add;%s;%s", ops[1], ops[2]);
            sendToSocket(dataRepoSock, tmp);
            // Wait for response from the data repo
            output = readFromSocket(dataRepoSock, output);
            free(tmp);
        } else {
            output = "NOADD";
        }
    } else if (!strcmp(command, "record")) {
        if (result) {
            // Recreate the complete path for the file
            char* path = (char*)malloc(BUF_SIZE * sizeof(char));
            sprintf(path, "%s/%s", TMP_DIR, ops[1]);

            // Tell the mdr to wait for the file
            char* tmp = malloc(BUF_SIZE * sizeof(char));
            sprintf(tmp, "record;%s;%s", ops[1], ops[2]);
            sendToSocket(dataRepoSock, tmp);
            sleep(1);

            // Send the file to mdr
            sendFile(dataRepoSock, path, get_file_size(path));

            // Wait for response from the data repo
            output = readFromSocket(dataRepoSock, output);

            // Finally free local variables
            free(tmp);
            free(path);
        } else {
            // There has been an error while receiving the file from the client
            output = "NORECORD";
        }
    } else if (!strcmp(command, "listen")) {
        puts("listen");

        char* tmp = malloc(BUF_SIZE * sizeof(char));
        sprintf(tmp, "get_messages_for;%s", ops[1]);
        sendToSocket(dataRepoSock, tmp);

        // Get the list of messages from the data repo to send it back to the client
        output = readFromSocket(dataRepoSock, output);

    } else if (!strcmp(command, "get_message")) {
        puts("get_message");

        // Ask the mdr for the message
        char* tmp = malloc(BUF_SIZE * sizeof(char));
        sprintf(tmp, "get_message;%s", ops[1]);
        sendToSocket(dataRepoSock, tmp);

        // Wait for the mdr to send the message
        char* filename = (char*)malloc(BUF_SIZE * sizeof(char));
        char* path = (char*)malloc(BUF_SIZE * sizeof(char));
        char** args = (char**)malloc(3 * sizeof(char*));

        // The datarepo response will be in the format:
        // get_message;<filename>;<filesize>
        tmp = readFromSocket(dataRepoSock, tmp);

        // Check if the message is valid
        if (!strcmp(tmp, "NOMESSAGE")) {
            output = "NOMESSAGE";
            goto sendToSocket;
        }

        tokenize(tmp, &args);
        strcpy(filename, args[1]);
        sprintf(path, "%s/%s", TMP_DIR, filename);
        int filesize = atoi(args[2]);
        
        // Listen for the data stream by using record
        if (record(filename, filesize, dataRepoSock) == 0) {
            // Tell the client something went wrong
            output = "ERROR";
            goto sendToSocket;    
        }

        // Send the message to the client
        free(tmp);
        tmp = (char*)malloc(BUF_SIZE * sizeof(char));
        sprintf(tmp, "listen;%s;%d", filename, filesize);
        sendToSocket(clientSock, tmp);
        sleep(1);
        puts("sending file to client");
        sendFile(clientSock, path, filesize);

        // Skip to next iteration
        free(tmp);
        goto endOfDoWork;

    } else if (!strcmp(command, "forward")) {
        puts("forward");

        // Send to mdr the request to forward the message
        char* tmp = malloc(BUF_SIZE * sizeof(char));
        sprintf(tmp, "forward;%s;%s;%s", ops[1], ops[2], ops[3]);

        sendToSocket(dataRepoSock, tmp);

        // Wait for the mdr's response
        output = readFromSocket(dataRepoSock, output);

    } else {
        // If the command is not recognized, send an error
        output = "ERROR";
        
        // Send the error to the client
        sendToSocket(clientSock, output);

        // Skip to next iteration
        goto endOfDoWork;
    }

    printf("[+] about to send to client: \"%s\"\n", output);

     /* --------------- SEND BACK TO CLIENT ----------------- */
sendToSocket:
    sendToSocket(clientSock, output);

    counter++;

endOfDoWork:
	free(ops);
    free(output);
    free(rcvString);
    free(command);
    return;
}