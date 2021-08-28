#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

// Standard Definitions
#define BUF_SIZE 4096
#define COLOR "\033[0;33m"                         // fancy yellow color for user interface 
#define STD_COL "\033[0m"                          // standard color for user interface            
#define TMP_DIR "/var/tmp/project-csap/client"     //the temporary directory in which the files will be saved
#define REC "/usr/bin/rec"                         //where rec is located
#define PLAY "/usr/bin/play"                       //where play is located

// DEBUG STATEMENT
#ifdef DEBUG
  #define DEBUGPRINT(a) printf a
#else
  #define DEBUGPRINT(a) (void)0
#endif

// Shared imports
#include "../shared/structs.h"
#include "../shared/utils.c"
#include "../shared/list.c"
#include "../shared/user.c"
#include "../shared/message.c"
// #include "../shared/connection.c"

// Local imports
#include "signin.c"
#include "connection.c"
#include "readConfig.c"
#include "handleServerReplies.c"
#include "messages.c"
#include "interaction.c"
#include "dowork.c"

