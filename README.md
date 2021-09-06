# project-csap
Project for A.Y. 2020/2021 for the course of Computer Systems and Programming @ Sapienza held by Prof. Giorgio Richelli.
All code is my own, unless stated otherwise.

Tested on Ubuntu 20.04 LTS, Ubuntu 21.04, Linux Mint Cinnamon 20.2, macOS 11 (even if not fully functional as it lacks rec and play)


# Installation
Use `make all` to compile all programs. (if `make all` does not work, try `make all -B` instead)

The executables will be found as `main` in the directories `client`, `server` and `datarepo`.

Alternatively, each program can be made indipendently, by calling its name.

All programs use some sort of debug to show whoever runs them what is going on behind the hood. However, for a _smoother_ user experience, `client` has them disabled by default.
Calling `make client ARGS="-D=DEBUG"` explicitely shows debug prints.

All programs make use of those debug prints. A message showing `DEBUG MODE` will be the first thing printed by the programs if the debug prints are active.

Of course, calling the `ARGS` flag on `make` allows to set all kinds of compiler variables.


# Usage
By default, `server` will run on localhost on port 16000, `datarepo` on 16001.

Client will attempt to connect to those ports and addresses.
These properties can be changed by modifying the `.config` file of each program.

It is possible, however, to change `datarepo`'s port by calling the option `-p` when running the program, for example `datarepo -p 15151`.

The machine running `client` will need to have installed `sox` as it is needed for both the `record` and `play` commands. 
If the machine does not have `sox`, the user will be prompted to install it via apt.

The client will work as a CLI program.
Currently implemented commands are:
- login
- logout
- info [shows currently logged in user's information]
- add [adds another user to one's addressbook]
- remove [removes user from one's addressbook]
- record [record audio message]
- listen [listens to an audio message]
- delete [deletes a received message]
- search [searches for message in a given time range]
- 
# Misc
Tools used while creating this project:
- **Visual Studio Code** as the IDE
- **Nemiver**, a visual interface for `gdb`. Saved me a lot while debugging.
- **Valgrind**, a tool to analyze memory leaks and fix overflow errors (I had a lot o'those)
- **GitHub** (duh) that helped me recover my work in case of some disastrous modifications
- **StackOverflow** for teaching how to fix my stupid mistakes
