// FUNCTIONS USED TO HANDLE HOW THE SOCKET RESPONDS TO THE CLIENT
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

/**
 * Decodes a user struct from the response from the MDS.
 * @response:	The response from the MDS.
*/
t_user* decodeUser(char* response, t_user* u) {
	DEBUGPRINT(("[~] decodeuser\n"));
	// Free the user before doing any operation to it
	if (u != NULL) {
		puts("user not null");
		free(u);
		u = (t_user*)malloc(sizeof(t_user));
	}

	u = readUser(response);
	if (u == NULL) {
		printf("User not valid.\n");
		return NULL;
	}
	DEBUGPRINT(("[~] end readuser in decodeuser\n"));
	printf("Information for user %s: \n", u->username);
	//char* str = formatPrintUser(u, str);
	printf("\033[1;31m");
	printf("%s", formatPrintUser(u, ""));
	printf("\033[0m\n");

	return u;
}

t_user* handleLogin(char* response, t_user* user)  {

	if (!strcmp(response, "NOLOGIN")) {
		printf("[-] Could not authenticate you. Did you check your password before logging in?\n");
		return user;
	}

	if (!strcmp(response, "null") || !strcmp(response, "NULL") || !strcmp(response, "ERROR") || strstr(response, ";") == NULL) {
		printf("[!] There has been a connection error. You are not the cause of such error. Please try again.\n");
		return user;
	}

	user = decodeUser(response, user);
	return user;

}

void handleSignup(char* response) {
	response[strlen(response)-1] = '\0';
	if (strcmp(response, "NOSIGNUP")) {
		printf("[-] You successfully signed up. Now you can use your credentials to log in and start using the service.\n");
	} else {
		printf("[-] The username you tried to sign up with has already been taken. Please try again with another username.\n");
	}

	return;
}

t_user* handleAdd(char* response, t_user* user)  {

	if (!strcmp(response, "NOADD")) {
		printf("[!] Could not find the other user you tried adding. Does this user exist?\n");
		return user;
	}

	user = decodeUser(response, user);
	return user;

}

t_user* handleRecord(char* response, t_user* user)  {

	// Check for various errors
	if (!strcmp(response, "NORECORD")) {
		printf("[!] There has been an error sending the message to the server. Please send the message again.\n");
		return user;
	} else if (!strcmp(response, "FILESIZEERROR")) {
		printf("[!] There has been an error saving the file on the data repo. Please check your internet connection and try again.\n");
		return user;
	}

	// If there are no errors, decode the response
	user = decodeUser(response, user);
	return user;

}

t_user* handleForward(char* response, t_user* user)  {

	// Check for various errors
	if (!strcmp(response, "MESSAGEERROR")) {
		printf("[!] There has been an error while processing the forward operation. Please perform the operation again.\n");
		return user;
	}

	// If there are no errors, decode the response
	user = decodeUser(response, user);
	return user;
}

t_user* handleDelete(char* response, t_user* user)  {
	// Check for various errors
	if (!strcmp(response, "DELETEERROR") || !strcmp(response, "null")) {
		printf("[!] There has been an error while processing the deletion operation. Please perform the operation again.\n");
		return user;
	}

	// If there are no errors, decode the response
	user = decodeUser(response, user);
	return user;
}

t_user* handleRemove(char* response, t_user* user)  {
	// Check for various errors
	if (!strcmp(response, "REMOVEERROR") || !strcmp(response, "null")) {
		printf("[!] There has been an error while processing the removal operation. Please perform the operation again.\n");
		return user;
	}

	// If there are no errors, decode the response
	user = decodeUser(response, user);
	return user;
}

t_user* handleServerReplies(char* command, char* response, t_user* user) {
	DEBUGPRINT(("[~] PREHANDLING: cmd: \"%s\", rsp: \"%s\"\n", command, response));

	if (response == NULL || !strcmp(response, "null")) {
		printf("[-] There has been an error with the server. Please try again.\n");
		return user;
	}

	if (!strcmp(command, "login")) {
		user = handleLogin(response, user);
	} else if (!strcmp(command, "signup")) {
		handleSignup(response);
	} else if (!strcmp(command, "add")) {
		user = handleAdd(response, user);
	} else if (!strcmp(command, "record")) {
		user = handleRecord(response, user);
	} else if (!strcmp(command, "forward")) {
		user = handleForward(response, user);
	} else if (!strcmp(command, "delete")) {
		user = handleDelete(response, user);
	} else if (!strcmp(command, "remove")) {
		user = handleRemove(response, user);
	} else if (!strcmp(command, "")){
		printf("There must have been an error in the response from the server.\n");
	} else {
		// Probably received junk data, best to abort
		printf("There has been an error while handling your request.\nPlease try restarting the program and trying again.\n");
		exit(1);
	}

	return user;
}

