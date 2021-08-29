///			MAIN CLIENT CODE
#include "client.h"

#define MAXARGS 64

// The next two are for handling the color output in the console
#define COLOR "\033[0;33m"
#define STD_COL "\033[0m"

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

    #ifdef DEBUG
        printf("\tDEBUG MODE ACTIVE\n");
    #endif

    // Main loop
    for (;;) {
        dowork(u, s);
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