//	GENERAL PURPOSE UTILITIES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEPARATOR " \t\r\n\v\f;"

void tokenize(char* input, char*** output) {
    printf("tokenize\n");
    char** tmp = *output;
    
    int i = 0;
    char* token = strtok(input, SEPARATOR);
    while (token) {
        printf("Token %d: %s\n", i, token);
        tmp[i] = (char*)malloc(BUF_SIZE * sizeof(char));
        strcpy(tmp[i], token);
        token = strtok(NULL, SEPARATOR);
        i++;
    }

    return;
}