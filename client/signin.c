#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

/** Signup routine.
 *  @param result the line that the client typed in stdin
 *  @param u_p address of a user struct. Needed to check if already logged in
 *  @returns the string to send to the server
*/
char* signup(char* result, t_user** u_p) {
	char* username = (char*)malloc(BUF_SIZE * sizeof(char));
	char* password = (char*)malloc(BUF_SIZE * sizeof(char));

	printf("[-] Signing up\n");
	printf("%s", COLOR);	//Red text

    if (*u_p != NULL) {
        printf("[!] It seems you are already logged in as a registered user.\n[-] Log out before trying to sign up.\n");
        sprintf(result, "null");
        return "null";
    }

	printf("Username: ");
	if (fgets(username,BUF_SIZE,stdin) == NULL) {
        //printf("\n");
        exit(0);
    }
    username[strlen(username) - 1] = '\0';
    printf(STD_COL);		//Reset to old color

    printf("%s", COLOR);	//Red text
    printf("Password for %s: ", username);
    password = getpass("");
    // if (fgets(password,sizeof(password),stdin) == NULL) {
    //     printf("\n");
    //     exit(0);
    // }
	printf(STD_COL);		//Reset to old color

	sprintf(result, "signup %s %s", username, password);

    free(username);
    free(password);
	return result;
}

/** Login routine.
 *  @result the line that the client typed in stdin
 *  @u_p address of a user struct. Needed to check if already logged in
 *  @returns the string to send to the server
*/
char* login(char* result, t_user** u_p) {
	char* username = (char*)malloc(BUF_SIZE * sizeof(char));
	char* password = (char*)malloc(BUF_SIZE * sizeof(char));

	printf("[-] Logging in\n");

    if (*u_p != NULL) {
        printf("[!] It seems you are already logged in as a registered user.\n[-] Log out before trying to log in again.\n");
        sprintf(result, "null");
        return "null";
    }

	printf("%s", COLOR);	//Red text
	printf("Username: ");
	if (fgets(username,BUF_SIZE,stdin) == NULL) {
        printf("\n");
        exit(0);
    }
    username[strlen(username) - 1] = '\0';
    printf(STD_COL);		//Reset to old color

    printf("%s", COLOR);	//Red text
    printf("Password for %s: ", username);
    if (fgets(password,BUF_SIZE,stdin) == NULL) {
        printf("\n");
        exit(0);
    }
	printf(STD_COL);		//Reset to old color

	sprintf(result, "login %s %s", username, password);
    result[strlen(result)-1] = '\0';    //Avoid \n
    free(username);
    free(password);
	return result;
}

/** Logout routine.
 *  @output the line that the client typed in stdin
 *  @u_p address of a user struct. Needed to check if already logged in
 *  @returns the string to send to the server
*/
char* logout(char* output, t_user** u_p) {
    if (*u_p == NULL) {
        printf("[!] You are not logged in.\n");
    } else {
        free(*u_p);
        *u_p = NULL;
        printf("[-] Succesfully logged out. \n");
    }
    sprintf(output, "null");
    return "null";
}

