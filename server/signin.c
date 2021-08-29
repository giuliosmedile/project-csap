#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define BUF_LEN 256
#define DELIMIT ";\n"


void itoa(int, char*, int);
void hash(char**);
FILE* fp;
char* arguments[3];

//function that allows a user to sign up in the system by writing in the repo
int signup (char* username, char* password) {
	char* line = (char*)malloc(BUF_LEN * sizeof(char));
	size_t len;
	char* l_username = malloc(sizeof(char*));

	// First check if a user with that name already exists
	if ((fp = fopen(CREDENTIALS_REPO, "r")) == NULL) return 0;
	while (getline(&line, &len, fp) != -1) {
		l_username = strtok(line, DELIMIT);
		printf("Comparing: %s - %s\n", username, l_username);
		if (strcmp(username, l_username)==0) {
			// I found already a user already registered with that name
			return 0;
		}
	}

	// Hooray! There is no user with that name already, so i can sign it up
	if ((fp = fopen(CREDENTIALS_REPO, "a")) == NULL) return 0;

	printf("%s, %s\n", username, password);
	fprintf(fp, "%s;%s\n", username, password);
	fclose(fp);

	//Then I create a new entry in the users file
	t_user* u = createUser(username);
	return 1;
}

int login(char* username, char* password) {
	//check the repository line by line, looking if the password matches
	char* line = malloc(sizeof(char*));
	size_t len;
	char* l_username = malloc(sizeof(char*));
	char* l_password = malloc(sizeof(char*));

	if ((fp = fopen(CREDENTIALS_REPO, "r")) == NULL) {
		printf("[] can't read repo\n");
		return -1;
	}
	while (getline(&line, &len, fp) != -1) {
		l_username = strtok(line, DELIMIT);
		if (strcmp(username, l_username)==0) {
			l_password = strtok(NULL, DELIMIT);
			printf("%s, %s, %s, %s\n", username, password, l_username, l_password);
			if (strcmp(password, l_password)==0) {
				return 1;
			}
		} else {
			continue;
		}
	}
	fclose(fp);
	return 0;
}