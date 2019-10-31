# dragonshell - README
## Introduction
This document outlines the design choices made and system calls used to implement dragonshell. The program was implemented and tested on the lab machines.


## Sources/Acknowledgments
**tokenize** function taken from starter code in the CMPUT 379 section of eClass.


## Overall Design
When the user enters a string input, we tokenize into possible multiple semicolon-separated commands, and then loop through each command. We fork on each command and execute the command as described below.


### Change Directory: cd
#### System calls used:
* chdir

#### Design:
User should input at the prompt "cd <path>". If command "cd" is detected, we call helper function changeDirectory and pass to it the supplied <path>. If no path was supplied, changeDirectory prints error message and we return to dragonshell prompt. If there was a path supplied, we do a chdir(path). If chdir returns a value of not zero, the path given could not be found, and we print an error message accordingly.

#### Testing:
Tested by cd to various paths, including full paths, "/", "..", and ".".

| Test Input        | Returned result |
|:------------------|:------          |
| cd                | dragonshell: expected argument to "cd" |
| cd /cshome/ahou   | pwd gives /cshome/ahou |
| cd ..             | pwd gives /cshome/ |


### Print Working Directory: pwd
#### System calls used:
* getcwd

#### Design:
We get the current working directory path using getcwd, and simply print it out.

#### Testing:
Tested by navigating around to different files and run pwd each time. Does not take any arguments in order to show present working directory.

| Test Input        | Returned result |
|:------------------|:------          |
| pwd               | /cshome/ahou/Documents/CMPUT379_A1 |
| cd ..; pwd        | /cshome/ahou/Documents |
| pwd /here         | /cshome/ahou/Documents |


### $PATH
#### System calls used:
None.

#### Design:
Program maintains a global variable containing the dragonshell $PATH variable. When the input string is detected as "$PATH", we print out the contents of the global variable.

#### Testing:
Tested by entering $PATH both before and after calling a2path.

| Test Input        | Returned result |
|:------------------|:------          |
| $PATH             | Current PATH: /bin/:/usr/bin/ |
| a2path $PATH:/cshome/; $PATH | Current PATH: /bin/:/usr/bin/:/cshome/ |


### a2path
#### System calls used:
* None

#### Design:
When shell receives command starting with a2path, it calls addToPath helper function and passes to it the user-supplied path to be added to $PATH. It then checks if this path is of format "$PATH:<new path>". If $PATH is not at the beginning, it overwrites $PATH variable with new path. Otherwise, it simply appends new path to the end of the existing $PATH variable.

#### Testing:
Show $PATH, then run a2path with or without $PATH in front of new path and check again.

| Test Input        | Returned result |
|:------------------|:------          |
| a2path $PATH:/cshome/ | $PATH returns "Current PATH: /bin/:/usr/bin/:/cshome/" |
| a2path /tmp           | $PATH returns "Current PATH: /tmp" |
| a2path                | $PATH returns "Current PATH: " |


### Execute External Programs
#### System calls used:
* execve

#### Design:
When user inputs something that isn't a basic command (i.e. cd, pwd, a2path, $PATH), it attempts to execute it as an external program. If the first argument user inputs to command line contains '/', we assume the full path to the program was given. If we cannot execute successfully, we print error message and return to prompt. If the first argument user inputted to command line did not contain '/', then we first attempt to execute the command from the current working directory. If that doesn't work, we try each to execute the command from each of the $PATH paths. If none of them result in success, we print error message and return to dragonshell prompt.

#### Testing:
Tested by running commands that are in $PATH, /bin and /bin/usr such as touch, which, ls. Also ran commands using their full path name. Ran ls with and without arguments/flags. All results returned as expected.

### Signal Handling
#### System calls used:
* signal
* kill

#### Design:
At the beginning of the dragonshell program, we use the signal wrapper function to associate signals SIGINT (Ctrl-C) and SIGTSTP (Ctrl-Z) with the signal handler function signalCallbackHandler. Upon catching these signals, the signalCallbackHandler function passes the signals down to any existing child/background processes.

#### Testing:
Pressed Ctrl-C while running the sleep program, and it exits that child process that is executing. Pressed Ctrl-Z while running sleep program for 10s in background and it stops it. Pressed Ctrl-C while running sleep program for 10s in background and it is interrupted before 10s is up. Pressing Ctrl-C and Ctrl-Z does not kill dragonshell both with and without background or child processes.

### Redirecting Output
#### System calls used:
* open
* dup2
* close

#### Design:
If the ">" character is present in a command, we will redirect the output to the filename specified to the right of the >. We use open to open the file for writing, and create it first if it doesn't exist. Then we use dup2 to copy stdout to the file descriptor of this file so that we can use fd and stdout interchangeably. This all takes place in the child process, and all other stdout from the child process will be written into the file. We raise a redirected output flag. If the flag is raised, we close the file descriptor at the end.

#### Testing:
Redirected output of both external and built-in commands to files.
dragonshell > ls > ls.out
dragonshell > cat ls.out
Makefile
README.md
dragonshell
dragonshell.c
dragonshell.o
ls.out
dragonshell > pwd > pwd.out
dragonshell > cat pwd.out
/cshome/ahou/Documents/CMPUT379_A1


### Piping
#### System calls used:
* pipe
* close
* dup2

#### Design:
If '|' character is detected, we know to pipe one process to another. To do this, we first fork. The parent process will read; it closes write end of the pipe. The child will write; it closes read end of pipe. Both parent and child continue on down as normal to execute their commands, while reading/writing from pipe.

#### Testing:
Pipe commands and ensured output was correct.
dragonshell > find ./ | sort
./Makefile
./README.md
./dragonshell
./dragonshell.c
./dragonshell.o
./ls.out

dragonshell > cat Makefile | wc
20      46     380


### Handling Multiple Commands in One Line
#### System calls used:
* fork
* \_exit

#### Design:
We tokenize at ";" character and receive array of the separate commands. We loop through each and fork for each command. The child then handles and executes the command.

#### Testing:
Entered several commands in one line. Tried different combinations, including ones that impact current working directory.

Examples:
**cd /cshome; ls** brings us to /cshome directory, and then shows what's inside /cshome

**a2path /bin; $PATH** overwrites $PATH variable and then displays "/bin"


### Background Processes
#### System calls used:
* fork
* setpgid
* close

#### Design:
If there is an ampersand (&) at the end of the command, we put it in the background by letting the child process execute the command. Stdout and stderr get closed for the child to suppress any output it gives. Meanwhile, the parent continues on and assigns the group pid of the child background process to the parent pid. We also update the global background pid variable to equal the newly created pid. Then the parent goes back to the shell prompt. When child finishes executing the command, it returns silently.

#### Testing:
Put a sleep 50s process into the background with **sleep 50s &**. Command **ps** shows the newly created process, and it remains for 50s before completing execution and disappears from the list of pids. Also successfully tested putting built-in commands in the background.


### Exiting
#### System calls used:
* kill
* \_exit

#### Design:
Checks for background process and kills that using kill(bgpid, SIGKILL).
Checks if child process, and exits from that.
Prints farewell message. Finally, it exits the main dragonshell program and returns to bash.

#### Testing:
Ran background process, then exited.
Ran child processes and background process, then exited.
Simply exited on start of program.
Started another dragonshell from dragonshell, and exit in the sub-dragonshell only closes sub-dragonshell; used exit again to exit original dragonshell.
