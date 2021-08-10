// FUNCTIONS TO HANDLE MESSAGES AND ADDRESSBOOK

// #include <stdlib.h>
// #include <time.h>
// #include <stdio.h>
// #include <unistd.h>
// #include <string.h>
// #include <sys/types.h>
// #include <sys/wait.h>
// #include <sys/stat.h>
// #include <malloc.h>

#define TMP_DIR "/var/tmp/project-csap/client"     //the temporary directory in which the files will be saved
#define REC "/usr/bin/rec"                         //where rec is located
#define PLAY "/usr/bin/play"                       //where play is located

#define true 1
#define false 0

void interceptSigInt(int signum) {
	printf("\n");
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

/* Function to add a user to the currently logged in user's addressbook
 * @param result the string that will be sent to the server 
 * @param u_p pointer to the current user
*/
char* add(char* result, t_user** u_p) {
    // the new user i am trying to add
    char* other = (char*)malloc(BUF_SIZE * sizeof(char));
    

    printf("%s", COLOR);

    // Check if i am logged in
    if (*u_p == NULL) {
        printf("[!] Before adding a user to your addressbook, perhaps you should try logging in first.\n");
        return "null";
    }

    printf("Username of the user you want to add: ");
	if (fgets(other,BUF_SIZE,stdin) == NULL) {
        return "null";
    }
    other[strlen(other)-1] ='\0';
    sprintf(result, "add %s %s", (*u_p)->username, other);
    return result;

}

/** 
 * Client function for message recording
 * @param result the string that will be sent to the server
 * @param u_p pointer to the current user
 * @param file the filename of the file that will be recorded
*/
char* record(char* result, t_user** u_p, char* file) {

    char* path = (char*)malloc(BUF_SIZE * sizeof(char));        // The file path in which the temp recording will be
    char* other = (char*)malloc(BUF_SIZE * sizeof(char));       // user who will receive the file
    char *args[3];                                              // Arguments for rec command
    int pid, status;
    int res;

    // to exit rec, a SIGINT must be sent, so we intercept it and do nothing, or else the program will exit
    signal(SIGINT, interceptSigInt);

    printf("%s", COLOR);
    // Check if i am logged in
    if (*u_p == NULL) {
        printf("[!] Before recording a message, perhaps you should try logging in first.\n");
        sprintf(result, "null");
        return "null";
    }
    // Check if i can run rec
    if (!can_run_command("rec")) {
        printf("[!] Be sure to have installed rec and play through\n\tsudo apt install sox\n");
        sprintf(result, "null");
        return "null";
    }
    // Create the temp directory, it if is not there
    struct stat st = {0};
    if (stat(TMP_DIR, &st) == -1) {
        mkdir(TMP_DIR, 0755);
    }

    // Take input
    selectUser(*u_p, other);
    

    time_t rawtime;
    time (&rawtime);
    struct tm* timeinfo = (struct tm*)malloc(sizeof(struct tm));
    timeinfo = localtime (&rawtime);
    
    // The file name is set as follows:
    // sender_receiver-YYYY-MM-DD-HH:MM:SS.wav
    sprintf(file, "%s-%s-%d:%d:%d:%d:%d:%d.wav", (*u_p)->username, other, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    sprintf(path, "%s/%s", TMP_DIR, file);
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
            args[1] = path;
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
            pid = wait(&status);printf("[-] Recorded audio at\n\t%s\n", path);
            printf("%s", STD_COL);

            // Data is then formatted to be sent to the server as
            // record [filename]
            sprintf(result, "record %s %d", file, get_file_size(path));
            return result;
    }

    // This won't be reached    
}