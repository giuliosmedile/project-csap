#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "signin.c"
// #include "../shared/user.c"

// From main.c
void DieWithError(char* str);
void tokenize(char* input, char*** output);
void dowork(int socket);

// From signin.c
int signup(char* username, char* password);
int login(char* username, char* password);

//From user.c
struct s_user;
typedef struct s_user user;
user createUser(char* username);