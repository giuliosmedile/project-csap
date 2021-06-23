#define BUF_SIZE 256

#define USERS_REPO "./data/users.txt"
#define MESSAGES_REPO "./data/messages.txt"


void dowork(int socket) {
	char** ops = (char**)malloc(3*BUF_SIZE);
	char rcvString[BUF_SIZE];
 	char* result;
	t_user* user;

    // Wait for requests
 	if (read(socket, rcvString, sizeof(rcvString)) < 0) {
		perror("read");
		exit(1);
	}

	printf("Received: %s\n", rcvString);
	// Insert the string received from the socket into the ops array
	tokenize(rcvString, &ops);

	printf("Inside dowork\n");

	// Tell messages.c to set the messages repo as REPO
	setMessagesRepository(MESSAGES_REPO);

	char* command = malloc(BUF_SIZE);
	strcpy(command, ops[0]);
	
	if (!strcmp(command, "login")) {
		user = searchUser(ops[1], USERS_REPO);
		result = getUser(ops[1], USERS_REPO);
	} else if (!strcmp(command, "adduser")) {
		user = searchUser(ops[1], USERS_REPO);
		sprintf(result, "%d", addUserToAddressBook(user, ops[2]));
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