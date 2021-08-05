// IMPLEMENTATION OF A LIST OF MESSAGES
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "message.c"

// typedef struct node {
// 	t_message* message;
// 	struct node* next;
// } NODE;

void init_list(NODE** head) {
	*head = NULL;
}

/*
void print_list(NODE* head) {
	NODE* current = head;
	int i = 0;
	while (current != NULL) {
		printf("- User %d: %s\n", i, current->user.username);
		current = current->next;
	}
}
*/

/**
 * Function that adds a new message at the beginning of a list
*/
NODE* add_node(NODE* head, t_message* m) {
	NODE* new = (NODE*)malloc(sizeof(NODE));
	new->message = m;
	new->next = head;
	return new;
}

/**
 * Function that removes a node from a list
*/
void remove_node(NODE* head) {
	NODE* temp = (NODE*) malloc(sizeof (NODE));
    temp = head->next;
    head->next = head->next->next;
    free(temp);
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
		if (strcmp(current->message->sender->username, username) == 0) {
			result = add_node(result, current->message);
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
		if (strcmp(current->message->receiver->username, username) == 0) {
			result = add_node(result, current->message);
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
		if (current->message->is_read == 1) {
			result++;
		}
	}
	return result;
}
