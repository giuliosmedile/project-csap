///			MAIN CLIENT CODE
#include "client.h"

#define MAXARGS 64

// The next two are for handling the color output in the console
#define COLOR "\033[0;33m"
#define STD_COL "\033[0m"

// int s;              // The socket I'm interacting with (server)
// t_user* u = NULL;   // The user that the socket sends after logging in

void restartOnError(int);  //definition of signal handler

void main (int argc, char** argv) {
    char* buf = (char*)malloc(BUF_SIZE * sizeof(char));
    char* command = (char*)malloc(BUF_SIZE * sizeof(char));
    char* response = (char*)malloc(BUF_SIZE * sizeof(char));
    char* output = (char*)malloc(BUF_SIZE * sizeof(char));
    char **args;
    char* str = (char*)malloc(BUF_SIZE * sizeof(char));

    signal(SIGSEGV, restartOnError);

    // Read the configuration file
    char* serv_add = (char*)malloc(sizeof(BUF_SIZE * sizeof(char)));
    unsigned short serv_port;
    readConfig(&serv_add, &serv_port);
    printf("Server address: %s\n", serv_add);

    s = connectToSocket(serv_add, serv_port);

    printf("\t -------------------------------\n");
    printf("\t -------------------------------\n");
    printf("\t ------ Voicemail Service ------\n");
    printf("\t -------------------------------\n");
    printf("\t -------------------------------\n");
    // Main loop
    for (;;) {
        dowork(u, s);
    //     // As some function (such as RECORD) may change the signal behavior, I make sure to reset them at each iteration
    //     signal(SIGQUIT, SIG_DFL);
    //     signal(SIGKILL, SIG_DFL); 

    //     // init variables
    //     buf = (char*)malloc(BUF_SIZE * sizeof(char));
    //     command = (char*)malloc(BUF_SIZE * sizeof(char));
    //     output = (char*)malloc(BUF_SIZE * sizeof(char));
    //     response = (char*)malloc(BUF_SIZE * sizeof(char));

    //     // take the user input
    //     takeUserInput(buf, u);
    //     strcpy(command, buf);
    //     command[strlen(command)-1] = '\0';

    //     // interpret it basing on the command
    //     interpretInput(buf, output);
    //     // if input is invalid (null) restart
    //     if (!strcmp(output, "null")) {
    //         free(buf);
    //         free(command);
    //         free(output);
    //         free(response);
    //     	continue;
    //     }

    //     // send the response to the server only if the output is NOT skipsend
    //     if (strcmp(output, "skipsend")) {
    //         // send the command and arguments to socket
    //         sendToSocket(s, output);
    //     }

    //     //wait for reply
    //     free(response);
    //     response = (char*)malloc(BUF_SIZE * sizeof(char));
    //     response = readFromSocket(s, buf);

    //     // Then handle the server response
    //     u = handleServerReplies(command, response, u);

    //     // finally reset all variables
    //     free(buf);
    //     free(command);
    //     free(output);
    //     free(response);

    // }
    }
}


// Function that restarts the client from main when fatal signals get caught
// Set to handle most fatal signals (SIGQUIT, SIGKILL, SIGSEGV)
void restartOnError(int signum) {
	printf("\033[0;33mAn error %d has occurred. Restarting software.\033[0m\n", signum);
    close(s);
    u = NULL;
    signal(SIGSEGV, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGKILL, SIG_DFL);
	main(0, NULL);
}