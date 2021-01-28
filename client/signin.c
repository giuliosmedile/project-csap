#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 64

char* signup() {
	char username[BUF_SIZE];
	char password[BUF_SIZE];
	char* result[3*BUF_SIZE];

	printf("[-] Signing up\n");
	printf("\033[1;31m");	//Red text
	printf("Username: ");
	if (fgets(username,sizeof(username),stdin) == NULL) {
        printf("\n");
        exit(0);
    }
    printf("\033[0m");		//Reset to old color

    printf("\033[1;31m");	//Red text
    printf("\nPassword for %s: ", username);
    if (fgets(password,sizeof(password),stdin) == NULL) {
        printf("\n");
        exit(0);
    }
	printf("\033[0m");		//Reset to old color

	sprintf(result, "login %s %s", username, password);
	return result;
}

char* login() {
	char username[BUF_SIZE];
	char password[BUF_SIZE];
	char* result[3*BUF_SIZE];

	printf("[-] Logging in\n");
	printf("\033[1;31m");	//Red text
	printf("Username: ");
	if (fgets(username,sizeof(username),stdin) == NULL) {
        printf("\n");
        exit(0);
    }
    printf("\033[0m");		//Reset to old color

    printf("\033[1;31m");	//Red text
    printf("\nPassword for %s: ", username);
    if (fgets(password,sizeof(password),stdin) == NULL) {
        printf("\n");
        exit(0);
    }
	printf("\033[0m");		//Reset to old color

	sprintf(result, "login %s %s", username, password);
	return result;
}

char* help() {
	return "";
}


