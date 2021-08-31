// FUNCTIONS TO CORRECTLY READ THE CONFIGURATION FILE

#define CONFIG "server.conf"		//name of the configuration file
#define COMMENT '#'					//character that defines a comment line
#define MAX_ARGS 2					//every line is structured as lhs=rhs, so the tokenize array only needs 2 fields

// The config file, specified in the CONFIG variable, must be structured 
// in the same order as the variables appear in the function. Comments
// are allowed by putting a character (by default, #, can be changed)
// that will tell the function to skip the line.
void readConfig (int *port, int *clients, int *mds, char ***mds_addr, int **mds_ports) {

	char* buf = (char*)malloc(BUF_SIZE * sizeof(char));
	char** args = (char**)malloc(MAX_ARGS);
	*mds_ports = (int*)malloc(MAX_DATAREPO * sizeof(int));
	size_t len;
	int mds_counter = 0;		// counter needed to store each mds variable
	int port_counter = 0;		// counter needed to store each port variable
	FILE* fp;

	// Open and check if the file exists
	if ((fp = fopen(CONFIG, "r")) == NULL) return;

	// Read the conf file line per line
	while (getline(&buf, &len, fp) != -1) {
		if (buf[strlen(buf) - 1] == '\n') buf[strlen(buf) - 1] = '\0';
		//If the first line is a comment, skip it
		if (buf[0] == '#') continue;

		//Tokenize the line
		tokenize(buf, &args);

		//Look at args[0] to see the left hand side of the assignment
		if (!strcmp(args[0], "port")) {
			*port = atoi(args[1]);
			continue;
		} 
		if (!strcmp(args[0], "clients")) {
			*clients = atoi(args[1]);
			continue;
		}
		if (!strcmp(args[0], "mds")) {
			*mds = atoi(args[1]);
			*mds_addr = (char**)malloc(*mds * BUF_SIZE * sizeof(char*));
			continue;
		}
		// I do this to store each mds address
		if (!strncmp(args[0], "mds[", strlen("mds["))) {
		    (*mds_addr)[mds_counter] = (char*)malloc(BUF_SIZE * sizeof(char));
			strcpy((*mds_addr)[mds_counter], args[1]);
			mds_counter++;
		}

		// And this to store each mds port
		if (!strncmp(args[0], "port[", strlen("port["))) {
		    (*mds_ports)[port_counter] = atoi(args[1]);
			port_counter++;
		}

	}
	fclose(fp);
	return;
}