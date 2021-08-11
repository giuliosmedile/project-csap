# project-csap
Project for A.Y. 2020/2021 for the course of Computer Systems and Programming @ Sapienza held by Prof. Giorgio Richelli.
All code is my own.


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

Commands that will be eventually implemented are:
- listen [listens to an audio message]
- delete [deletes a received message]

# Misc
Tools used while creating this project:
- **Visual Studio Code** as the IDE
- **Nemiver**, a visual interface for gdb. Saved me a lot while debugging.
- **GitHub** (duh) that helped me recover my work in case of some disastrous modifications
- **StackOverflow** for teaching how to fix my stupid mistakes
