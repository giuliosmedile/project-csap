// FUNCTIONS TO HANDLE MESSAGES AND ADDRESSBOOK

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>

char* add(char* result, t_user** u_p) {
    // the new user i am trying to add
    char* other = (char*)malloc(BUF_SIZE * sizeof(char));

    printf("[-] Adding user to addressbook");
    printf("%s", COLOR);

    // Check if i am logged in
    if (*u_p == NULL) {
        printf("[!] Before adding a user to your addressbook, perhaps you should try logging in first.\n");
        return "null";
    }

    // Get user input
    printf("Username of the user you want to add: ");
	if (fgets(other,sizeof(other),stdin) == NULL) {
        //printf("\n");
        exit(0);
    }
    other[strlen(other)-1] = '\0';
    sprintf(result, "add %s %s", (*u_p)->username, other);
    return result;

}