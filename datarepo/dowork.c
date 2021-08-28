
void dowork(int socket) {
	puts("[] Inside dowork\n");
	char* rcvString = (char*)malloc(BUF_SIZE * sizeof(char));

    // Wait for requests
 	if (read(socket, rcvString, BUF_SIZE) < 0) {
		perror("read");
		exit(1);
	}

	char** ops = (char**)malloc(10*BUF_SIZE);
	char* command = (char*)malloc(BUF_SIZE * sizeof(char));
 	char* result;
	t_user* user;

	printf("[-] Received: \"%s\"\n", rcvString);
	// Insert the string received from the socket into the ops array
	tokenize(rcvString, &ops);

	printf("Inside dowork\n");
	printf("ops: \"%s\"\n", ops[0]);
	sprintf(command, "%s", ops[0]);

	// HANDLE LOGIN
	if (!strcmp(command, "login")) {
		printf("login\n");
		result = getUser(ops[1], USERS_REPOSITORY);
		if (result == NULL) {
			result = "USERNOTFOUND";
		} else {
			if (result[strlen(result)-1] = '\n') result[strlen(result)-1] = '\0';	//Make sure the string is null terminated, not \n terminated		
			printf("end login: \"%s\"\n", result);
		}

	// HANDLE SIGNUP
	} else if (!strcmp(command, "signup")) {
		printf("signup\n");
		user = createUser(ops[1]);
		saveUser(user, USERS_REPOSITORY);
		result = getUser(user->username, USERS_REPOSITORY);

		printf("result: \"%s\"\n", result);
		printf("end signups\n");
		free(user);
	
	// HANDLE ADD
	} else if (!strcmp(command, "add")) {
		printf("add\n");
		user = searchUser(ops[1], USERS_REPOSITORY);
		printf("\t1\n");
		user = addUserToAddressBook(user, ops[2]);
		printf("\t%s\n", formatPrintUser(user, ""));
		saveUser(user, USERS_REPOSITORY);
		printf("\t3\n");
		result = getUser(user->username, USERS_REPOSITORY);

		printf("result: \"%s\"\n", result);
		printf("end add\n");
		free(user);

	// HANDLE RECORD
	} else if (!strcmp(command, "record")) {
		printf("record\n");

		// Create the temp directory, it if is not there
		struct stat st = {0};
		if (stat(TMP_DIR, &st) == -1) {
			mkdir(TMP_DIR, 0755);
		}

		// Define the path I'll save into
		char* path = (char*)malloc(BUF_SIZE * sizeof(char));
		sprintf(path, "%s/%s", TMP_DIR, ops[1]);

		// Wait for the file from the socket
		receiveFile(socket, path);

		// If the file size is not the same, it means the file was not sent correctly
		if (get_file_size(path) != atoi(ops[2])) {
			result = "FILESIZEERROR";
		} else {            
			// To get the username of the sender, I need to tokenize ops[1]
			char* tmp = (char*)malloc(BUF_SIZE * sizeof(char));
			char** tmp_ops = (char**)malloc(10*BUF_SIZE);
			strcpy(tmp, ops[1]);
			tokenizeWithSeparators(tmp, &tmp_ops, "-:");
			char* user = (char*)malloc(BUF_SIZE * sizeof(char));
			sprintf(user, "%s", tmp_ops[0]);
			
			// Update the sender's messages
			t_user* u = (t_user*)malloc(sizeof(t_user));
			u = searchUser(user, USERS_REPOSITORY);
			u = addMessageToUser(u, ops[1]);

			// Update the user on the repo
			saveUser(u, USERS_REPOSITORY);

			// Move the file on the final directory
			// TODO? Nah.
			
			// Send the result back to server
			result = getUser(user, USERS_REPOSITORY);
			result[strlen(result)-1] = '\0'; 			// Make sure the string is null terminated, not \n terminated
			printf("result: \"%s\"\n", result);
			printf("end record\n");
			free(tmp);
			free(tmp_ops);
			free(user);
		}
		free(path);

	// HANDLE FIRST PART OF LISTEN AND FORWARD
	} else if (!strcmp(command, "get_messages_for")) {
		printf("get_messages_for\n");

		// // Get all the messages that the user has received
		// puts("before getting messages");
		// NODE* listOfMessages = getByReceiverFromFile(MESSAGES_REPO, ops[1]);
		// puts("after getting messages");
		// char* listOfMessagesString = print_list_to_string(listOfMessages);
		// printf("list of messages: %s\n", listOfMessagesString);

		char* listOfMessagesString = getByReceiverFromFile(MESSAGES_REPO, ops[1]);

		// Send the result back to the server
		strcpy(result, listOfMessagesString);

		// Free the memory
		free(listOfMessagesString);
		//free(listOfMessages);

	// HANDLE GETTING MESSAGES FOR LISTEN
	} else if (!strcmp(command, "get_message")) {
		printf("get_message\n");

		// Check if the message actually exists
		if (!checkIfMessageExists(ops[1], MESSAGES_REPO)) {
			result = "MESSAGEERROR";
		} else {
			char* path = (char*)malloc(BUF_SIZE * sizeof(char));
			char* filename = (char*)malloc(BUF_SIZE * sizeof(char));
			strcpy(filename, ops[1]);
			sprintf(path, "%s/%s", TMP_DIR, filename);

			// Tell the server that I'm about to send the file
			char* buf = (char*)malloc(BUF_SIZE * sizeof(char));
			sprintf(buf, "get_message;%s;%d", ops[1], get_file_size(path));
			sendToSocket(socket, buf);

			sleep(1);

			// Send the file to the server
			puts("about to send file to server");
			sendFile(socket, path, get_file_size(path));

			// Update the repository, flagging the message as read
			t_message* m = (t_message*)malloc(sizeof(t_message));
			m = getFromRepository(MESSAGES_REPO, filename);
			flagMessageRead(m);
			saveInRepository(m, MESSAGES_REPO);

			// Update the sender's messages, flagging the message as read
			t_user* u = (t_user*)malloc(sizeof(t_user));
			u = searchUser(m->sender, USERS_REPOSITORY);
			flagMessageAsReadInList(u->messages, m);
			saveUser(u, USERS_REPOSITORY);

			free(path);
			free(filename);
			free(m);
			free(u);

			// Go to next main iteration
			return;
		}


	} else if (!strcmp(command, "forward")) {
		printf("forward\n");

		// To forward the message, just update the messages repo file
		// by copying over the original line, and changing the sender and receiver
		// one can always see that the message is forwarded by looking at the filename, that won't change.
		// Even if the message was already forwarded in the first place (and the sender and receiver are the same)

		// Get the message from the repository
		t_message* m = (t_message*)malloc(sizeof(t_message));
		m = getFromRepository(MESSAGES_REPO, ops[3]);

		if (m == NULL) {
			strcpy(result, "MESSAGEERROR");
		} else {
			// Update the sender and receiver, and set the message as unread
			strcpy(m->sender, ops[1]);
			strcpy(m->receiver, ops[2]);
			m->is_read = 0;

			// Testing if message updated correctly
			printf("%s\n", formatPrintMessage(m, ""));

			// Save the message in the repository
			saveInRepository(m, MESSAGES_REPO);

			// Update the sender with the information about the message
			t_user* u = (t_user*)malloc(sizeof(t_user));
			u = searchUser(ops[1], USERS_REPOSITORY);
			u = addMessageToUserNoRepo(u, m);
			saveUser(u, USERS_REPOSITORY);

			// Send the result back to the server, with the updated user
			sprintf(result, "%s", printUser(u, ""));
		}
		free(m);

	// HANDLE DELETE
	} else if (!strcmp(command, "delete")) {
		printf("delete\n");

		// Check if the message actually exists
		puts("first check");
		if (!checkIfMessageExists(ops[3], MESSAGES_REPO)) {
			result = "MESSAGEERROR";
		} else {

			// Get the message struct, will need it later when updating the user
			t_message* m = (t_message*)malloc(sizeof(t_message));
			m = getFromRepository(MESSAGES_REPO, ops[3]);
			printf("testing m: %s\n", formatPrintMessage(m, ""));

			puts("before removing from repo");
			// Delete the message from the repository
			removeDuplicateMessages(ops[1], ops[3], MESSAGES_REPO);

			// If there are no more messages in the repository with the same filename, I can safely delete the file
			if (!checkIfMessageExists(ops[3], MESSAGES_REPO)) {
				puts("deleting all the rest");
				char* path = (char*)malloc(BUF_SIZE * sizeof(char));
				char* filename = (char*)malloc(BUF_SIZE * sizeof(char));
				strcpy(filename, ops[3]);
				sprintf(path, "%s/%s", TMP_DIR, filename);

				// Delete the file
				remove(path);
			}
			puts("updating the user");
			
			puts("\t1");
			// Update the sender and receiver, to be sure that I'm deleting the right message
			strcpy(m->sender, ops[1]);
			strcpy(m->receiver, ops[2]);

			puts("\t2");
			// Update the sender
			t_user* u = (t_user*)malloc(sizeof(t_user));
			u = searchUser(ops[1], USERS_REPOSITORY);
			u = removeMessageFromUserNoRepo(u, m);
			saveUser(u, USERS_REPOSITORY);

			// Send the result back to the server, with the updated user
			sprintf(result, "%s", printUser(u, ""));

			free(u);
		}

	// HANDLE DEFAULT
	} else {
		printf("operation not supported");
		strcpy(result, "noop");
	}

	// Send the result back to the server
	sendToSocket(socket, result);

	free(ops);
	free(rcvString);
	free(command);
	free(result);

} 	