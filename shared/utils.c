//	GENERAL PURPOSE UTILITIES
#ifdef DEBUG
  #define DEBUGPRINT(a) printf a
#else
  #define DEBUGPRINT(a) (void)0
#endif


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

/**
 * Function that takes an integer and converts it to a string
 * @param value the integer to be converted
 * @param str the result string
 * @param base the base of the integer
**/	
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

/**
 * Simple hashing function.
**/
void hash(char **str) {
    unsigned long hash = 5381;
    int c;
    char* tmp = *str;

    DEBUGPRINT(("ABOUT TO HASH: \"%s\"\n", tmp));

    while (c = *tmp++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    itoa(hash, *str, 10);
}

/**
 * Function that prints a file back to a string.
 * @param filename the file to be printed
 * @param str the string to be printed to
 * @param size the size of the string (to be eventually reallocated)
 * @return the string containing the file
**/
char* printFileToString(char* filename, char* str, int size) {
    str = (char*)malloc(size * sizeof(char));
    DEBUGPRINT(("enter printfiletostring\n"));
    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }
    char c;
    int i = 0;
    while ((c = fgetc(fp)) != EOF) {
        // If i exceed the original string size, i realloc it to double the size
        if (i > size) {
            size *= 2;
            str = (char*)realloc(str, size * sizeof(char));
        }
        str[i] = c;
        i++;
    }
    fclose(fp);
    str[i] = '\0';
    return str;
}

/**
 * Function that writes a string to a file.
 * Attention: rewrites the original file!
 * @param filename the file to be written to
 * @param str the string to be written
 * @return 1 if the operation was successful, 0 otherwise
**/
int printStringToFile(char* filename, char* str) {
    FILE *fp;
    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return 0;
    }
    int result = fprintf(fp, "%s", str);
    if (result < 0) {
        printf("Error writing to file!\n");
        return 0;
    }
    fclose(fp);
    return 1;
}

/**
 * Function that prints out the path of the filename of the latest file in a directory.
 * Uses the command "ls -t" to get the latest file.
 * @param directory the directory to be searched
 * @returns the filename of the latest file in directory 
**/
char* getNewestFileFromDirectory(char* directory) {
    char* result = (char*)malloc(BUF_SIZE * sizeof(char));
    pid_t pid;          // Pid for the fork
    int fd[2];          // File descriptors for the pipe

    // Create the pipe
    if (pipe(fd) == -1) {
        printf("Error creating pipe!\n");
        return NULL;
    }

    // Fork the process to execute the command
    switch(pid = fork()) {
        // Error
        case -1:
            printf("Fork failed!\n");
            exit(1);
        
        // Child process
        case 0:
            // Close the read end of the pipe
            close(fd[0]);
            // Setup the write end of the pipe
            dup2(fd[1], STDOUT_FILENO);
            // Execute the command
            execlp("ls", "ls", "-t", directory, NULL);
            exit(1);

        // Parent process
        default:
            // Close the write end of the pipe
            close(fd[1]);
            // Read the output of the command
            read(fd[0], result, BUF_SIZE);
            // Close the read end of the pipe
            close(fd[0]);
            // Wait for the child process to finish
            wait(NULL);

            // Take just up to the first newline
            char* tmp = strchr(result, '\n');
            if (tmp != NULL) {
                *tmp = '\0';
            }
            return result;
    }   

}



