// IMPLEMENTATION OF A LIST OF USERS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "user.c"

typedef struct node {
	t_user user;
	struct node* next;
} NODE;

void new(NODE** head) {
	*head = NULL;
}

void print_list(NODE* head) {
	NODE* current = head;
	int i = 0;
	while (current != NULL) {
		printf("- User %d: %s\n", i, current->user.username);
		current = current->next;
	}
}

NODE* add_node(NODE* head, t_user u) {
	NODE* new = (NODE*)malloc(sizeof(NODE));
	temp->user = u;
	temp->next = head;
	return head;
}

void remove_node(NODE* head) {
	NODE* temp = (NODE*) malloc(sizeof (NODE));
    temp = head->next;
    head->next = head->next->next;
    free(temp);
}

t_user search_list(NODE* head, char* username) {
	NODE* tmp = head;
	while (tmp != NULL) {
		if (!strcmp(tmp->user.username, username)) return tmp->user;
		tmp = tmp->next;
	}
	return NULL;
}

int contains(t_user user, LIST* list) {
	int result = 0;
	NODE* current = list->head;
	while (current != NULL) {
		if(compareUsers(current, user)) return 0;
		current = current->next;
	}
}
