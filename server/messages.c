// FUNCTIONS TO HANDLE MESSAGES AND ADDRESSBOOK

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define TMP_DIR "/var/tmp/project-csap/server"     //the temporary directory in which the files will be saved"

/**
 *  Function to add a user to another's addressbook.
 * @param user the user who wants to add @param other to its addressbook
 * @returns 1 if operation succeeded, 0 otherwise
*/
int add(char* user, char* other) {
    char* line = (char*)malloc(BUF_LEN * sizeof(char));
	size_t len;
	char* l_username = malloc(sizeof(char*));
    int found = 0;      // tells me if i found the username in REPO

    if ((fp = fopen(REPO, "r")) == NULL) return 0;
	while (getline(&line, &len, fp) != -1) {
		l_username = strtok(line, DELIMIT);
		printf("Comparing: %s - %s\n", other, l_username);
		if (strcmp(other, l_username)==0) {
			// I found already a user already registered with that name
            fclose(fp);
			return 1;
		}
	}

    // If i'm here, it means i haven't found anyone matching
    fclose(fp);
    return 0;
}

// TODO: add hash check to see if file was sent correctly
int record(char* filename, char* _filesize, int socket) {

	int filesize = atoi(_filesize);
	// Create the temp directory, it if is not there
    struct stat st = {0};
    if (stat(TMP_DIR, &st) == -1) {
        mkdir(TMP_DIR, 0755);
    }

	// Define the path I'll save into
	char* path = (char*)malloc(BUF_SIZE * sizeof(char));
	sprintf(path, "%s/%s", TMP_DIR, filename);

	// Wait for the file from the socket
	receiveFile(socket, path, filesize);

	return 0;
}