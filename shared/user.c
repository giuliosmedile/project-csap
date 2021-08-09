#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
//#include "list.c"

// #define BUF_SIZE 4096
#define MAX_ADDRESSBOOK_SIZE 50
#ifdef TEST
	char* repo = "test.txt";
#else
	char* repo = "../datarepo/data/users.txt";
#endif
// struct s_user {
// 	char* username;			// the user's username
// 	int addressbook_size;	// how many users this user has in their addressbook
// 	int messagesno;			// the number of messages the user has sent
// 	NODE* messages;			// the user's messages
// 	char* addressbook[];	// the user's addressbook
// };
// typedef struct s_user t_user;

void removeDuplicates(char* username, char* filename);
t_user* searchUser(char* username, char* filename);

/**
 * Initializes a user. 
 * All numeric parameters are set to zero.
 * 
 * @username: the username of the new user
 * @return: the user struct of the new user
*/
t_user* createUser(char* username) {
	// Allocate the space for that user
	t_user* u = malloc(sizeof(t_user*)+MAX_ADDRESSBOOK_SIZE*sizeof(char));

	// Fill the struct
	u->username = (char*)malloc(BUF_SIZE * sizeof(char));
	strcpy(u->username, username);
	u->addressbook_size = 0;
	*(u->addressbook) = malloc(MAX_ADDRESSBOOK_SIZE * sizeof(char*));
	u->messagesno = 0;
	u->messages = (NODE*)malloc(sizeof(NODE));
	init_list(&(u->messages));
	return u;
}

// Similar to saveUser, just saves the result to a string
char* printUser(t_user* u, char* string) {

	char* buf = (char*)malloc(10000 * sizeof(char));

	// Write the username
	strcat(buf, u->username);
	strcat(buf, ";");

	// Write the number of messages sent
	char tmp[10];
	sprintf(tmp, "%d", u->messagesno);
	strcat(buf, tmp);
	strcat(buf, ";");

	// Write the addressbook size
	sprintf(tmp, "%d", u->addressbook_size);
	strcat(buf, tmp);
	strcat(buf, ";");

	// Write the addressbook
	for (int i = 1; i<u->addressbook_size+1; i++) {
		strcat(buf, u->addressbook[i]);
		strcat(buf, ";");
	}

	// Write the messages
	NODE* temp = u->messages;
	while (temp != NULL) {
		sprintf(tmp, "%s;", temp->message->filename);
		strcat(buf, tmp);
	}

	printf("- End print: buf: \"%s\"\n", buf);

	return buf;

}

/**
 * Fancily prints an input user
 * @u:		the user taken in input
 * @string:	the string i want to save the output in
 * @returns the formatted string i want to save the output in
*/
char* formatPrintUser(t_user* u, char* string) {

	char* tmp = malloc(BUF_SIZE * sizeof(char));
	string = malloc(BUF_SIZE * sizeof(char));

	sprintf(tmp, "username: %s\n", u->username);
	strcat(string, tmp);

	sprintf(tmp, "number of messages sent: %d\n", u->messagesno);
	strcat(string, tmp);

	if (u->addressbook_size == 0) {
		sprintf(tmp, "There is nobody saved in your addressbook.\n");
		strcat(string, tmp);
	} else {
		sprintf(tmp, "-- Addressbook --\n");
		strcat(string, tmp);

		for (int i = 1; i<=u->addressbook_size; i++) {
			sprintf(tmp, "%d\t%s\n", i, u->addressbook[i]);
			strcat(string, tmp);
		}
	}

	if (u->messagesno == 0) {
		sprintf(tmp, "You have not sent any messages.\n");
	} else {
		sprintf(tmp, "-- Messages --\n");
		strcat(string, tmp);

		NODE* temp = u->messages;
		int i = 1;
		while (temp != NULL) {
			sprintf(tmp, "\tMessage number %d\n", i);
			strcat(string, tmp);

			sprintf(tmp, "\t\tFrom: %s\n", temp->message->sender->username);
			strcat(string, tmp);

			sprintf(tmp, "\t\tTo: %s\n", temp->message->receiver->username);
			strcat(string, tmp);

			sprintf(tmp, "\t\tDate: %s\n", ctime(&(temp->message->timestamp)));
			strcat(string, tmp);

			i++;
		}
	}

	return string;

}


/**
 * Saves the user "u" in file "filename", formatting it in this way:
 *		username;messagesno;addressbook_size;[addressbook]\n
*/

void saveUser(t_user* u, char* filename) {
	// First of all, remove duplicates, if exist
	removeDuplicates(u->username, filename);

	FILE* fp;

	// Open the file to append this user
	if ((fp = fopen(filename, "a")) == NULL) {
		printf("err: could not open file %s", filename);
		return;
	}
	char* buf = (char*)malloc(10000 * sizeof(char));

	// Write the username
	strcat(buf, u->username);
	strcat(buf, ";");

	// Write the number of messages sent
	char tmp[10];
	sprintf(tmp, "%d", u->messagesno);
	strcat(buf, tmp);
	strcat(buf, ";");

	// Write the addressbook size
	sprintf(tmp, "%d", u->addressbook_size);
	strcat(buf, tmp);
	strcat(buf, ";");

	// Write the addressbook
	for (int i = 1; i<u->addressbook_size+1; i++) {
		strcat(buf, u->addressbook[i]);
		strcat(buf, ";");
	}

	// Finish by newline
	strcat(buf, "\n");

	// Print this on the file
	fprintf(fp, "%s", buf);
	fclose(fp);
	free(buf);
	return;
}

// Function that returns a user struct after reading a line
// The line _MUST_ be formatted as shown in the function above this
t_user* readUser(char* line) {
	// Tokenize the line
	char** args = malloc((MAX_ADDRESSBOOK_SIZE+3) * sizeof(char*));
	tokenize(line, &args);

	// Allocate the space for the new user
	// I can use args[2] because I know that is the number of people in the addressbook
	t_user* u = malloc(sizeof(t_user*)+atoi(args[2])*sizeof(char));

	// Now I can start filling the struct
	u->username = args[0];
	u->messagesno = atoi(args[1]);
	u->addressbook_size = atoi(args[2]);
	*(u->addressbook) = malloc(MAX_ADDRESSBOOK_SIZE * sizeof(char*));
	for (int i = 1; i <= u->addressbook_size; i++) {
		// I first allocate the memory for the string, then copy it over
		u->addressbook[i] = (char*)malloc(BUF_SIZE * sizeof(char));
		strcpy(u->addressbook[i], args[2+i]);
	}
	return u;
}

t_user* addUserToAddressBook(t_user* u, char* username) {
	// First check if the user I'm adding exists
	printf("\ta\n");
	t_user* tmp = searchUser(username, repo);
	printf("\t%s\n", tmp->username);
	if(tmp==NULL) return NULL;

	// After I checked, I can add the user
	printf("\ta\n");
	int i = ++(u->addressbook_size);
	printf("\tb\n");
	u->addressbook[i] = malloc(BUF_SIZE * sizeof(char));
	printf("\tc\n");
	strcpy(u->addressbook[i], username);
	printf("\td\n");
	return u;
}


/** 
 * Function that search a user in the repository
 * @param username the username of the user to search
 * @param repo the repository to search in
 * @returns the user struct if it exists, NULL otherwise
*/
t_user* searchUser(char* username, char* filename) {
	FILE* fp;
	char* buf = malloc(BUF_SIZE * sizeof(char));
	size_t len;
	if ((fp = fopen(filename, "r")) == NULL) return NULL;

	while (getline(&buf, &len, fp) != -1) {
		t_user* u = readUser(buf);
		if (!strcmp(u->username, username))	return u;
	}
	fclose(fp);
}

/**
 * Function that tells if a user exists in another's addressbook
 * @param username the username of the user to search
 * @param u the user struct to search in
 * @returns 1 if the user exists, 0 otherwise
*/
int isInAddressBook(char* username, t_user* u) {
	for (int i = 1; i<=u->addressbook_size; i++) {
		if (!strcmp(u->addressbook[i], username)) return 1;
	}
	return 0;
}



/**
 * removes duplicates of username from repo file
*/
void removeDuplicates(char* username, char* filename) {
	size_t len;
	FILE *fp, *tmp_fp;
	char* buf = malloc(BUF_SIZE * sizeof(char));
    char** args = (char**)malloc(MAX_ADDRESSBOOK_SIZE * sizeof(char*));
    char temp[] = "temp.txt";

    if ((fp = fopen(filename, "r")) == NULL) return;
	if ((tmp_fp = fopen(temp, "w")) == NULL) return;

    // copy all contents to the temporary file except the specific line
    while (getline(&buf, &len, fp) != -1) {
    	char* t_str = malloc(BUF_SIZE * sizeof(char));
    	strcpy(t_str, buf);
        tokenize(t_str, &args);
        // If the username of the line I'm looking at is not the one passed in the function
        // I can copy it over. If it is, I skip it
    	if (strcmp(args[0], username) != 0) {
    		fprintf(tmp_fp, "%s", buf);
    	}
    }
    fclose(fp);
    fclose(tmp_fp);
    remove(filename);  		    // remove the original file 
    rename(temp, filename); 	// rename the temporary file to original name
}

/**
 * Function to retrieve a user from its username as a structured line from passed file
 * @username: 	the username i want to get
 * @filename:	the file in which the user is stored
 * @returns: 	the formatted line of the user in the repo file, null if not present
*/
char* getUser(char* username, char* filename) {
	// Search for the user in the filename file, and return it as it is written
	FILE* fp;
	size_t len;
	char* buf = malloc(BUF_SIZE * sizeof(char));
	char** args = malloc(MAX_ADDRESSBOOK_SIZE * sizeof(char*));
	if ((fp = fopen(filename, "r")) == NULL) return NULL;

	// Scan the whole file
	while (getline(&buf, &len, fp) != -1) {
		char* tmp = malloc(BUF_SIZE * sizeof(char));
		strcpy(tmp, buf);
		tokenize(tmp, &args);
		// If I found a match, I return the whole string as it is in the file
		if (!strcmp(args[0], username)) return buf;
	}


	return NULL;
}

/**
 *  Function that allows a user to select a user from another's addressbook. 
 *  User input can either be the positional number of the user in the addressbook or the username
 * 
 *  @param u the user struct to search in
 *  @param result the result string
 *  @returns the username of the user selected
*/
char* selectUser(t_user* u, char* result) {
	// Print the addressbook
	for (int i = 1; i<=u->addressbook_size; i++) {
		printf("%d\t%s\n", i, u->addressbook[i]);
	}
	printf("\n");

	// Loop until the user selects a valid username
	char* username = (char*)malloc(BUF_SIZE * sizeof(char));
	username = NULL;
	while (username == NULL) {

		// Ask the user to select a user
		printf("Select a user: ");
		char* username = (char*)malloc(BUF_SIZE * sizeof(char));
		fgets(username, BUF_SIZE, stdin);
		username[strlen(username)-1] = '\0';

		// Check if it is a number and if it is within range of the addressbook
		if (isNumber(username) && atoi(username)>0 && atoi(username)<=u->addressbook_size) {
			// Select the i-th user 
			sprintf(result, "%s", u->addressbook[atoi(username)]);
			return result;
		} else {
			// The input is not a number, so i check if the user is in the addressbook
			if (isInAddressBook(username, u)) {
				sprintf(result, "%s", username);
				return result;
			}
		}

		// The user provided an invalid input, so I ask him again
	}
	
	// Not reached
	return NULL;
}

