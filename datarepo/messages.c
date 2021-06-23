#define BUF_SIZE 256
#define MESSAGES_EXTENSION ".wav"
char* repo;
/**
 * Function to set the repository from which look for messages
*/
void setMessagesRepository(char* input) {
    repo = (char*)malloc(BUF_SIZE * sizeof(char));
    strcpy(repo, input);
    return;
}

char* searchMessage(char* username, int messageno) {
    char* filename = (char*)malloc(BUF_SIZE * sizeof(char));

    // Messages are stored in /datarepo/data/messages/[username]
    sprintf(filename, "./data/messages/%s/%d.%s", username, messageno, MESSAGES_EXTENSION);
    if (access(filename, F_OK) == 0) {
        return filename;
    } else {
        return NULL;
    }
}

void deleteMessage(char* username, int messageno) {
    char* filename = (char*)malloc(BUF_SIZE * sizeof(char));

    // Messages are stored in /datarepo/data/messages/[username]
    sprintf(filename, "./data/messages/%s/%d.%s", username, messageno, MESSAGES_EXTENSION);
    if (access(filename, F_OK) == 0) {
        remove(filename);
    }
    return;
}