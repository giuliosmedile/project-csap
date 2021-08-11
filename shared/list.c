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

// Function that adds a message in tail to the list
NODE* add_node(NODE* head, t_message* message) {
	NODE* new = (NODE*) malloc(sizeof(NODE));
	new->message = message;
	new->next = NULL;
	if (head == NULL) {
		head = new;
	} else {
		NODE* current = head;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = new;
	}
	return head;
}

// Function that removes a message from the list
NODE* remove_node(NODE* head, t_message* message) {
	NODE* current = head;
	NODE* previous = NULL;
	while (current != NULL) {
		if (current->message == message) {
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

// Function that removes the i-th node from the list
NODE* remove_i_th_node(NODE* head, int i) {
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
		if (strcmp(current->message->receiver, username) == 0) {
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
		if (current->message->is_read) {
			result++;
		}
	}
	return result;
}
