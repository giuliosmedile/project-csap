#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
//#include "../shared/credentials.c"

#define REPO "data/credentials.dat"

void itoa(int, char*, int);
void hash(char**);
FILE* fp;
char* arguments[3];
char delimit[]=";\n";	//delimiter for credentials repository

//function that allows a user to sign up in the system by writing in the repo
int signup (char* username, char* password) {
	if ((fp = fopen(REPO, "a")) == NULL) return 0;
	hash(&password);
	printf("%s, %s\n", username, password);
	fprintf(fp, "%s;%s\n", username, password);
	fclose(fp);
	return 1;
}

int login(char* username, char* password) {
	//check the repository line by line, looking if the password matches
	char* line = malloc(sizeof(char*));
	size_t len;
	char* l_username = malloc(sizeof(char*));
	char* l_password = malloc(sizeof(char*));
	hash(&password);
	if ((fp = fopen(REPO, "r")) == NULL) return -1;
	while (getline(&line, &len, fp) != -1) {
		l_username = strtok(line, delimit);
		if (strcmp(username, l_username)==0) {
			l_password = strtok(NULL, delimit);

			printf("%s, %s, %s, %s\n", username, password, l_username, l_password);

			if (strcmp(password, l_password)==0) {
				printf("Hooray!\n");
				return 1;
			}
		} else {
			continue;
		}
	}
	fclose(fp);
	return 0;
}

//Function needed for itoa
void strreverse(char* begin, char* end) {	
	char aux;	
	while(end>begin)	
		aux=*end, *end--=*begin, *begin++=aux;	
}
	
void itoa(int value, char* str, int base) {
	
	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";	
	char* wstr=str;	
	int sign;	
	div_t res;

	// Validate base
	if (base<2 || base>35){ *wstr='\0'; return; }
	
	// Take care of sign	
	if ((sign=value) < 0) value = -value;
	
	// Conversion. Number is reversed.	
	do {	
		res = div(value,base);
		*wstr++ = num[res.rem];	
	}while(value=res.quot);
	
	if(sign<0) *wstr++='-';	
	*wstr='\0';

	// Reverse string
	strreverse(str,wstr-1);
	
}

void hash(char **str) {
    unsigned long hash = 5381;
    int c;
    char* tmp = *str;
    while (c = *tmp++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    itoa(hash, *str, 10);
}