// FUNCTIONS USED TO HANDLE HOW THE SOCKET RESPONDS TO THE CLIENT
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void handleLogin(char* response)  {
	t_user* u = readUser(response);
	if (u == NULL) {
		printf("User not valid.\n");
	}
	char* str = printUser(u, str);
	printf("\033[1;31m");
	printf("USER: ");
	printf("%s", str);
	printf("\033[0m");

}

void handleSignup(char* response) {
	response[strlen(response)-1] = '\0';
	if (!strcmp(response, "1")) {
		printf("[-] You successfully signed up. Now you can use your credentials to log in and start using the service.\n");
	} else {
		printf("[-] The username you tried to sign up with has already been taken. Please try again with another username.\n");
	}

	return;
}

void handleServerReplies(char* command, char* response) {

	printf("PREHANDLING: \"%s\", \"%s\"\n", command, response);
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

