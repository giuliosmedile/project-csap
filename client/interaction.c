/// GENERAL FUNCTION THAT HANDLES GENERAL USER INTERACTION

char* takeUserInput(char* input, t_user* u) {
    printf("%s", COLOR);

    if (u == NULL) {
        printf("[]: ");
    } else {
        printf("[%s]: ", u->username);
    }
    if (fgets(input,BUF_SIZE,stdin) == NULL) {
        printf("\n");
        exit(0);
    }
    printf("%s", STD_COL);

    return input;
}

/**
 * Function that handles user input regarding dates. 
 * Any numerical input will be converted to a date.
 * @returns the date selected by the user from epoch time.
**/
time_t getDate() {
    time_t rawtime;
    struct tm * timeinfo;
    int year, month ,day;

userInput:
    /* prompt user for date */
    printf ("Enter year: "); fflush(stdout); scanf ("%d",&year);
    printf ("Enter month: "); fflush(stdout); scanf ("%d",&month);
    printf ("Enter day: "); fflush(stdout); scanf ("%d",&day);

    // get current timeinfo and modify it to the user's choice
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    timeinfo->tm_year = year - 1900;
    timeinfo->tm_mon = month - 1;
    timeinfo->tm_mday = day;
    timeinfo->tm_hour = 0;
    timeinfo->tm_min = 0;
    timeinfo->tm_sec = 0;

    // Check for errors and make sure the date is valid
    if (mktime(timeinfo) == -1 || year > 2038 || year < 1970 || month > 12 || month < 1 || day > 31 || day < 1) {
        printf ("\nInvalid date!\n");
        goto userInput;
    }
    rawtime = mktime ( timeinfo );

    return rawtime;
}
