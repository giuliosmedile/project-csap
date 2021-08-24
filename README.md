# project-csap
Project for A.Y. 2020/2021 for the course of Computer Systems and Programming @ Sapienza held by Prof. Giorgio Richelli.
All code is my own, unless stated otherwise.
Tested on Ubuntu 20.04 LTS, Ubuntu 21.04, Linux Mint Cinnamon 21, macOS 11 (even if not fully functional as it lacks rec and play)


# Installation
Use `make all` to compile all programs.
The executables will be found as 	`main` in the directories `client`, `server` and `datarepo`.

Alternatively, each program can be made indipendently, by calling its name.


# Usage
By default, `server` will run on localhost on port 16000, `datarepo` on 16001.

Client will attempt to connect to those ports and addresses.
These properties can be changed by modifying the `.config` file of each program.

The client will work as a CLI program.
Currently implemented commands are:
- login
- logout
- info [shows currently logged in user's information]
- add [adds another user to one's addressbook]
- record [record audio message]
- listen [listens to an audio message]

Commands that will be eventually implemented are:
- delete [deletes a received message]

# TODO
- Better user experience:
	- ability to listen to own messages
	- ability to delete messages
	- ability to forward message to another user
- 1-1-n communication, with multiple datarepos listening to the server
- Error correction and extensive testing
- Test on actual network, not on my local computer

# Misc
Tools used while creating this project:
- **Visual Studio Code** as the IDE
- **Nemiver**, a visual interface for gdb. Saved me a lot while debugging.
- **GitHub** (duh) that helped me recover my work in case of some disastrous modifications
- **StackOverflow** for teaching how to fix my stupid mistakes
