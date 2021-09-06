///			MAIN CLIENT CODE
#include "client.h"

#define MAXARGS 64

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

    // Connect to the server
    s = connectToSocket(serv_add, serv_port);

    // When I see this, I finally connected and I'm ready to go
    printf("\t -------------------------------\n");
    printf("\t -------------------------------\n");
    printf("\t ------ Voicemail Service ------\n");
    printf("\t -------------------------------\n");
    printf("\t -------------------------------\n\n");
    printf("Type \"help\" to see all commands or start inserting one.\n");

    #ifdef DEBUG
        printf("\t\tDEBUG MODE\n");
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