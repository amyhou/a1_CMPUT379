# dragonshell - README
## Introduction
This document outlines the design choices made and system calls used to implement dragonshell.

## Sources/Acknowledgments
**tokenize** function taken from starter code in the CMPUT 379 section of eClass.

## Overall Design

### Change Directory: cd
#### System calls used:
* chdir

#### Design:
User should input at the prompt "cd <path>". If command "cd" is detected, we call helper function changeDirectory and pass to it the supplied <path>. If no path was supplied, changeDirectory prints error message and we return to dragonshell prompt. If there was a path supplied, we do a chdir(path). If chdir returns a value of not zero, the path given could not be found, and we print an error message accordingly.

#### Testing:

### Print Working Directory: pwd
#### System calls used:
* getcwd

#### Design:
We get the current working directory path using getcwd, and simply print it out.

#### Testing:

### $PATH
#### System calls used:
None.

#### Design:
Program maintains a global variable containing the dragonshell $PATH variable. When the input string is detected as "$PATH", we print out the contents of the global variable.

#### Testing:

### a2path
#### System calls used:
#### Design:
#### Testing:

### Execute commands
#### System calls used:
#### Design:
#### Testing:

### Signal Handling
#### System calls used:
#### Design:
#### Testing:

### Redirecting Output
#### System calls used:
#### Design:
#### Testing:

### Piping
#### System calls used:
#### Design:
#### Testing:

### Handling Multiple Commands in One Line
#### System calls used:
#### Design:
#### Testing:

### Exiting
#### System calls used:
#### Design:
#### Testing:
