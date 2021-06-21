// FUNCTIONS TO CORRECTLY READ THE CONFIGURATION FILE

#define CONFIG "client.conf"		//name of the configuration file
#define COMMENT '#'					//character that defines a comment line
#define MAX_ARGS 2					//every line is structured as lhs=rhs, so the tokenize array only needs 2 fields

void readConfig (char** serv_add, unsigned short* serv_port) {
	char* tmp = *serv_add;
	char* buf = (char*)malloc(BUF_SIZE * sizeof(char));
	char** args = (char**)malloc(MAX_ARGS * sizeof(char*));
	size_t len;
	FILE* fp;

	// Open and check if the file exists
	if ((fp = fopen(CONFIG, "r")) == NULL) return;
	// Read the conf file line per line
	while (getline(&buf, &len, fp) != -1) {
		buf[strlen(buf)-1] = '\0';
		//If the first line is a comment, skip it
		if (buf[0] == '#') continue;

		//Tokenize the line
		tokenize(buf, &args);

		//Look at args[0] to see the left hand side of the assignment
		if (!strcmp(args[0], "serveraddress")) {
			strcpy(*serv_add, args[1]);
		}
		if (!strcmp(args[0], "port")) {
			*serv_port = atoi(args[1]);
		}
	}
	fclose(fp);
	return;
}