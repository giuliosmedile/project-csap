// Test to ensure that getNewestFileFromDirectory() returns the last file in a directory.

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "../datarepo/datarepo.h"

int main(int argc, char *argv[]) {

    char *dir = "/var/tmp/project-csap/datarepo";
    char *file = getNewestFileFromDirectory(dir);

    printf("Last file: \"%s\"\n", file);
    exit(0);

}
