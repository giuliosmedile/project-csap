// IMPLEMENTATION OF A LIST OF MESSAGES
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "message.c"

#define MAX_MESSAGES 50

char* formatPrintMessage(t_message* m, char* string);
char* printMessage(t_message* m, char* string);
t_message* readMessage(char* string);
t_message* saveMessage(char* filename);
char* print_list(NODE* head, char* result);
int equalsMessage(t_message* m1, t_message* m2);

// typedef struct node {
// 	t_message* message;
// 	struct node* next;
// } NODE;

NODE* init_list(NODE* head) {
	head = (NODE*)malloc(sizeof(NODE));
	head = NULL;
	return head;
}

/** Function that adds a message at the beginning of the list
 * @param head pointer to the list head
 * @param message the message to be added
 * @return the new list head
*/
NODE* add_node(NODE** p_head, t_message* message) {
	NODE* head = *p_head;
	NODE* node = (NODE*)malloc(sizeof(NODE));
	node->message = message;
	node->next = head;
	head = node;

	return head;
}



// Function that removes a message from the list
NODE* remove_node(NODE** p_head, t_message* message) {
	NODE* head = *p_head;
	NODE* current = head;
	NODE* previous = NULL;
	while (current != NULL) {
		if (equalsMessage(current->message, message)) {
			if (previous == NULL) {
				head = current->next;
			} else {
				previous->next = current->next;
			}
			free(current);
			return head;
		}
		previous = current;
		current = current->next;
	}
	return head;
}

// Function that returns the ith message in the list
t_message* getMessage(NODE* head, int i) {
	NODE* current = head;
	int j = 0;
	while (current != NULL) {
		if (j == i) {
			return current->message;
		}
		j++;
		current = current->next;
	}
	return NULL;
}

// Function that removes the i-th node from the list
NODE* remove_ith_node(NODE* head, int i) {
	NODE* current = head;
	NODE* previous = NULL;
	int j = 0;
	while (current != NULL) {
		if (j == i) {
			if (previous == NULL) {
				head = current->next;
			} else {
				previous->next = current->next;
			}
			free(current);
			return head;
		}
		previous = current;
		current = current->next;
		j++;
	}
	return head;
}

/** Function that finds in the input list all the messages that
 * were sent by the same person
 * @param list the list of messages
 * @param user the user that sent the messages
 * @return the list of messages that were sent by the same person
*/ 
NODE* getBySender(NODE* list, char* username) {
	NODE* result = (NODE*) malloc(sizeof(NODE));
	NODE* current = list;

	while (current != NULL) {
		if (strcmp(current->message->sender, username) == 0) {
			result = add_node(&result, current->message);
		}
		current = current->next;
	}
	return result;
}

/**
 * Function that finds in the input list all the messages that
 * were received by the same person
 * @param list the list of messages
 * @param user the user that received the messages
 * @return the list of messages that were received by the same person
*/
NODE* getByReceiver(NODE* list, char* username) {
	NODE* result = (NODE*) malloc(sizeof(NODE));
	NODE* current = list;

	while (current != NULL) {
		if (strcmp(current->message->receiver, username) == 0) {
			result = add_node(&result, current->message);
		}
		current = current->next;
	}
	return result;
}

/**
 * Function that counts how many unread messages are there in a list
 * @param list the list of messages
 * @return the number of unread messages in the list
*/ 
int countUnreadMessages(NODE* list) {
	int result = 0;
	NODE* current = list;

	while (current != NULL) {
		if (current->message->is_read) {
			result++;
		}
	}
	return result;
}

// Function that counts the total number of messages in a list
int count_messages(NODE* list) {
	int result = 0;
	NODE* current = list;
	while (current->next != NULL) {
		current = current->next;
		result++;
	}
	return result;
}

/**
 * Function that prints a list of messages.
 * Intended to be used for user visualization or interaction.
 * @param list the list of messages
 * @param result the string that will contain the lists
 * @return the list of messages in a string
**/
char* print_list(NODE* head, char* result) {
	result = (char*)malloc(BUF_SIZE * sizeof(char));
	char* temp = (char*)malloc(BUF_SIZE * sizeof(char));
	NODE* current = head;
	for(int i = 1; i <= count_messages(head) && current != NULL; i++) {
		sprintf(temp, "\tMessage number %d", i);
		strcat(result, temp);

		sprintf(temp, "\n%s\n", formatPrintMessage(current->message, ""));
		strcat(result, temp);
		current = current->next;
	}
	free(temp);
	return result;
}

/**
 * TEMP: The function should return a list, but will instead temporarily return the string, in a similar fashion to print_list_from_string. 
 * 
 * //
 * Function that reads messages from a file, and returns a list of all the messages with the same receiver
 * @param filename the name of the file
 * @param username the username of the user who received the messages
 * @return the list of messages
*/
char* getByReceiverFromFile(char* filename, char* username) {
	// NODE* result = (NODE*)malloc(MAX_MESSAGES * sizeof(NODE));
	FILE* fp;
	char* buf = malloc(BUF_SIZE * sizeof(char));
	char* result = malloc(BUF_SIZE * sizeof(char));	//TEMP
	char* temp = malloc(BUF_SIZE * sizeof(char));	//TEMP
	size_t len;
	int i = 0;

	if ((fp = fopen(filename, "r")) == NULL) {
		return NULL;
	}

	// Read the file line by line
	while (getline(&buf, &len, fp) != -1) {		
		// If the string is newline terminated, remove '\n'
		if (buf[strlen(buf)-1] == '\n') {
			buf[strlen(buf)-1] = '\0';
		}

		// To make it more efficient, I first tokenize the file line, and compare just if the receiver (second arg) is the same
		char** args = malloc(MAX_MESSAGES * sizeof(char*));
		// I must use a temp variable or else the original buf will be modified
		char* tmp = malloc(BUF_SIZE * sizeof(char));
		strcpy(tmp, buf);
		tokenize(tmp, &args);
		printf("Compare: args: %s, username: %s\n", args[1], username);

		if (strcmp(args[1], username) == 0) {
			printf("adding message from %s to list\n", args[0]);
			// If the receiver is the same, add the message to the list
			t_message* m = (t_message*) malloc(sizeof(t_message));
			buf[strlen(buf)-1] = '|';

			strcat(result, buf);
			i++;

			//// TEMP TEMP TEMP
			// m = readMessage(buf);
			// result = add_node(&result, m);
			//// TEMP TEMP TEMP
		}
		free(args);
		free(tmp);
	}
	//TEMP result->next = NULL;
	char* tempS = (char*)malloc(BUF_SIZE * sizeof(char));
	sprintf(tempS, "%d|%s", i, result);
	tempS[strlen(tempS)-1] = '\0';

	//printf("testing if messages added correctly in getByReceiverFromFile: %s\n", print_list(result, ""));

	free(buf);
	fclose(fp);
	return tempS;
}

/**
 * Function that prints a list to a string, to be either saved or sent to the client. 
 * The string is structured in ths way: 
 * 			<total_messages>;<message_string>;<message_string>;...
 * @param list the list of messages
 * @return the string with the list of messages
*/
char* print_list_to_string(NODE* list) {
	NODE* tmp = list;
	char* result = (char*)malloc(BUF_SIZE * sizeof(char));
	int i = 0;

	// Count the number of messages
	sprintf(result, "%d", count_messages(list));

	while (tmp != NULL) {
		char* temp = (char*)malloc(BUF_SIZE * sizeof(char));

		// Just in this case, the messages are separated by |
		sprintf(temp, "|%s", printMessage(tmp->message, ""));
		strcat(result, temp);
		tmp = tmp->next;
		i++;
	}

	// Before returning, i must add the number of messages at the beginning
	//sprintf(result, "%d|%s", i, result);
	return result;
}

/**
 * Function that retrieves a list from a string, as ouput by the function print_list_to_string.
 * @param string the string with the list of messages
 * @return the list of messages
**/
NODE* get_list_from_string(char* string) {
	// Tokenize the input
	char** args = (char**)malloc(MAX_MESSAGES * sizeof(char*));
	char* tmp = (char*)malloc(BUF_SIZE * sizeof(char));
	strcpy(tmp, string);
	tokenizeWithSeparators(tmp, &args, "|");

	/// Testing tokenize
	for (int i = 0; i<5; i++) {
		printf("%d\t%s\n", i, args[i]);
	}
	///

	// The first token is the number of messages
	int num_messages = atoi(args[0]);

	// Double check if there are messages 
	// (this also acts as a sanity check, as if there are no numbers in the first token the result will be 0)
	if (num_messages == 0) return NULL;

	NODE* result = (NODE*)malloc(num_messages * sizeof(NODE));

	// Add all the messages to result
	NODE* current = result;
	for (int i = 1; i<=num_messages; i++) {
			current->message = readMessage(args[i]);
			current->next = (NODE*)malloc(sizeof(NODE));
			current = current->next;
		}
	current->next = NULL;

	free(args);
	free(tmp);
	return result;
}

NODE* searchForMessagesInRange(char* username, time_t lower, time_t upper, char* repository) {
	char* buf = malloc(BUF_SIZE * sizeof(char));
	size_t len;
	NODE* result = (NODE*)malloc(sizeof(NODE));
	NODE* current = result;

	printf("lower: %ld, upper: %ld\n", lower, upper);

	puts("open file in searchformessages");
	// Open the file
	FILE* fp;
	if ((fp = fopen(repository, "r")) == NULL) {
		puts("error opening file");
		return NULL;
	}

	// Read the file line by line
	int i;
	while ((i = getline(&buf, &len, fp)) != -1) {
		printf("%d\n", i);
		// If the string is newline terminated, remove '\n'
		if (buf[strlen(buf)-1] == '\n') {
			buf[strlen(buf)-1] = '\0';
		}
		printf("buffer: \"%s\"\n", buf);

		// Tokenize the buffer
		char** args = malloc(MAX_MESSAGES * sizeof(char*));
		char* tmp = malloc(BUF_SIZE * sizeof(char));
		strcpy(tmp, buf);
		tokenize(tmp, &args);

		// Check if the receiver is the one we are looking for
		if (strcmp(args[1], username) == 0) {
			// Check if the message is in the range
			time_t timestamp = atoi(args[2]);
			if (timestamp >= lower && timestamp <= upper) {
				printf("\tadding buf: %s\n", buf);
				// Add the message to the list
				current->message = readMessage(buf);
				current->next = malloc(sizeof(NODE));
				current = current->next;
			}
		}
		free(args);
		free(tmp);
	}
	puts("after while");

	// Close the file, lear memory and return 
	fclose(fp);
	free(buf);
	puts("returning");
	return result;
}

/**
 * Function that searches for a given message in a list, and returns the message pointer if found.
 * @param list the list of messages
 * @param message the message to search for
 * @return the message pointer if found, NULL otherwise
**/
t_message* searchForMessageInList(NODE* list, t_message* m) {
	NODE* tmp = list;
	while (tmp != NULL) {
		if (equalsMessage(tmp->message, m)) {
			return tmp->message;
		}
		tmp = tmp->next;
	}
	return NULL;
}

/**
 * Function that, given a list of messages, flags a certain message as read.
 * Uses equalsMessage() to check if the messages are the same.
 * @param list the list of messages
 * @param message the message to flag as read
 * @return 1 if the message was found and flagged, 0 otherwise
**/
int flagMessageAsReadInList(NODE* list, t_message* m) {
	NODE* tmp = list;

	while (tmp != NULL) {
		if (equalsMessage(tmp->message, m)) {
			tmp->message->is_read = 1;
			return 1;
		}
		tmp = tmp->next;
	}
	return 0;
}
