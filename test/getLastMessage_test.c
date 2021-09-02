#include "../datarepo/datarepo.h"

int main (int argc, char *argv[]) {

    // Get the last message and print it out!
    char* result = getLastMessage("../datarepo/data/messages.txt");

    printf("%s\n", result);


    return 0;
}