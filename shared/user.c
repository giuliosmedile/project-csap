#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
//#include "list.c"

// #define BUF_SIZE 4096
#define MAX_ADDRESSBOOK_SIZE 50
#define MAX_MESSAGES 50
#ifdef TEST
	char* repo = "test.txt";
#else
	char* repo = "../datarepo/data/users.txt";
#endif


// Function prototypes
void removeDuplicates(char* username, char* filename);
t_user* searchUser(char* username, char* filename);
t_message* saveMessage(char* filename);					// this is in message.c
t_message* createMessage(char* sender, char* receiver, char* filename, time_t timestamp, int is_read);
char* formatPrintMessage(t_message* m, char* string); 	// this is in message.c
void saveInRepository(t_message* m, char* filename); 	// this is in message.c

/**
 * Initializes a user. 
 * All numeric parameters are set to zero.
 * 
 * @username: the username of the new user
 * @return: the user struct of the new user
*/
t_user* createUser(char* username) {
	// Allocate the space for that user
	t_user* u = malloc(sizeof(t_user)+MAX_ADDRESSBOOK_SIZE*sizeof(char)+MAX_MESSAGES*sizeof(t_message));

	// Fill the struct
	u->username = (char*)malloc(BUF_SIZE * sizeof(char));
	strcpy(u->username, username);
	u->addressbook_size = 0;
	*(u->addressbook) = malloc(MAX_ADDRESSBOOK_SIZE * sizeof(char*));
	u->messagesno = 0;
	u->messages = init_list(u->messages);
	return u;
}

// Similar to saveUser, just saves the result to a string
char* printUser(t_user* u, char* string) {

	char* buf = (char*)malloc(BUF_SIZE * sizeof(char));

	// Write the username
	sprintf(buf, "%s;", u->username);

	// Write the number of messages sent
	char tmp[BUF_SIZE];
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
	char* tmpList = (char*)malloc(BUF_SIZE* sizeof(char));
	for (int i = 1; i<=u->messagesno; i++) {
		printf("testing filename in printuser: %s\n", temp->message->filename);
		sprintf(tmp, "%s", printMessage(temp->message, tmp));
		strcat(buf, tmp);
		temp = temp->next;
	}


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
	sprintf(string, "%s", tmp);

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
	if (u->messagesno != 0) {
		sprintf(tmp, "-- Messages --\n");
		strcat(string, tmp);
		NODE* temp = u->messages;
		for (int i = 1; i<=u->messagesno; i++) {
			sprintf(tmp, "\tMessage number %d\n", i);
			strcat(string, tmp);

			tmp = formatPrintMessage(temp->message, tmp);
			strcat(string, tmp);
			temp = temp->next;
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

	// Let's see if the user was passed corretly DEBUG
	// printf("%s\n", print_list(u->messages, ""));

	FILE* fp;

	// Open the file to append this user
	if ((fp = fopen(filename, "a")) == NULL) {
		printf("err: could not open file %s", filename);
		return;
	}
	
	// Call the printUser function to print the user
	char* string = (char*)malloc(BUF_SIZE * sizeof(char)); 
	string = printUser(u, string);

	fprintf(fp, "%s\n", string);
	fclose(fp);
	free(string);
	return;
}

// Function that returns a user struct after reading a line
// The line _MUST_ be formatted as shown in the function above this
t_user* readUser(char* line) {


	// Tokenize the line
	char** args = malloc((3+MAX_ADDRESSBOOK_SIZE+MAX_MESSAGES * 5) * sizeof(char*));
	tokenize(line, &args);

	// Allocate the space for the new user
	// I can use args[2] because I know that is the number of people in the addressbook
	// and args[1] as the numbers of messages sent
	// t_user* u = malloc(sizeof(t_user) + atoi(args[1]+1*sizeof(NODE)));  //+atoi(args[2])*sizeof(char)+atoi(args[1])+1*sizeof(NODE));

	// Now I can start filling the struct
	t_user* u = createUser(args[0]);
	u->messagesno = atoi(args[1]);
	u->addressbook_size = atoi(args[2]);
	*(u->addressbook) = malloc(MAX_ADDRESSBOOK_SIZE * sizeof(char*));
	for (int i = 1; i <= u->addressbook_size; i++) {
		// I first allocate the memory for the string, then copy it over
		u->addressbook[i] = (char*)malloc(BUF_SIZE * sizeof(char));
		strcpy(u->addressbook[i], args[2+i]);
	}

	// Fill the messages
	if (atoi(args[1]) != 0) {
		u->messages = malloc(sizeof(NODE)); //(NODE*)malloc(atoi(args[1])+1 * sizeof(NODE));
		NODE* temp = u->messages;
		// Iterate from the first message to the end of the string
		// I should iterate from the third item plus the number of people in the addressbook
		// and up to 5 times the number of messages (as each message has 5 items) plus the number of people in the addressbook
		for (int i = 3+atoi(args[2]); i<5*atoi(args[1])+3+atoi(args[2]); i+=5) {
			// Messages are saved in the following format:
			//		from;to;timestamp;is_read;filename;
			time_t timestamp = (time_t) atoi(args[i+2]);
			temp->message = createMessage(args[i], args[i+1], args[i+4], timestamp, atoi(args[i+3]));
			temp->next = (NODE*)malloc(sizeof(NODE));
			temp = temp->next;
		}
		temp->next = NULL;
	} else {
		u->messages = NULL;
	}
	
	free(args);
	return u;
}

t_user* addUserToAddressBook(t_user* u, char* username) {
	// First check if the user I'm adding exists
	t_user* tmp = searchUser(username, repo);
	if(tmp==NULL) return NULL;

	// After I checked, I can add the user
	int i = ++(u->addressbook_size);
	u->addressbook[i] = malloc(BUF_SIZE * sizeof(char));
	strcpy(u->addressbook[i], username);
	return u;
}

/**
 * Function that removes a user from another's addressbook
 * @param u the user that will have the addressbook updated
 * @param username the user that will be removed from the addressbook
**/
t_user* removeUserFromAddressBook(t_user* u, char* username) {
	DEBUGPRINT(("entering removeuserfromaddressbook\n"));

	// I create a temporary addressbook,s that will be used to replace the original one
	char** tmp = malloc(u->addressbook_size * sizeof(char*));
	int i, j = 1;
	for (i = 1; i <= u->addressbook_size; i++) {
		DEBUGPRINT(("u->[i]: \"%s\", username: \"%s\"\n", u->addressbook[i], username));
		if (strcmp(u->addressbook[i], username) != 0) {
			DEBUGPRINT(("copying over %s\n", u->addressbook[i]));
			tmp[j] = malloc(BUF_SIZE * sizeof(char));
			strcpy(tmp[j], u->addressbook[i]);
			j++;
		}
	}
	DEBUGPRINT(("end of loop in removeuser\n"));
	u->addressbook_size--;
	for (i = 1; i <= u->addressbook_size; i++) {
		DEBUGPRINT(("Copyinng %s to the %d i in addressbook\n", tmp[i], i));
		// free(u->addressbook[i]);
		// u->addressbook[i] = malloc(BUF_SIZE * sizeof(char));
		strcpy(u->addressbook[i], tmp[i]);
	}

	// Free the memory
	DEBUGPRINT(("exiting removeuserfromaddressbook\n"));

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
	if ((fp = fopen(filename, "r")) == NULL) {
		return NULL;
	}

	while (getline(&buf, &len, fp) != -1) {
		
		// If the string is newline terminated, remove '\n'
		if (buf[strlen(buf)-1] == '\n') {
			buf[strlen(buf)-1] = '\0';
		}
		printf("searchuser buf: \"%s\"\n", buf);

		// To make it more efficient, I first tokenize the file line, and compare just if the username (first arg) is the same
		char** args = malloc((2+MAX_ADDRESSBOOK_SIZE+MAX_MESSAGES) * sizeof(char*));
		// I must use a temp variable or else the original buf will be modified
		char* tmp = malloc(BUF_SIZE * sizeof(char));
		strcpy(tmp, buf);
		tokenize(tmp, &args);
		printf("Compare: args: %s, username: %s\n", args[0], username);
		if (strcmp(args[0], username) == 0) {
			// Read user from buffer line
			t_user* u = (t_user*)malloc(sizeof(t_user));
			u = readUser(buf);
			return u;
		}
	}
	fclose(fp);
	return NULL;
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

	free(buf);
	free(args);
	return;
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

	if (u->addressbook_size == 0) {
		printf("[!] You have no users in your addressbook.\n");
		return "";
	}

	// Print the addressbook
	for (int i = 1; i<=u->addressbook_size; i++) {
		printf("%d\t%s\n", i, u->addressbook[i]);
	}

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

/**
 * Function that adds a message to a user struct and saves it in the repo file
 * @param u the user struct to add the message to
 * @param filename the filename of the message i want to add
 * @returns the user struct with the message added
*/
t_user* addMessageToUser(t_user* u, char* filename) {

	t_message* message = (t_message*)malloc(sizeof(t_message));
	message = saveMessage(filename);
	saveInRepository(message, MESSAGES_REPO);

	// add the message to the user's message list
	u->messages = add_node(&(u->messages), message);
	u->messagesno++;


	return u;
}

/**
 * Function that adds a message to a user struct. 
 * Differently from addMessageToUser, it does not save the message in the repo file
 * @param u the user struct to add the message to
 * @param message the message to add
 * @returns the user struct with the message added
**/
t_user* addMessageToUserNoRepo(t_user* u, t_message* message) {

	// add the message to the user's message list
	u->messages = add_node(&(u->messages), message);
	u->messagesno++;

	return u;
}

/**
 * Function that removes a message from a user's messages list. 
 * It does not remove the message from the repo file.
 * 
 * @param u the user struct to remove the message from
 * @param message the message to remove
 * @returns the user struct with the message removed
**/
t_user* removeMessageFromUserNoRepo(t_user* u, t_message* message) {

	// remove the message from the user's message list
	u->messages = remove_node(&(u->messages), message);
	u->messagesno--;

	return u;
}

