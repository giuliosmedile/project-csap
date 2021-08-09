#define MESSAGES_EXTENSION ".wav"
char* messagesRepo;
char* userRepo;

/**
 * Function to set the repo from which look for messages
*/
void setMessagesmessagesRepository(char* input) {
    free(messagesRepo);
    messagesRepo = (char*)malloc(BUF_SIZE * sizeof(char));
    //strcpy(messagesRepo, input);
    sprintf(messagesRepo, "%s", input);
    printf("set messagesRepo as %s\n", messagesRepo);
    return;
}

char* searchMessage(char* username, int messageno) {
    char* filename = (char*)malloc(BUF_SIZE * sizeof(char));

    // Messages are stored in /datarepo/data/messages/[username]/[messageno].wav
    sprintf(filename, "./data/messages/%s/%d.%s", username, messageno, MESSAGES_EXTENSION);
    if (access(filename, F_OK) == 0) {
        return filename;
    } else {
        return NULL;
    }
}

void deleteMessage(char* username, int messageno) {
    char* filename = (char*)malloc(BUF_SIZE * sizeof(char));

    // Messages are stored in /datarepo/data/messages/[username]/[messageno].wav
    sprintf(filename, "./data/messages/%s/%d.%s", username, messageno, MESSAGES_EXTENSION);
    if (access(filename, F_OK) == 0) {
        remove(filename);
    }
    return;
}

