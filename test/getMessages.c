#include "../datarepo/datarepo.h"

int main (int argc, char *argv[]) {
    
    char* result = malloc(sizeof(char) * BUF_SIZE);
    result = printFileToString("demoMessagesRepo.txt", result, BUF_SIZE);

    printf("%s\n", result);
    printf("size: %d\n", strlen(result));


    return 0;
}