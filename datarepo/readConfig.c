// FUNCTIONS TO CORRECTLY READ THE CONFIGURATION FILE

#define CONFIG "datarepo.conf"		//name of the configuration file
#define COMMENT '#'					//character that defines a comment line
#define MAX_ARGS 2					//every line is structured as lhs=rhs, so the tokenize array only needs 2 fields
#define BUF_SIZE 256				//max buffer size

void readConfig (short unsigned int* serv_port) {
	int tmp = *serv_port;
	char* buf = (char*)malloc(BUF_SIZE * sizeof(char));
	char** args = (char**)malloc(MAX_ARGS);
	size_t len;
	FILE* fp;

	// Open and check if the file exists
	if ((fp = fopen(CONFIG, "r")) == NULL) return;

	// Read the conf file line per line
	while (getline(&buf, &len, fp) != -1) {
		buf[strlen(buf)-1] = '\0';
		printf("[-] Reading line. Content: \"%s\"\n", buf);
		//If the first line is a comment, skip it
		if (buf[0] == '#') continue;

		//Tokenize the line
		tokenize(buf, &args);
		printf("[-] tokenized: \"%s\", \"%s\"\n", args[0], args[1]);

		//Look at args[0] to see the left hand side of the assignment
		if (!strcmp(args[0], "port")) {
			printf("%s\n", args[1]);
			*serv_port = atoi(args[1]);
		}
	}
	fclose(fp);
	return;
}