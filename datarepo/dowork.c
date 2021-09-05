// Boolean to tell if I'm a leader or a slave
int amLeader = 1;	// TEMPORARILY SET AT 1

// Set of booleans to tell if a file was modified
// Will use those to decide if we need to tell the slaves to update the file
int wasMessagesModified, wasUsersModified, wasMessageAdded = 0;

/**
 * Function that gets modified files to send them back to the server
 * @return the modified files
**/
char* getModifiedFiles() {
	char* result = malloc(sizeof(char) * BUF_SIZE);
	sprintf(result, "%d*", wasMessagesModified+wasUsersModified+wasMessageAdded);

	if (!wasMessagesModified && !wasUsersModified && !wasMessageAdded) {
		sprintf(result + strlen(result), "noModifiedFiles");
	}

	if (wasMessagesModified) {
		// Set the flag back to zero
		wasMessagesModified = 0;
		char* temp = malloc(sizeof(char) * BUF_SIZE);
		temp = printFileToString(MESSAGES_REPO, result, BUF_SIZE);
		sprintf(result + strlen(result), "messages*%s", temp);
	} 
	if (wasUsersModified) {
		// Set the flag back to zero
		wasUsersModified = 0;
		char* temp = malloc(sizeof(char) * BUF_SIZE);
		temp = printFileToString(USERS_REPO, result, BUF_SIZE);
		sprintf(result + strlen(result), "users\\%s", printFileToString(USERS_REPO, result, BUF_SIZE));
	} 
	if (wasMessageAdded) {
		// Set the flag back to zero
		// In this case, it is useless to jump the message back and forth between datarepos,
		// I just make it so the server sends it back "automatically"
		wasMessageAdded = 0;
		sprintf(result + strlen(result), "messageAdded");
	}
	return result;

}

/**
 * Function that does the main work of the program. It is called by dowork only if server signals that this is a Slave datarepo
 * TODO
**/
char* doworkAsSlave(char* rcvString, int socket) {

	char** ops = (char**)malloc(10*BUF_SIZE);
	char* command = (char*)malloc(BUF_SIZE * sizeof(char));
 	char* result = (char*)malloc(BUF_SIZE * sizeof(char));

	// Read request from server
	DEBUGPRINT(("before tokenizing\n"));
	printf("[-] Slave Received: \"%s\"\n", rcvString);

	if (strcmp(rcvString, "noModifiedFiles") == 0) {
		// No files were modified, just return
		return rcvString;
	}

	// Insert the string received from the socket into the ops array
	tokenizeWithSeparators(rcvString, &ops, "*\\");

	// Process request by tokenizing it
	printf("Inside dowork %s\n", ops[0]);
	int count = atoi(ops[0]);
	DEBUGPRINT(("count: %s\n", ops[0]));

	// For each modified file
	for (int i = 0; i < count; i++) {
		// Understand which file was modified and update it correctly
		strcpy(command, ops[i+1]);
		printf("%d\t%s\n", i, command);
		if (strcmp(command, "messages") == 0) {
			puts("updating messages repository");

			// Update the messages repository
			if (printStringToFile(MESSAGES_REPO, ops[i+2])) {
				strcpy(result, "success");
			} else {
				strcpy(result, "failure");
			}
		} else if (strcmp(command, "users") == 0) {
			puts("updating users repository");

			// Update the users repository
			if (printStringToFile(USERS_REPO, ops[i+2])) {
				strcpy(result, "success");
			} else {
				strcpy(result, "failure");
			}

		} else if (strcmp(command, "messageAdded") == 0) {

			printf("messageAdded\n");

			// Wait for the server to send back metadata about the file
			// which will be structured as "message;[filename];[size]"
			bzero(ops, 10*BUF_SIZE);
			bzero(rcvString, BUF_SIZE);
			rcvString = readFromSocket(socket, rcvString);
			tokenizeWithSeparators(rcvString, &ops, ";");
			
			// Create the temp directory, it if is not there
			struct stat st = {0};
			if (stat(TMP_DIR, &st) == -1) {
				mkdir(TMP_DIR, 0755);
			}

			// Define the path I'll save into
			char* path = (char*)malloc(BUF_SIZE * sizeof(char));
			sprintf(path, "%s/%s", TMP_DIR, ops[i+1]);

			// Wait for the file from the socket
			receiveFile(socket, path);

			// If the file size is not the same, it means the file was not sent correctly
			if (get_file_size(path) != atoi(ops[2])) {
				strcpy(result, "failure");
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
				u = searchUser(user, USERS_REPO);
				u = addMessageToUser(u, ops[1]);

				// Update the user on the repo
				saveUser(u, USERS_REPO);
				
				free(tmp);
				free(tmp_ops);
				free(user);

				strcpy(result, "success");
			}
			free(path);

		}

	}
	return result;

}

/** 
 * Function that does the main work of the program. It is called by dowork only if server signals that this is a Leader datarepo
 * TODO
**/
char* doworkForClient(char* rcvString, int socket) {
	char** ops = (char**)malloc(10*BUF_SIZE);
	char* command = (char*)malloc(BUF_SIZE * sizeof(char));
 	char* result = (char*)malloc(BUF_SIZE * sizeof(char));
	t_user* user;

	DEBUGPRINT(("before tokenizing"));
	printf("[-] Leader Received: \"%s\"\n", rcvString);
	// Insert the string received from the socket into the ops array
	tokenize(rcvString, &ops);

	printf("Inside dowork\n");
	DEBUGPRINT(("ops: \"%s\"\n", ops[0]));
	sprintf(command, "%s", ops[0]);

	DEBUGPRINT(("handling operations\n"));
	// HANDLE LOGIN
	if (!strcmp(command, "login")) {
		printf("login\n");
		result = getUser(ops[1], USERS_REPO);
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
		saveUser(user, USERS_REPO);
		result = getUser(user->username, USERS_REPO);

		printf("result: \"%s\"\n", result);
		printf("end signups\n");

		wasUsersModified = 1;
		free(user);
	
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

		wasUsersModified = 1;
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
			u = searchUser(user, USERS_REPO);
			u = addMessageToUser(u, ops[1]);

			// Update the user on the repo
			saveUser(u, USERS_REPO);

			// Move the file on the final directory
			// TODO? Nah.
			
			// Send the result back to server
			result = getUser(user, USERS_REPO);
			result[strlen(result)-1] = '\0'; 			// Make sure the string is null terminated, not \n terminated
			printf("result: \"%s\"\n", result);
			printf("end record\n");

			wasUsersModified = 1;
			wasMessagesModified = 1;
			wasMessageAdded = 1;

			free(tmp);
			free(tmp_ops);
			free(user);
		}
		free(path);

	// HANDLE FIRST PART OF LISTEN AND FORWARD
	} else if (!strcmp(command, "get_messages_for")) {
		printf("get_messages_for\n");

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
			t_message* m;			
			m = getFromRepository(MESSAGES_REPO, filename);
			flagMessageRead(m);
			saveInRepository(m, MESSAGES_REPO);

			// Update the sender's messages, flagging the message as read
			t_user* u = (t_user*)malloc(sizeof(t_user));
			u = searchUser(m->sender, USERS_REPO);
			flagMessageAsReadInList(u->messages, m);
			saveUser(u, USERS_REPO);

			wasMessagesModified = 1;
			wasUsersModified = 1;

			free(path);
			free(filename);
			free(m);
			free(u);

			// Go to next main iteration and tell dowork not to send the result
			strcpy(result, "null");
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
			u = searchUser(ops[1], USERS_REPO);
			u = addMessageToUserNoRepo(u, m);
			saveUser(u, USERS_REPO);

			// Send the result back to the server, with the updated user
			sprintf(result, "%s", printUser(u, ""));

			wasUsersModified = 1;
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
			u = searchUser(ops[1], USERS_REPO);
			u = removeMessageFromUserNoRepo(u, m);
			saveUser(u, USERS_REPO);

			wasMessagesModified = 1;
			wasUsersModified = 1;

			// Send the result back to the server, with the updated user
			strcpy(result, printUser(u, ""));
		}

	// HANDLE REMOVE
	} else if (!strcmp(command, "remove")) {
		printf("remove\n");

		// Get the client's user from the repository
		DEBUGPRINT(("before search\n"));
		t_user* u = (t_user*)malloc(sizeof(t_user));
		u = searchUser(ops[1], USERS_REPO);

		DEBUGPRINT(("test user: %s\n", formatPrintUser(u, "")));

		// Remove the user from the addressbook
		u = removeUserFromAddressBook(u, ops[2]);

		// Save the user in the repository
		puts("save user");
		saveUser(u, USERS_REPO);

		// Send the result back to the server, with the updated user
		char* temp = malloc(BUF_SIZE * sizeof(char));
		temp = printUser(u, temp);
		sprintf(result, "%s", temp);
		DEBUGPRINT(("result: %s\n", result));
		free(temp);

		wasUsersModified = 1;
		free(u);

	// HANDLE SEARCH
	} else if (!strcmp(command, "search")) {
		printf("search\n");

		// Search all the messages in the time range
		char* listOfMessagesString = getByReceiverFromFileInRange(MESSAGES_REPO, ops[1], atoi(ops[2]), atoi(ops[3]));

		// Send the result back to the server
		strcpy(result, listOfMessagesString);

		// Free the memory
		free(listOfMessagesString);

	// HANDLE DEFAULT
	} else if (!strcmp(command, "ping")) {
		printf("ping\n");
		strcpy(result, "null");
		return result;
	} else {
		puts("operation not supported");
		strcpy(result, "noop");
		return result;
	}

	free(ops);
	free(command);

	return result;
}


void dowork(int socket) {
	puts("\n----------------------- Inside dowork ----------------------------\n");
	char* result = (char*)malloc(BUF_SIZE * sizeof(char));
	char* rcvString = (char*)malloc(BUF_SIZE * sizeof(char));

	DEBUGPRINT(("waiting for replies from %d\n", socket));
    // Wait for requests
 	rcvString = readFromSocket(socket, rcvString);

	if (!strcmp(rcvString, "ping")) {
		printf("[!] Pinged from %d\n", socket);
		free(rcvString);
		free(result);
		return;
	}

	if (!strcmp(rcvString, "LEADER")) {
		printf("[!] I'm a leader\n");
		amLeader = 1;
		free(rcvString);
		free(result);
		return;
	}

	if (!strcmp(rcvString, "SLAVE")) {
		printf("[!] I'm a slave\n");
		amLeader = 0;
		free(rcvString);
		free(result);
		return;
	}

	// TODO switch if i'm a slave or a leader
	if (amLeader) {
		result = doworkForClient(rcvString, socket);

		// Send to server only if result is NOT "null"
		if (strcmp(result, "null")) {
			DEBUGPRINT(("before send\n"));
			// Send the result back to the server
			sendToSocket(socket, result);
		}

		// Send modified files back to the server
		result = getModifiedFiles(socket);
		DEBUGPRINT(("Before sending to slaves: %s\n", result));
		sendToSocket(socket, result);
		DEBUGPRINT(("After sending to slaves\n"));


	} else {
		doworkAsSlave(rcvString, socket);
	}

	DEBUGPRINT(("before next it\n"));
	free(rcvString);
	free(result);
	return;

} 	