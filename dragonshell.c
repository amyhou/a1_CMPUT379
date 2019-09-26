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

int main(int argc, char **argv) {
  // print the string prompt without a newline, before beginning to read
  // tokenize the input, run the command(s), and print the result
  // do this in a loop
  // program running flag
  int prog = TRUE;

  /* Print welcome message */
  printf("Welcome to DragonShell!\n");
  printf("-------------------------\n");

  while (prog == TRUE)
  {
    char input[PATH_MAX] = "";
    char * tokArgs[PATH_MAX] = {NULL};
    
    // print string prompt
    printf("dragonshell > ");
    // get input
    if (scanf("%[^\n]%*c", &input[0]) != 1)
    {
      printf("\ndragonshell: Failed to read from stdin.\n");
      fflush(stdin);
      continue;
    }

    // tokenize into separate commands using delimiter ';'
    tokenize(&input[0], ";", &tokArgs[0]);

    int i = 0;
    while (tokArgs[i] != NULL)
    {
      // tokenize into separate commands/arguments using delimiter ' '
      char * cmdArgs[PATH_MAX] = {NULL};
      printf("tokArgs[i]: %s\n", tokArgs[i]);
      fflush(stdout);

      tokenize(tokArgs[i], " ", &cmdArgs[0]);
      
      printf("cmdargs[0]: %s, cmdargs[1]: %s\n", cmdArgs[0], cmdArgs[1]);
      fflush(stdout);
      if (strcmp(cmdArgs[0], "exit") == 0) // exit dragonshell
      {
        prog = FALSE;
        break;
      }
      else // command not found
      {
        printf("\ndragonshell: Command not found\n");
        break;
      }
      i++;
    }
    fflush(stdout);
  }
  return 0;
}
