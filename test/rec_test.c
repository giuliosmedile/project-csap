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

#define true 1
#define false 0

/**
 * Function to check if this machine can run a certain command
 * @cmd: the command i am checking
 * @returns true, if current user can run the command, false otherwise
 * Shamelessly stolen from https://stackoverflow.com/questions/41230547/check-if-program-is-installed-in-c
*/
int can_run_command(const char *cmd) {
    if(strchr(cmd, '/')) {
        // if cmd includes a slash, no path search must be performed,
        // go straight to checking if it's executable
        return access(cmd, X_OK)==0;
    }
    const char *path = getenv("PATH");
    if(!path) return false; // something is horribly wrong...
    // we are sure we won't need a buffer any longer
    char *buf = malloc(strlen(path)+strlen(cmd)+3);
    if(!buf) return false; // actually useless, see comment
    // loop as long as we have stuff to examine in path
    for(; *path; ++path) {
        // start from the beginning of the buffer
        char *p = buf;
        // copy in buf the current path element
        for(; *path && *path!=':'; ++path,++p) {
            *p = *path;
        }
        // empty path entries are treated like "."
        if(p==buf) *p++='.';
        // slash and command name
        if(p[-1]!='/') *p++='/';
        strcpy(p, cmd);
        // check if we can execute it
        if(access(buf, X_OK)==0) {
            free(buf);
            return true;
        }
        // quit at last cycle
        if(!*path) break;
    }
    // not found
    free(buf);
    return false;
}

void main(int argc, char** argv) {
    char *args[3];
    int pid, status;
    int res;
    char* filename = (char*)malloc(256 * sizeof(char));
    char* input = (char*)malloc(256 * sizeof(char));

    if (!can_run_command("rec") || !can_run_command("play")) {
        printf("Be sure to have installed rec and play through\n\tsudo apt install sox\n");
        exit(2);
    }

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