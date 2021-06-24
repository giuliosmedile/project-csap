#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

// The next two are for handling the color output in the console
#define COLOR "\033[0;33m"
#define STD_COL "\033[0m"

#define BUF_SIZE 256

char* signup(char* result) {
	char username[BUF_SIZE];
	char password[BUF_SIZE];

	printf("[-] Signing up\n");
	printf("%s", COLOR);	//Red text
	printf("Username: ");
	if (fgets(username,sizeof(username),stdin) == NULL) {
        //printf("\n");
        exit(0);
    }
    username[strlen(username) - 1] = '\0';
    printf("\033[0m");		//Reset to old color

    printf("%s", COLOR);	//Red text
    printf("Password for %s: ", username);
    if (fgets(password,sizeof(password),stdin) == NULL) {
        printf("\n");
        exit(0);
    }
	printf("\033[0m");		//Reset to old color

	sprintf(result, "signup %s %s", username, password);
	return result;
}

char* login(char* result) {
	char username[BUF_SIZE];
	char password[BUF_SIZE];

	printf("[-] Logging in\n");
	printf("%s", COLOR);	//Red text
	printf("Username: ");
	if (fgets(username,sizeof(username),stdin) == NULL) {
        printf("\n");
        exit(0);
    }
    username[strlen(username) - 1] = '\0';
    printf("\033[0m");		//Reset to old color

    printf("%s", COLOR);	//Red text
    printf("Password for %s: ", username);
    if (fgets(password,sizeof(password),stdin) == NULL) {
        printf("\n");
        exit(0);
    }
	printf("\033[0m");		//Reset to old color

	sprintf(result, "login %s %s", username, password);
    result[strlen(result)-1] = '\0';    //Avoid \n
	return result;
}

