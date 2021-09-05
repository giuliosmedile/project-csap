// Function that handle consensus between datarepos

/**
 * Function that checks all available datarepos and returns the one that's been chosen by the consensus algorithm
 * In an ideal world, the leader would be chosen by comparing the network's stability or the latency, but I just choose the first available one.
 * 
 * @param repos Array of datarepos socket descriptors
 * @param repos_count Number of datarepos
 * @param status Array of datarepos status, 0 if available, 1 if unavailable
 * @return The chosen datarepo socket descriptor to be the leader
**/
int chooseNewLeader(int *repos, int* status, int repos_count) {
    for (int i = 0; i < repos_count; i++) {
        if (status[i] == 1) {
            return repos[i];
        }
    }
    return -1;
}

/**
 * Function that flags a datarepo as alive.
 * @param alive the datarepo's socket descriptor which is alive
 * @param repos Array of datarepos socket descriptors
 * @param repos_count Number of datarepos
 * @param status Array of datarepos status, 0 if available, 1 if unavailable
**/
int* markRepoAlive(int alive, int *repos, int repos_count, int* status) {
    // First get the index of the alive datarepo
    int alive_index = -1;
    for (int i = 0; i < repos_count; i++) {
        if (repos[i] == alive) {
            alive_index = i;
            break;
        }
    }

    // Mark the datarepo as alive
    status[alive_index] = 1;

    DEBUGPRINT(("seeing all statuses\n"));
    for(int i = 0; i < repos_count; i++) {
        DEBUGPRINT(("%d\t%d\n", repos[i], status[i]));
    }

    return status;
}

/**
 * Function that flags a datarepo as dead.
 * @param dead the datarepo's socket descriptor which is dead
 * @param repos Array of datarepos socket descriptors
 * @param repos_count Number of datarepos
 * @param status Array of datarepos status, 0 if available, 1 if unavailable
 * @returns status
**/
int* markRepoDead(int dead, int *repos, int repos_count, int* status) {
    // First get the index of the dead datarepo
    int dead_index = -1;
    for (int i = 0; i < repos_count; i++) {
        if (repos[i] == dead) {
            dead_index = i;
            break;
        }
    }

    // Mark the datarepo as dead
    status[dead_index] = 0;

    DEBUGPRINT(("seeing all statuses\n"));
    for(int i = 0; i < repos_count; i++) {
        DEBUGPRINT(("%d\t%d\n", repos[i], status[i]));
    }

    return status;
}

/**
 * Function that tells if all repos are dead.
 * @param status the array of statuses of the datarepos
 * @param repos_count Number of datarepos
 * @return 1 if all datarepos are dead, 0 otherwise
**/
int allReposDead(int* status, int repos_count) {
    int result = 1;
    for (int i = 0; i < repos_count; i++) {
        if (status[i] == 1) {
            result = 0;
            break;
        }
    }
    return result;
}


/**
 * Function to get modified files from the leader and send them to the client
 * @param leader the leader datarepo
 * @param datarepos the array of datarepos
 * @param count the number of datarepos 
**/
void sendModifiedFiles(int leader, int* datarepos, int count) {

    char* rcvString = malloc(sizeof(char) * BUF_SIZE);

    // Get the modified files from the leader
    readFromSocket(leader, rcvString);

    DEBUGPRINT(("Received from socket before sending to slaves: %s\n", rcvString));

    // Send the result back to the datarepos
    for (int i = 0; i < count; i++) {
        if (datarepos[i] != leader) {
            sendToSocket(datarepos[i], rcvString);
        }
    }

    return;

}
