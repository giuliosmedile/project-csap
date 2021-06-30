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

	u = readUser(response);
	if (u == NULL) {
		printf("User not valid.\n");
		return NULL;
	}
	printf("Information for user %s: \n", u->username);
	char* str = formatPrintUser(u, str);
	printf("\033[1;31m");
	printf("%s", str);
	printf("\033[0m\n");

	return u;
}

void handleLogin(char* response, t_user* user)  {

	if (!strcmp(response, "NOLOGIN")) {
		printf("[-] Could not authenticate you. Did you check your password before logging in?\n");
		return;
	}

	user = decodeUser(response, user);
	return;

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

void handleServerReplies(char* command, char* response, t_user* user) {
#ifdef TEST
	printf("[-] PREHANDLING: cmd: \"%s\", rsp: \"%s\"\n", command, response);
#endif
	if (!strcmp(command, "login")) {
		handleLogin(response, user);
	} else if (!strcmp(command, "signup")) {
		handleSignup(response);
	} else {
		printf("How did you even get here?\n");
		return;
	}
	return;
}

