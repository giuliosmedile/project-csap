# project-csap
Project for A.Y. 2020/2021 for the course of Computer Systems and Programming @ Sapienza held by Prof. Giorgio Richelli.
All code is my own, unless stated otherwise.

Tested on Ubuntu 20.04 LTS, Ubuntu 21.04, Linux Mint Cinnamon 20.2, macOS 11 (even if not fully functional as it lacks rec and play)


# Installation
Use `make all` to compile all programs.
The executables will be found as 	`main` in the directories `client`, `server` and `datarepo`.

Alternatively, each program can be made indipendently, by calling its name.

All programs use some sort of debug to show whoever runs them what is going on behind the hood. However, for a _smoother_ user experience, `client` has them disabled by default.
Calling `make client ARGS="-D=DEBUG"` explicitely shows debug prints.

Albeit the "debug mode" is defined in all programs, and calling `make all ARGS="-D=DEBUG"` won't raise any errors, currently both `server` and `datarepo` programs do not make use of any debug print.


# Usage
By default, `server` will run on localhost on port 16000, `datarepo` on 16001.

Client will attempt to connect to those ports and addresses.
These properties can be changed by modifying the `.config` file of each program.

The machine running `client` will need to have installed `sox` as it is needed for both the `record` and `play` commands. 
If the machine does not have `sox`, the user will be prompted to install it via apt.

The client will work as a CLI program.
Currently implemented commands are:
- login
- logout
- info [shows currently logged in user's information]
- add [adds another user to one's addressbook]
- record [record audio message]
- listen [listens to an audio message]
- delete [deletes a received message]

Commands that will be eventually implemented are:
- search [searches for message in a given time range]
- remove [removes user from one's addressbook]

# TODO
- Better user experience:
	- ability to listen to own messages
- 1-1-n communication, with multiple datarepos listening to the server
- Error correction and extensive testing
- Test on actual network, not on my local computer
- Ability to pipe stdout to a file, for easy logging

# Misc
Tools used while creating this project:
- **Visual Studio Code** as the IDE
- **Nemiver**, a visual interface for `gdb`. Saved me a lot while debugging.
- **GitHub** (duh) that helped me recover my work in case of some disastrous modifications
- **StackOverflow** for teaching how to fix my stupid mistakes
- **Check**, a library for unit testing
