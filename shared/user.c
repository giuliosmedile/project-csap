#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "llist.h"

typedef struct user {
	char* username;
	llist addressbook;
} t_user;

t_user createUser(char* username) {
	return;
}