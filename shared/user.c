#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "llist.h"

#define BUF_SIZE 256

struct s_user {
	char* username;
	llist addressbook;
};
typedef struct s_user t_user;

t_user createUser(char* username) {
	t_user u;
	memset(&u, 0, sizeof(t_user));
	u.username = (char*)malloc(BUF_SIZE * sizeof(char));
	strcpy(u.username, username);
	return u;
}