#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include "signin.c"

void main(int argc, char** argv) {
	if (argc != 4) {
		printf("\tUsage: %s n username password\n", argv[0]);
	}

	int selector = atoi(argv[1]);
	char* username = malloc(50);
	char* password = malloc(50);
	strcpy(username, argv[2]);
	strcpy(password, argv[3]);
	int result;

	switch (selector) {
		case 1:
			printf("Signup\n");
			result = signup(username, password);
			printf("Result: %d\n", result);
			break;
		case 2:
			printf("Signin\n");
			result = signin(username, password);
			printf("Result: %d\n", result);
			break;
		default:
			exit(0);
	}


}