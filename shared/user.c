#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

typedef struct user_credentials {
	char* username;
	char* password;
} t_credentials;

typedef struct user_data {
	t_credentials credentials;
	llist addressbook;
} t_user_data;

