// FUNCTIONS TO HANDLE MESSAGES AND ADDRESSBOOK

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define TMP_DIR "/var/tmp/project-csap"     //the temporary directory in which the files will be saved
#define REC "/usr/bin/rec"                  //where rec is located
#define PLAY "/usr/bin/play"                //where play is located

#define true 1
#define false 0

void interceptSigInt(int signum) {
	printf("This does absoluely nothing.\n");
}

/**
 * Function to check if this machine can run a certain command 
 * Shamelessly stolen from https://stackoverflow.com/questions/41230547/check-if-program-is-installed-in-c
 * @cmd: the command i am checking
 * @returns true, if current user can run the command, false otherwise
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

char* add(char* result, t_user** u_p) {
    // the new user i am trying to add
    char* other = (char*)malloc(BUF_SIZE * sizeof(char));
    

    printf("%s", COLOR);

    // Check if i am logged in
    if (*u_p == NULL) {
        printf("[!] Before adding a user to your addressbook, perhaps you should try logging in first.\n");
        return "null";
    }

    // Get user input
    printf("Username of the user you want to add: ");
	if (fgets(other,sizeof(other),stdin) == NULL) {
        return "null";
    }
    other[strlen(other)-1] = '\0';
    sprintf(result, "add %s %s", (*u_p)->username, other);
    return result;

}

char* record(char* result, t_user** u_p) {
    char* file = (char*)malloc(BUF_SIZE * sizeof(char));        // The file path in which the temp recording will be
    char* other = (char*)malloc(BUF_SIZE * sizeof(char));       // user who will receive the file
    char *args[3];                                              // Arguments for rec command
    int pid, status;
    int res;
    // to exit rec, a SIGINT must be sent, so we intercept it and do nothing, or else the program will exit
    signal(SIGINT, interceptSigInt);

    printf("%s", COLOR);

    // Check if i am logged in
    if (*u_p == NULL) {
        printf("[!] Before adding a user to your addressbook, perhaps you should try logging in first.\n");
        return "null";
    }

    // Check if i can run rec
    if (!can_run_command("rec")) {
        printf("[!] Be sure to have installed rec and play through\n\tsudo apt install sox\n");
        return "null";
    }

    // Create the temp directory, it if is not there
    struct stat st = {0};
    if (stat(TMP_DIR, &st) == -1) {
        mkdir(TMP_DIR, 0755);
    }

    // Take input
    printf("Username of the user you want to send a message to: ");
	if (fgets(other,sizeof(other),stdin) == NULL) {
        return "null";
    }
    other[strlen(other)-1] ='\0';
    
    time_t rawtime = (time_t)malloc(sizeof(time_t));
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    sprintf(file, "%s/%s-%d:%d:%d.wav", TMP_DIR, other, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

    // Fork child to record audio message
    switch(pid=fork()) {
        // Error
        case -1:
            perror("fork");
            exit(1);

        // Child
        case 0:
            ;
            // Exec program
            args[0] = REC;
            args[1] = file;
            args[2] = NULL;     // must be here or else execvp won't work...

            if (res = execvp(REC, args) < 0) {
                // ?? Exec failed
                fprintf(stderr,"%s:", REC);
                perror("execvp");
                exit(1);
            } else break;     

        // Parent
        default:
        	// Let's wait for the child to finish...
            pid = wait(&status);printf("[-] Succesfully recorded audio at\n\t%s\n", file);
            printf("%s", STD_COL);

            return "null";
    }

    printf("[-] Succesfully recorded audio at\n\t%s\n", file);
    printf("%s", STD_COL);

    return "null";   //TEMP!!!!!!
    // TODO return file;
}