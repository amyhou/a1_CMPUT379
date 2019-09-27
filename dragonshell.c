#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

#define TRUE  (1)
#define FALSE (0)
/**
 * @brief Tokenize a C string
 *
 * @param str - The C string to tokenize
 * @param delim - The C string containing delimiter character(s)
 * @param argv - A char* array that will contain the tokenized strings
 * Make sure that you allocate enough space for the array.
 */
void tokenize(char* str, const char* delim, char ** argv) {
  char* token;
  token = strtok(str, delim);
  for(size_t i = 0; token != NULL; ++i){
    argv[i] = token;
    token = strtok(NULL, delim);
  }
}

void changeDirectory(char * dirPath) {
  int rc;
  if ((rc = chdir(dirPath)) != 0)
  {
    printf("Failed to change directory.");
    fflush(stdout);
  }
}

void printWorkingDirectory() {
  char path[PATH_MAX];
  printf("%s\n", getcwd(path, PATH_MAX));
  fflush(stdout);
}

void showPath() {
  int rc = execve();
  if (rc == -1)
  {
    printf("Could not print $PATH.\n");
  }
}

void welcomeMsg() {
  /* Print welcome message (make fancier later) */
  printf("Welcome to DragonShell!\n");
  printf("-------------------------\n");
}

int main(int argc, char **argv) {
  // print the string prompt without a newline, before beginning to read
  // tokenize the input, run the command(s), and print the result
  // do this in a loop

  pid_t pid; //shared child pid

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
        // char path[PATH_MAX];
        // printf("Enter path of program: ");
        // scanf("%s", &cmdArgs[0]);
        if ((pid = fork()) < 0) perror("fork error!");
        if (pid == 0) {
            // printf("Child\n");
            char *argv1[] = {NULL}; // change this!
            printf("cmdArgs[1]: %s\n", cmdArgs[1]);
            int rc = execve(cmdArgs[0], cmdArgs, argv1);
            if (rc == -1)
            {
                perror("dragonshell: command not found\n");
            }
        }
        else {
            sleep(1); // should not use sleep!
            // printf("Parent killing child.\n");
            kill(pid, SIGKILL);
        }
        // printf("dragonshell: command not found\n");
      }
      i++;
    }
    fflush(stdout);
  }
  return 0;
}
