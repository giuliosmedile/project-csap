///			MAIN CLIENT CODE
#include "client.h"

#define MAXARGS 64
#define BUF_SIZE 256

// The next two are for handling the color output in the console
#define COLOR "\033[0;33m"
#define STD_COL "\033[0m"

int s;       // The socket I'm interacting with (server)
t_user* u = NULL;   // The user that the socket sends after logging in

void restartOnError(void);  //definition of signal handler

void help() {
    printf("Currently supported commands:\n");
    printf("%s", COLOR);
    printf("\tlogin\n");
    printf("\tlogout\n");
    printf("\tsignup\n");
    printf("\texit\n");
    printf("\trecord\n");
    printf("\tlisten\n");
    printf("%s", STD_COL);

    printf("TEST USER\n");
    char* tmp;
    tmp = formatPrintUser(u, tmp);
    printf("%s", tmp);
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
        login(output, u);
    } else if (!strcmp(command, "signup")) {
        signup(output, u);
    } else if (!strcmp(command, "logout")) {
        logout(output, u);
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
    char* buf = (char*)malloc(BUF_SIZE * sizeof(char));
    char* command = (char*)malloc(BUF_SIZE * sizeof(char));
    char* response = (char*)malloc(BUF_SIZE * sizeof(char));
    char* output = (char*)malloc(BUF_SIZE * sizeof(char));
    char **args;
    char* str = (char*)malloc(BUF_SIZE * sizeof(char));

    signal(SIGSEGV, restartOnError);

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
        // init variables
        buf = (char*)malloc(BUF_SIZE * sizeof(char));
        command = (char*)malloc(BUF_SIZE * sizeof(char));
        output = (char*)malloc(BUF_SIZE * sizeof(char));
        response = (char*)malloc(BUF_SIZE * sizeof(char));

        // take the user input
        takeUserInput(buf);
        strcpy(command, buf);
        command[strlen(command)-1] = '\0';

        // interpret it basing on the command
        interpretInput(buf, output);
        if (!strcmp(output, "null")) {
            free(buf);
            free(command);
            free(output);
            free(response);
        	continue;
        }

        // send the command and arguments to socket
        sendToSocket(s, output);

        //wait for reply
        response = readFromSocket(s, buf);

        // Then handle the server response
        u = handleServerReplies(command, response, u);

        // finally reset all variables
        free(buf);
        free(command);
        free(output);
        free(response);

    }
}


/* Function that restarts the client from main when SISEGV gets caught */
void restartOnError(void) {
	printf("\033[0;33mAn error has occurred. Restarting software.\033[0m\n");
	main(0, NULL);
}