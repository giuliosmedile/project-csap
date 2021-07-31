//	GENERAL PURPOSE UTILITIES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEPARATOR " \t\r\n\v\f;=_"
#define BUF_SIZE 256

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
