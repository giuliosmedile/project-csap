//	GENERAL PURPOSE UTILITIES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SEPARATOR " \t\r\n\v\f;="

/** Function that tokenizes a string. Uses marks defined in SEPARATOR to separate strings from one another 
 * @param input the input string
 * @param output the output string vector pointer that is returned
*/
void tokenize(char* input, char*** output) {
    char** tmp = *output;
    
    int i = 0;
    char* token = strtok(input, SEPARATOR);
    while (token) {
        tmp[i] = (char*)malloc(BUF_SIZE * sizeof(char));
        strcpy(tmp[i], token);
        token = strtok(NULL, SEPARATOR);
        i++;
    }

    return;
}
/**
 * Function that tokenizes a string. Uses marks defined in passed variable to separate strings from one another
 * @param input the input string
 * @param output the output string vector pointer that is returned
 * @param separator the separator string
*/
void tokenizeWithSeparators(char* input, char*** output, char* separators) {
    char** tmp = *output;
    
    int i = 0;
    char* token = strtok(input, separators);
    while (token) {
        tmp[i] = (char*)malloc(BUF_SIZE * sizeof(char));
        strcpy(tmp[i], token);
        token = strtok(NULL, separators);
        i++;
    }

    return;
}
// Function that returns the file size in bytes of a file
int get_file_size(char *source)
{
    FILE *fp = fopen(source,"rb");
    fseek(fp,0,SEEK_END);
    int size = ftell(fp);
    fseek(fp,0,SEEK_SET);
    fclose(fp);
    return size;
}

// Function that tells if a string is a number
int isNumber(char s[])
{
    for (int i = 0; s[i]!= '\0'; i++)
    {
        if (isdigit(s[i]) == 0)
              return 0;
    }
    return 1;
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

    DEBUGPRINT(("ABOUT TO HASH: \"%s\"\n", tmp));

    while (c = *tmp++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    itoa(hash, *str, 10);
}
