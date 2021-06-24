// Function(s) to decode the client requests and potentially send new ones to
// the data repository.
// This function represent the main client-server-dataserver interaction
// of the system.
#define BUF_SIZE 256

void dowork(int servSock, int dataRepoSock) {
	char** ops = (char**)malloc(3*BUF_SIZE);
    char* command = (char*)malloc(BUF_SIZE * sizeof(char));
    char* output = (char*)malloc(BUF_SIZE * sizeof(char));
	char* rcvString = (char*)malloc(BUF_SIZE * sizeof(char));
 	int result = 0;

    // Wait for requests
 	if (read(servSock, rcvString, BUF_SIZE) < 0) {
		perror("read");
		exit(1);
	}

	printf("Received: \"%s\"\n", rcvString);
	// Insert the string received from the socket into the ops array
	tokenize(rcvString, &ops);

	strcpy(command, ops[0]);
	
	if (strcmp(command, "login") == 0) {
		printf("- Logging in -\n");
		result = login(ops[1], ops[2]);
	}
	if (strcmp(command, "signup") == 0) {
		printf("- Signing up - \n");
		result = signup(ops[1], ops[2]);
	}

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
            output = "-1";
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
        } else {
            // Didn't correctly sign up
            output = "-1";
        }
    }

    printf("[]output: \"%s\"\n", output);

	// sprintf(output, "%d", result);
	printf("[-]output: %s\n", output);
	if (write(servSock, output, BUF_SIZE) < 0) {
		perror("write");
		exit(1);
	}

	free(ops);
    free(rcvString);
    free(command);
    free(output);

}