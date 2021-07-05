#define BUF_SIZE 256

#define USERS_REPO "./data/users.txt"
#define MESSAGES_REPO "./data/messages.txt"


void dowork(int socket) {
	char* rcvString = (char*)malloc(BUF_SIZE * sizeof(char));

    // Wait for requests
 	if (read(socket, rcvString, BUF_SIZE) < 0) {
		perror("read");
		exit(1);
	}

	char** ops = (char**)malloc(3*BUF_SIZE);
	char* command = (char*)malloc(BUF_SIZE * sizeof(char));
 	char* result;
	t_user* user;

	printf("[-] Received: \"%s\"\n", rcvString);
	// Insert the string received from the socket into the ops array
	tokenize(rcvString, &ops);

	printf("Inside dowork\n");
	printf("ops: \"%s\"\n", ops[0]);
	sprintf(command, "%s", ops[0]);


	// TODO: Tell messages.c to set the messages repo as REPO
	//setMessagesRepository(MESSAGES_REPO);

	// HANDLE LOGIN
	if (!strcmp(command, "login")) {
		printf("login\n");
		result = getUser(ops[1], USERS_REPO);
		if (result[strlen(result)-1] = '\n') result[strlen(result)-1] = '\0';	//Make sure the string is null terminated, not \n terminated		
		printf("end login: \"%s\"\n", result);

	// HANDLE SIGNUP
	} else if (!strcmp(command, "signup")) {
		printf("signup\n");
		user = createUser(ops[1]);
		saveUser(user, USERS_REPO);
		result = getUser(user->username, USERS_REPO);

		printf("result: \"%s\"\n", result);
		printf("end signups\n");
	
	// HANDLE ADD
	} else if (!strcmp(command, "add")) {
		printf("add\n");
		user = searchUser(ops[1], USERS_REPO);
		printf("\t1\n");
		user = addUserToAddressBook(user, ops[2]);
		printf("\t%s\n", formatPrintUser(user, ""));
		saveUser(user, USERS_REPO);
		printf("\t3\n");
		result = getUser(user->username, USERS_REPO);

		printf("result: \"%s\"\n", result);
		printf("end add\n");

	// HANDLE DEFAULT
	} else {
		printf("operation not supported");
		result = "noop";
	}
	
	sendToSocket(socket, result);

	free(ops);
	free(rcvString);
	free(command);
	free(result);
	free(user);

} 	