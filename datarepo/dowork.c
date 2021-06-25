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


	// Tell messages.c to set the messages repo as REPO
	setMessagesRepository(MESSAGES_REPO);

	if (!strcmp(command, "login")) {
		printf("login\n");
		result = getUser(ops[1], USERS_REPO);
		printf("end login: %s\n", result);
	} else if (!strcmp(command, "signup")) {
		printf("signup\n");
		user = createUser(ops[1]);
		saveUser(user, USERS_REPO);
		result = getUser(user->username, USERS_REPO);
		printf("result: \"%s\"\n", result);
		printf("end signups\n");
	} else {
		printf("operation not supported");
		result = "noop";
	}


	if (write(socket, result, strlen(result)+1) < 0) {
		perror("write");
		exit(1);
	}

	free(ops);
	free(rcvString);
	free(result);
	free(user);

} 	