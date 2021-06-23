///			MAIN CLIENT CODE
#include "client.h"

#define MAXARGS 64
#define BUF_SIZE 256

// The next two are for handling the color output in the console
#define COLOR "\033[0;33m"
#define STD_COL "\033[0m"

int s;      // The socket I'm interacting with
t_user u;   // The user that the socket sends after logging in

void help() {
    printf("Currently supported commands:\n");
    printf("%s", COLOR);
    printf("\tlogin\n");
    printf("\tsignup\n");
    printf("\texit\n");
    printf("\trecord\n");
    printf("\tlisten\n");
    printf("%s", STD_COL);
    return;
}

char* takeUserInput(char* input) {
    printf("%s", COLOR);
    printf("$: ");
    if (fgets(input,sizeof(input),stdin) == NULL) {
        printf("\n");
        exit(0);
    }
    printf("%s", STD_COL);

    return input;
}

char* interpretInput(char* command, char* output) {
    command[strlen(command)-1] = '\0';

    if (!strcmp(command, "login")) {
        login(output);
    } else if (!strcmp(command, "signup")) {
        signup(output);
    } else if (!strcmp(command, "exit")) {
        exit(0);
    } else if (!strcmp(command, "help")) {
        help();
    } else {
    	// Command not found
    	strcpy(output, "null");
	}

    return output;
}

void main (int argc, char** argv) {
    char buf[BUF_SIZE];
    char command[BUF_SIZE];
    char* response = (char*)malloc(BUF_SIZE * sizeof(char));
    char output[BUF_SIZE];
    char **args;
    char* str = (char*)malloc(BUF_SIZE * sizeof(char));

    //TODO: AUTOMATIC CONNECTION TO SERVER

    // Read the configuration file
    char* serv_add = (char*)malloc(sizeof(BUF_SIZE * sizeof(char)));
    unsigned short serv_port;
    readConfig(&serv_add, &serv_port);
    printf("Server address: %s\n", serv_add);

    s = connectToSocket(serv_add, serv_port);

    printf("\t ---------------------------\n");
    printf("\t ---- Voicemail Service ----\n");
    printf("\t ---------------------------\n");
    // Main loop
    for (;;) {
        // take the user input
        takeUserInput(buf);
        strcpy(command, buf);
        command[strlen(command)-1] = '\0';
        // interpret it basing on the command
        interpretInput(buf, output);
        if (!strcmp(output, "null")) {
        	continue;
        }
        // send the command and arguments to socket
        sendToSocket(s, output);
        //wait for reply
        response = readFromSocket(s, buf);
        // Then handle the server response
        handleServerReplies(command, response);

        // Reset all strings
        memset(buf,0,BUF_SIZE);
        memset(command,0,BUF_SIZE);
        memset(output,0,BUF_SIZE);
        memset(response,0,BUF_SIZE);

    }
}