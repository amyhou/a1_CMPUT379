/***
 *    ________                                       _________.__           .__  .__
 *    \______ \____________     ____   ____   ____  /   ____/|  |__   ____ |  | |  |
 *     |    |  \_  __ \__  \   / ___\ /  _ \ /    \ \____  \ |  |  \_/ __ \|  | |  |
 *     |    `   \  | \// __ \_/ /_/  >  <_> )   |  \/       \|   Y  \  ___/|  |_|  |__
 *    /_______  /__|  (____  /\___  / \____/|___|  /_____  /|___|  /\___  >____/____/
 *            \/           \//_____/             \/      \/      \/     \/
 *                                  Programmed by Amy Hou
 */

#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

/* Questions:
  1. Do we need to fflush after each printf statement?
*/

/*
  1. Send process to BG.
  2. Handling signals
  3. Exit needs to kill all running processes.
*/

/* ------------------------------ DEFINE MACROS ---------------------------- */
#define TRUE  (1)
#define FALSE (0)
/* --------------------------- END OF DEFINED MACROS ----------------------- */

/* ----------------------------- GLOBAL VARIABLES -------------------------- */
char shellPath[PATH_MAX] = "/bin/:/usr/bin/"; //default path
/* ------------------------- END OF GLOBAL VARIABLES ----------------------- */

/* -------------------------- FUNCTIONS DEFINITIONS ------------------------ */
void welcomeMsg() {
  /* Print welcome message (make fancier later) */
  printf("\n");
  printf("--------------------------------------------------------------------------------\n\n");
  printf("________                                       ________.__           .__  .__\n");
  printf("\\______ \\____________     ____   ____   ____  /   ____/|  |__   ____ |  | |  |\n");
  printf(" |    |  \\_  __ \\__  \\   / ___\\ /  _ \\ /    \\ \\____  \\ |  |  \\_/ __ \\|  | |  |\n");
  printf(" |    `   \\  | \\// __ \\_/ /_/  >  <_> )   |  \\/       \\|   Y  \\  ___/|  |_|  |__\n");
  printf("/_______  /__|  (____  /\\___  / \\____/|___|  /______  /|___|  /\\___  >____/____/\n");
  printf("        \\/           \\//_____/             \\/       \\/      \\/     \\/\n");
  printf("                               Programmed by Amy Hou\n\n");
  printf("--------------------------------------------------------------------------------\n\n");
}

void tokenize(char* str, const char* delim, char ** argv) {
  /**
   * @brief Tokenize a C string
   *
   * @param str - The C string to tokenize
   * @param delim - The C string containing delimiter character(s)
   * @param argv - A char* array that will contain the tokenized strings
   * Make sure that you allocate enough space for the array.
   */
  char* token;
  token = strtok(str, delim);
  for(size_t i = 0; token != NULL; ++i){
    argv[i] = token;
    token = strtok(NULL, delim);
  }
}

void removeQuotes(char * str) {
  int i, j;
  int len = strlen(str);
  for(i = 0; i < len; i++)
  {
    if((str[i] == '"') || (str[i] == '\''))
    {
        for(j=i; j<len; j++)
        {
            str[j] = str[j+1];
        }
        len--;
        // Don't increment i if char was removed
        i--;
    }
  }
}

void changeDirectory(char * dirPath) {
  /*  Changed directory to the one specified by input path.
  *   Input param:
  *     dirPath - path to directory relative to current working directory
  *   Returns:
  *     Nothing
  */
  int rc;
  if (dirPath == NULL)
  {
    printf("dragonshell: expected argument to \"cd\"\n");
    return;
  }
  if ((rc = chdir(dirPath)) != 0)
  {
    printf("dragonshell: No such file or directory.\n");
    fflush(stdout);
  }
}

void printWorkingDirectory() {
  char path[PATH_MAX];
  printf("%s\n", getcwd(path, PATH_MAX));
  fflush(stdout);
}

void showPath() {
  /* Prints out $PATH */
  printf("Current PATH: %s\n", shellPath);
}

void addToPath(char * path) {
  /* Adds path param to $PATH variable */
  int i = 0;
  char * newPaths[PATH_MAX] = {NULL};

  if (path == NULL) // set path to empty string
  {
    memset(&shellPath[0], 0, sizeof(shellPath));
    return;
  }
  tokenize(&path[0], ":", &newPaths[0]);
  if (strcmp(newPaths[0], "$PATH") != 0)
  {
    // printf("Clearing $PATH var\n");
    memset(&shellPath[0], 0, sizeof(shellPath));
  }
  else
  {
    if (strlen(shellPath) > 0) // if path is not empty str, append preparatory ':'
    {
      strcat(shellPath, ":");
    }
    i++;
  }
  while (newPaths[i] != NULL)
  {
    strcat(shellPath, newPaths[i]);
    strcat(shellPath, ":");
    // printf("new shellPath = %s\n", shellPath);
    i++;
  }
  // Get rid of trailing ":"
  size_t len = strlen(shellPath);
  if (len > 0 && shellPath[len - 1] == ':') {
    shellPath[len - 1] = '\0';
  }
}

int executeCmd(char ** cmdArgs) {
  /*Try executing command using any given command line arguments. Look for
  * program in any of the $PATH directories.
  * Input:
  *   cmdArgs:
  *     cmdArgs[0]: program name
  *     cmdArgs[1], ... : program arguments
  * Returns:
  *   rc - return code is 0 for success, -1 for failure to execute program
  */
  char ** argv1 = cmdArgs;
  char * envp1[] = {NULL}; // change this!
  // printf("cmdArgs[1]: %s\n", cmdArgs[1]);
  fflush(stdout);
  int rc = execve(cmdArgs[0], argv1, envp1);
  if (rc == -1)
  {
    char * paths[PATH_MAX] = {NULL};
    tokenize(&shellPath[0], ":", &paths[0]);
    int j = 0;
    while (paths[j] != NULL)
    {
      // printf("Try to execute from paths[j]: %s\n", paths[j]);
      char tmp[PATH_MAX];
      strcpy(tmp, paths[j]);
      strcat(tmp, "/");
      rc = execve(strcat(tmp, cmdArgs[0]), argv1, envp1);
      j++;
    }
  }
  return rc;
}

int basicCmds(char ** cmdArgs) {
  /* check and execute if basic built-in command */
  if (strcmp(cmdArgs[0], "cd") == 0)
  {
    if (cmdArgs[1] != NULL)
    {
      changeDirectory(cmdArgs[1]);
    }
    return 0;
  }
  else if (strcmp(cmdArgs[0], "pwd") == 0)
  {
    printWorkingDirectory();
    return 0;
  }
  else if (strcmp(cmdArgs[0], "$PATH") == 0) // show $PATH variable
  {
    showPath();
    return 0;
  }
  else if (strcmp(cmdArgs[0], "a2path") == 0)
  {
    if (cmdArgs[2] == NULL)
    {
      addToPath(cmdArgs[1]);
    }
    return 0;
  }
  return -1;
}
/* ------------------------ END OF FUNCTION DEFINITIONS -------------------- */

/* ------------------------------ MAIN PROGRAM ----------------------------- */
int main(int argc, char **argv) {
  /* Welcome message! */
  welcomeMsg();
  while (TRUE)
  {
    char input[PATH_MAX] = "";
    char * tokArgs[PATH_MAX] = {NULL};
    int i = 0;
    int fd, status;
    int redirOutputFlag = FALSE;
    pid_t pid;
    int pipeFd[2];

    // print string prompt
    printf("dragonshell > ");
    fflush(stdout);
    if (fgets(&input[0], PATH_MAX, stdin) == NULL)
    {
      printf("\ndragonshell: Failed to read from stdin.\n");
      fflush(stdout);
      continue;
    }

    // get rid of newline character at end
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n')
    {
      input[len - 1] = '\0';
    }

    // tokenize into separate commands using delimiter ';'
    tokenize(&input[0], ";", &tokArgs[0]);

    while (tokArgs[i] != NULL)
    {
      char * cmdArgs[PATH_MAX] = {NULL};

      if ((pid = fork()) < 0)
      {
        printf("dragonshell: fork error!\n");
      }
      if (pid == 0)
      {
        // tokenize into input cmd and output file using delimiter '>'
        char * inOutToks[PATH_MAX] = {NULL};
        tokenize(tokArgs[i], ">", &inOutToks[0]);

        if (inOutToks[1] != NULL)
        {
          // Output file was supplied, write stdout to specified filename
          redirOutputFlag = TRUE;

          // Get filename
          char * filename = strtok(inOutToks[1], " ");

          // Open
          fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
          if (fd == -1) {
            printf("dragonshell: Could not open file.\n");
            _exit(1);
          }
          // Use dup2 to use opened file fd and STDOUT fd interchangeably
          if (dup2(fd, STDOUT_FILENO) == -1) {
            printf("dragonshell: Could not create file descriptor copy.\n");
            close(fd);
            _exit(1);
          }
        }

        // tokenize into pipe sections using delimiter '|'
        char * pipeCmds[PATH_MAX] = {NULL};
        tokenize(inOutToks[0], "|", &pipeCmds[0]);
        if (pipeCmds[1] != NULL)
        {
          int pipeRc;
          if ((pipeRc = pipe(pipeFd)) == -1)
          {
            printf("dragonshell: pipe error!\n");
            _exit(1);
          }

          int rwIdx, cStatus; // read or write end of pipe

          pid_t cid;
          if ((cid = fork()) < 0) {
            printf("dragonshell: fork error!\n");
            _exit(1);
          }
          if (cid > 0)
          {
            // Child proceeds
            rwIdx = 1; // child reads
            close(pipeFd[1]); // close write end
            dup2(pipeFd[0], STDIN_FILENO);
            close(pipeFd[0]);
          }
          else if (cid == 0)
          {
            // Parent proceeds
            rwIdx = 0; //parent writes
            close(pipeFd[0]); // close read end
            dup2(pipeFd[1], STDOUT_FILENO);
            close(pipeFd[1]);
          }
          // tokenize into separate commands/arguments using delimiter ' '
          tokenize(pipeCmds[rwIdx], " ", &cmdArgs[0]);
        }
        else
        {
          // tokenize into separate commands/arguments using delimiter ' '
          tokenize(pipeCmds[0], " ", &cmdArgs[0]);
        }

        // printf("cmdArgs[0]: %s\n", cmdArgs[0]);
        if (cmdArgs[0] == NULL) break;
        // Decide if should be background process based on last cmd line arg
        /* TO-DO: has to come before getting rid of quotes*/

        // get rid of quotation marks around commands/arguments so they can be properly exec'd
        int k = 0;
        while (cmdArgs[k] != NULL)
        {
          removeQuotes(cmdArgs[k]);
          // printf("removed quotes, remains: %s\n", cmdArgs[k]);
          k++;
        }

        /* Check and execute if built-in command */
        int rc = basicCmds(cmdArgs);
        if (rc != 0) // It wasn't a basic command, check other cases
        {
          if (strcmp(cmdArgs[0], "exit") == 0) // exit dragonshell
          {
            fflush(stdout);
            _exit(0);
          }
          else
          {
            int rc;
            if ((rc = executeCmd(cmdArgs)) == -1)
            {
              // print error message
              printf("dragonshell: command not found\n");
            }
          }
          fflush(stdout);
          if (redirOutputFlag == TRUE)
          {
            if (close(fd) == -1)
            {
              printf("dragonshell: Error closing file.\n");
              _exit(1);
            }
          }
        }
        _exit(0);
      }
      else
      {
        // if not BG process, wait for process to complete before returning to prompt
        waitpid(pid, &status, 0);

        // need to retokenize since child process' local variables no longer accessible
        tokenize(tokArgs[i], " ", &cmdArgs[0]);

        // again, get rid of quotation marks around commands/arguments so they can be properly exec'd
        int k = 0;
        while (cmdArgs[k] != NULL)
        {
          removeQuotes(cmdArgs[k]);
          k++;
        }

        // Decide what command to run based on first cmd line arg
        if (strcmp(cmdArgs[0], "cd") == 0)
        {
          // printf("changing directory from parent\n");
          changeDirectory(cmdArgs[1]);
        }
        else if (strcmp(cmdArgs[0], "a2path") == 0)
        {
          if ((cmdArgs[2] != NULL) && (strcmp(cmdArgs[2], ">") != 0))
          {
            printf("dragonshell: a2path only supports one command argument.\n");
          }
          else
          {
            addToPath(cmdArgs[1]);
          }
        }

        fflush(stdout);

        if (strcmp(cmdArgs[0], "exit") == 0) // exit dragonshell
        {
          // TO-DO: close all active forked processes
          fflush(stdout);
          printf("Goodbye!\n");
          // Make sure all running process and background processed killed...
          if (kill(pid, SIGKILL) == 0)
          {
            printf("Killed process %d\n", pid);
          }
          _exit(0);
        }
      }
    i++; // increment semicolon-separated commands counter
    }
  }
  return 0;
}
/* ------------------------ END OF MAIN FUNCTION -------------------------- */
