// FUNCTIONS USED TO HANDLE HOW THE SOCKET RESPONDS TO THE CLIENT
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

/**
 * Decodes a user struct from the response from the MDS.
 * @response:	The response from the MDS.
*/

void decodeUser(char* response) {

	t_user* u = readUser(response);
	if (u == NULL) {
		printf("User not valid.\n");
		return;
	}
	printf("Information for user %s: \n", u->username);
	char* str = formatPrintUser(u, str);
	printf("\033[1;31m");
	printf("%s", str);
	printf("\033[0m\n");

	return;

}

void handleLogin(char* response)  {

	if (!strcmp(response, "-1")) {
		printf("[-] Could not authenticate you. Did you check your password before logging in?\n");
		return;
	}

	decodeUser(response);
	return;

}

void handleSignup(char* response) {
	response[strlen(response)-1] = '\0';
	if (strcmp(response, "")) {
		printf("[-] You successfully signed up. Now you can use your credentials to log in and start using the service.\n");
	} else {
		printf("[-] The username you tried to sign up with has already been taken. Please try again with another username.\n");
	}

	return;
}

void handleServerReplies(char* command, char* response) {
#ifdef TEST
	printf("[-] PREHANDLING: cmd: \"%s\", rsp: \"%s\"\n", command, response);
#endif
	if (!strcmp(command, "login")) {
		handleLogin(response);
	} else if (!strcmp(command, "signup")) {
		handleSignup(response);
	} else {
		printf("How did you even get here?\n");
		return;
	}
	return;
}

