#define BUF_SIZE 256

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
    // TODO
    char* filename = (char*)malloc(BUF_SIZE * sizeof(char));
    return filename;
}

void deleteMessage(char* username, int messageno) {
    // TODO
    return;
}