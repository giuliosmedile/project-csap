// This code will allow me to test and explore how rec and play work
// alongside storage options and metadata reading

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define TMP_DIR "/var/tmp/project-csap"     //the temporary directory in which the files will be saved
#define REC "/usr/bin/rec"                  //where rec is located
#define PLAY "/usr/bin/play"                //where play is located

// Function to handle child exiting 
void childExit(int t)
{
  // printf("Child has exited: %d\n", t);
  //  exit(1);
}

// Function to handle ^C
void haltedProg(int t)
{
  // printf("Halted: %d\n", t);
  //  exit(1);
}

void main(int argc, char** argv) {
    char *args[3];
    int pid, status;
    int res;
    char* filename = (char*)malloc(256 * sizeof(char));
    char* input = (char*)malloc(256 * sizeof(char));

    // Associate the functions to the signal
    signal(SIGCHLD,childExit);
    signal(SIGINT,haltedProg);

    // Get and sanitize input
    printf("Tell me the filename you want for the recording: ");
    if (fgets(input,sizeof(input),stdin) == NULL) {
        printf("\n");
        exit(0);
    }
    printf("\n");
    input[strlen(input)-1] = '\0';
    sprintf(filename, "%s/%s.wav", TMP_DIR, input);

    struct stat st = {0};

    // Create the temp directory, it if is not there
    if (stat(TMP_DIR, &st) == -1) {
        mkdir(TMP_DIR, 0755);
    }

    // Fork child to record audio message
    switch(pid=fork()) {
        // Error
        case -1:
            perror("fork");
            exit(1);

        // Child
        case 0:
            ;   //must put this here or else the association on the next line won't work
            // Exec program
            args[0] = REC;
            args[1] = filename;

            res = execvp(REC, args);

            // ?? Exec failed
            fprintf(stderr,"%s:", REC);
            perror("execvp");
            exit(1);
            break;       

        // Parent
        default:
        	// Let's wait for the child to finish...
            pid = wait(&status);
            printf("Result: %d\n", res);
            break;
    }

    // Now let's play the result. Again, let's call a child to do this...
        switch(pid=fork()) {
        // Error
        case -1:
            perror("fork");
            exit(1);

        // Child
        case 0:
            ;
            // Exec program
            args[0] = PLAY;
            args[1] = filename;

            res = execvp(PLAY, args);

            // ?? Exec failed
            fprintf(stderr,"%s:", PLAY);
            perror("execvp");
            exit(1);
            break;       

        // Parent
        default:

        	// Let's wait for the child to finish...
            pid = wait(&status);
            printf("Result: %d\n", res);
            break;
    }

}