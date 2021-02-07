void dowork(int socket) {
	char** ops = (char**)malloc(3*BUF_SIZE);
	char rcvString[BUF_SIZE];
 	int result;

    // Wait for requests
 	if (read(socket, rcvString, sizeof(rcvString)) < 0) {
		perror("read");
		exit(1);
	}

	printf("Received: %s\n", rcvString);
	// Insert the string received from the socket into the ops array
	tokenize(rcvString, &ops);

	printf("Inside dowork\n");

	char* command = malloc(BUF_SIZE);
	strcpy(command, ops[0]);
	
	if (!strcmp(command, "login")) {
		result = getUser(ops[1]);
	} else if (!strcmp(command, "adduser")) {
		result = addUserToAddressBook(ops[1], ops[2]);
	}

	char* output = (char*)malloc(BUF_SIZE * sizeof(char));
	strcpy(output, result);
	printf("[-]output: %s\n", output);
	if (write(socket, output, sizeof(output)) < 0) {
		perror("write");
		exit(1);
	}

	free(ops);

} 	