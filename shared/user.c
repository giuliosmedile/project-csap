#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#define BUF_SIZE 256
#define MAX_ADDRESSBOOK_SIZE 50

struct s_user {
	char* username;			// the user's username
	int messagesno;			// the number of messages the user has sent
	int addressbook_size;	// how many users this user has in their addressbook
	char* addressbook[];	// the user's addressbook
};
typedef struct s_user t_user;

t_user createUser(char* username) {
	// Allocate the space for that user
	t_user* u = malloc(sizeof(t_user)+MAX_ADDRESSBOOK_SIZE*sizeof(char));

	// Fill the struct
	u->username = (char*)malloc(BUF_SIZE * sizeof(char));
	strcpy(u->username, username);
	u->messagesno = 0;
	return u;
}

void printUser(t_user* u, char* filename) {
	FILE* fp;

	// Open the file to append this user
	if ((fp = fopen(filename, "a")) == NULL) return;

	char* buf = (char*)malloc(BUF_SIZE * sizeof(char));

	// Write the username
	strcat(buf, u->username);
	strcat(buf, ";");

	// Write the number of messages sent
	strcat(buf, u->messagesno);
	strcat(buf, ";");

	// Write the addressbook size
	strcat(buf, u->addressbook_size);
	strcat(buf, ";");

	// Write the addressbook
	for (int i = 0; i<u->addressbook_size; i++) {
		strcat(buf, u->addressbook[i]);
		strcat(buf, ";");
	}

	// Finish by newline
	strcat(buf, "\n");

	// Print this on the file
	fprintf(fp, buf);

}

// Function that returns a user struct after reading a line
// The line _MUST_ be formatted as shown in the function above this
t_user readUser(char* line) {

	// Tokenize the line
	char** args = (char**)malloc((MAX_ADDRESSBOOK_SIZE+3) * sizeof(char*));
	tokenize(line, &args);
	// Allocate the space for the new user
	// I can use args[2] because I know that is the number of people in the addressbook
	t_user* u = malloc(sizeof(t_user)+atoi(args[2])*sizeof(char));

	// Now I can start filling the struct
	u->username = args[0];
	u->messagesno = args[1];
	u->addressbook_size = atoi(args[2]);
	for (int i = 0; i < u->addressbook_size; i++) {
		// I first allocate the memory for the string, then copy it over
		u->addressbook[i] = malloc(BUF_SIZE * sizeof(char));
		strcpy(u->addressbook[i], args[3+i]);
	}


}