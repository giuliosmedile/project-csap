#define BUF_SIZE 256

#define USERS_REPO "./data/users.txt"
#define MESSAGES_REPO "./data/messages.txt"


void dowork(int socket) {
	char** ops = (char**)malloc(3*BUF_SIZE);
	char* rcvString = (char*)malloc(BUF_SIZE * sizeof(char));
	char* command = (char*)malloc(BUF_SIZE * sizeof(char));
 	char* result;
	t_user* user;

    // Wait for requests
 	if (read(socket, rcvString, BUF_SIZE) < 0) {
		perror("read");
		exit(1);
	}

	printf("Received: \"%s\"\n", rcvString);
	// Insert the string received from the socket into the ops array
	tokenize(rcvString, &ops);

	printf("Inside dowork\n");
	printf("ops: %s\n", ops[0]);
	sprintf(command, "%s", ops[0]);
	//strcpy(command, ops[0]);
	printf("command: %s\n", command);


	// Tell messages.c to set the messages repo as REPO
	printf("pre\n");
	setMessagesRepository(MESSAGES_REPO);
	printf("post\n");


	if (!strcmp(command, "login")) {
		printf("login\n");
		user = searchUser(ops[1], USERS_REPO);
		result = getUser(ops[1], USERS_REPO);
		printf("end login\n");
	} else if (!strcmp(command, "adduser")) {
		printf("adduser\n");
		user = searchUser(ops[1], USERS_REPO);
		sprintf(result, "%d", addUserToAddressBook(user, ops[2]));
		printf("end adduser\n");
	}

	char* output = (char*)malloc(BUF_SIZE * sizeof(char));
	strcpy(output, result);
	printf("[-]output: %s\n", output);
	if (write(socket, output, sizeof(output)) < 0) {
		perror("write");
		exit(1);
	}

	free(ops);
	free(rcvString);
	free(result);
	free(user);

} 	