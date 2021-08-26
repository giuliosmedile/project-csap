/* mktime example: weekday calculator */
#include <stdio.h>      /* printf, scanf */
#include <time.h>       /* time_t, struct tm, time, mktime */

#define BUF_SIZE 1024
#define COLOR ""
#define STD_COL ""

#include "../shared/utils.c"
#include "../shared/structs.h"  /* project structs */
#include "../shared/list.c"
#include "../shared/user.c"
#include "../shared/message.c"
#include "../client/interaction.c" /* for getTime() */
/**
 * The purpose of this test is to check if the function searchForMessagesInRange actually returns the correct messages
 * If the test is correct, the test should return the messages as shown in the comments
**/



int main(int argc, char *argv[]) {

  char* username = "giulio";
  char* repository = "./demoMessagesRepo.txt";

  time_t lower =  644191200; // June 1, 1990
  time_t higher = 675727200; // June 1, 1991

  // Go into the messages repo and get all the messages that have "username" as receiver, and fall between the two dates
  puts("before testlist");
  NODE* testList = searchForMessagesInRange(username, lower, higher, repository);
  puts("after testlist");
  // Print the messages
  // If the test is correct, the list should be (in any particular order):
  // Message 1: lollo;giulio;662684400;1;lollo-giulio-1991:1:1:0:0:0.wav;
  printf("%s\n", print_list(testList, ""));

  return 0;
}