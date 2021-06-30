// Function(s) to decode the client requests and potentially send new ones to
// the data repository.
// This function represent the main client-server-dataserver interaction
// of the system.
#define BUF_SIZE 256
int counter = 0;
char* rcvString;
char** ops;
char* command;
char* output;
int result;

void dowork(int servSock, int dataRepoSock) {
    printf("-- Dowork-- \n");

	rcvString = (char*)malloc(BUF_SIZE * sizeof(char));
 	result = 0;

    /* --------------- WAIT FOR INPUT ----------------- */

    printf("[-] Waiting for client...\n");
    // Wait for requests
    readFromSocket(servSock, rcvString);

    ops = (char**)malloc(3*BUF_SIZE);
    command = (char*)malloc(BUF_SIZE * sizeof(char));
    output = (char*)malloc(BUF_SIZE * sizeof(char));

    printf("---------------------- [+] Counter: %d\n", counter);

	printf("[-] Received: \"%s\"\n", rcvString);

    /* --------------- PROCESS INPUT ----------------- */

	// Insert the string received from the socket into the ops array
	tokenize(rcvString, &ops);

	strcpy(command, ops[0]);

     /* --------------- FIRST OPERATIONS ----------------- */
	
	if (strcmp(command, "login") == 0) {
		printf("- Logging in -\n");
		result = login(ops[1], ops[2]);
	}
	if (strcmp(command, "signup") == 0) {
		printf("- Signing up - \n");
		result = signup(ops[1], ops[2]);
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
    }

    printf("[+] about to send to client: \"%s\"\n", output);

     /* --------------- SEND BACK TO CLIENT ----------------- */
    sendToSocket(servSock, output);

    counter++;

	free(ops);
    free(rcvString);
    free(command);
    return;
}