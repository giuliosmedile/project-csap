#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// #include "user.c"
#include "../client/client.h"	//for tokenize

void main(int argc, char** argv) {

	printf("{} test - create a new user\n");
	// Create a new user
	t_user* u = createUser("Giulio");

	printf("{} test - print it out\n");
	// Print it out
	printUser(u, "test.txt");

	printf("{} test - create another user\n");
	// Create another user
	t_user* u1 = createUser("Andrea");
	printUser(u1, "test.txt");


	printf("{} test - add users to the addressbook\n");
	// Add it to the addressbook of the first user
	addUserToAddressBook(u, "Andrea");
	addUserToAddressBook(u, "Roberto");
	addUserToAddressBook(u, "Giancarlo");
	printUser(u, "test.txt");

	printf("{} test - search for a user\n");
	// Search for a user
	t_user* s_u = searchUser("Giulio", "test.txt");
	printUser(s_u, "searchTest.txt");

	/*

	After these tests, the file test.txt should appear as

Andrea;0;0;
Giulio;0;1;Andrea;
	
	and the file seachTest.txt should appear as

Giulio;0;1;Andrea;

	*/

}