/***
 *    ________                                       _________.__           .__  .__
 *    \______ \____________     ____   ____   ____  /   _____/|  |__   ____ |  | |  |
 *     |    |  \_  __ \__  \   / ___\ /  _ \ /    \ \_____  \ |  |  \_/ __ \|  | |  |
 *     |    `   \  | \// __ \_/ /_/  >  <_> )   |  \/        \|   Y  \  ___/|  |_|  |__
 *    /_______  /__|  (____  /\___  / \____/|___|  /_______  /|___|  /\___  >____/____/
 *            \/           \//_____/             \/        \/      \/     \/
 */

#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

/* Questions:
  1. Do we need to fflush after each printf statement?
*/

/* ------------------------------ DEFINE MACROS ---------------------------- */
#define TRUE  (1)
#define FALSE (0)
/* --------------------------- END OF DEFINED MACROS ----------------------- */

/* ----------------------------- GLOBAL VARIABLES -------------------------- */
pid_t pid; //shared child pid
char shellPath[PATH_MAX] = "/bin/:/usr/bin/";
/* ------------------------- END OF GLOBAL VARIABLES ----------------------- */


/* -------------------------- FUNCTIONS DEFINITIONS ------------------------ */
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

void changeDirectory(char * dirPath) {
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
  tokenize(&path[0], ":", &newPaths[0]);
  if (strcmp(newPaths[0], "$PATH") != 0)
  {
    // printf("Clearing $PATH var\n");
    memset(&shellPath[0], 0, sizeof(shellPath));
  }
  else
  {
    strcat(shellPath, ":");
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

void welcomeMsg() {
  /* Print welcome message (make fancier later) */
  printf("Welcome to Dragonshell!\n");
  printf("-------------------------\n");
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
      rc = execve(strcat(tmp, cmdArgs[0]), argv1, envp1);
      j++;
    }
  }
  return rc;
}
/* ------------------------ END OF FUNCTION DEFINITIONS -------------------- */


/* ------------------------------ MAIN PROGRAM ----------------------------- */
int main(int argc, char **argv) {
  // print the string prompt without a newline, before beginning to read
  // tokenize the input, run the command(s), and print the result
  // do this in a loop

  /* Welcome message! */
  welcomeMsg();

  while (TRUE)
  {
    char input[PATH_MAX] = "";
    char * tokArgs[PATH_MAX] = {NULL};

    // print string prompt
    printf("dragonshell > ");

    if (fgets(&input[0], PATH_MAX, stdin) == NULL)
    {
      printf("\ndragonshell: Failed to read from stdin.\n");
      fflush(stdout);
      continue;
    }

    // get rid of newline character at end
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n') {
      input[len - 1] = '\0';
    }

    // tokenize into separate commands using delimiter ';'
    tokenize(&input[0], ";", &tokArgs[0]);

    int i = 0;
    while (tokArgs[i] != NULL)
    {
      // tokenize into separate commands/arguments using delimiter ' '
      char * cmdArgs[PATH_MAX] = {NULL};
      // printf("tokArgs[i]: %s\n", tokArgs[i]);
      // fflush(stdout);

      tokenize(tokArgs[i], " ", &cmdArgs[0]);

      // printf("cmdargs[0]: %s, cmdargs[1]: %s\n", cmdArgs[0], cmdArgs[1]);
      // fflush(stdout);

      if (cmdArgs[0] == NULL) break;

      // Decide if should be background process based on last cmd line arg
      /* TO-DO: */

      // Decide what command to run based on first cmd line arg
      if (strcmp(cmdArgs[0], "cd") == 0)
      {
        changeDirectory(cmdArgs[1]);
      }
      else if (strcmp(cmdArgs[0], "pwd") == 0)
      {
        printWorkingDirectory();
      }
      else if (strcmp(cmdArgs[0], "$PATH") == 0) // show $PATH variable
      {
        showPath();
      }
      else if (strcmp(cmdArgs[0], "a2path") == 0)
      {
        if (cmdArgs[2] != NULL)
        {
          printf("dragonshell: a2path only supports one command argument.\n");
        }
        else
        {
          addToPath(cmdArgs[1]);
        }
      }
      else if (strcmp(cmdArgs[0], "exit") == 0) // exit dragonshell
      {
        // TO-DO: close all active forked processes
        fflush(stdout);
        printf("Goodbye!\n");
        // _exit(0);
        return 0;
      }
      else // try to exec
      {
        int rc;
        if ((pid = fork()) < 0) perror("fork error!");
        if (pid == 0)
        {
          // printf("Child\n");
          if ((rc = executeCmd(cmdArgs)) == -1)
          {
            // print error message
            printf("dragonshell: command not found\n");
            _exit(0);
          }
        }
        else
        {
          waitpid(pid, &rc, 0); // should not use sleep! we should use wait() for child to return
          // printf("child pid: %d\n", pid);
          // printf("Parent killing child.\n");
          // kill(pid, SIGKILL);
        }
      }
      i++;
    }
    fflush(stdout);
  }
  return 0;
}
/* ------------------------ END OF MAIN FUNCTION -------------------------- */
