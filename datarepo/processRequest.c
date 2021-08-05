// FUNCTIONS THAT THE METADATA SERVER USES TO PROCESS A SERVER REQUEST

#define MESSAGES_LOCATION "messages"

char* processRequest(int socket, char* request) {
	char** args = (char**)malloc(2*sizeof(char*));
	// Each process is structured as "command;data", so i tokenize to get the actual request
	tokenize(request, &args);

	// TODO PER BENE
	if (!strcmp(args[0], "send")) {
		// The rest of the request is structured as "username;messageno", so I just look for that
		char* filename = searchMessage(args[1], atoi(args[2]));

		// Once I find the message, I just send it
		sendFile(socket, filename, get_file_size(filename));

	} else if (!strcmp(args[0], "receive")) {

		// The server sent the data of the recording as "username;messageno", 
		// so I store these
		char filename[BUF_SIZE];
		sprintf(filename, "%s/%s;%s", MESSAGES_LOCATION, args[1], args[2]);

		// After I received this, the server will send the actual message data, so
		// I listen on the socket for it
		receiveFile(socket, filename);

	} else if (!strcmp(args[0], "delete")) {

		// The rest of the request is structured as "username;messageno", so I just look for that name combination
		// and delete the correct file
		deleteMessage(args[1], atoi(args[2]));
	} else {
		return "null";
	}
}