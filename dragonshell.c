#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

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
  while (1)
  {
    char input[PATH_MAX];
    char * tokArgs[PATH_MAX];
    // print string prompt
    printf("dragonshell > ");
    // get input
    scanf("%s", &input[0]);
    // tokenize into separate commands using delimiter ';'
    tokenize(&input[0], ";", &tokArgs[0]);

    for (int i = 0; i < PATH_MAX; i++)
    {
    printf("%s\n",tokArgs[i]);
      if (tokArgs[i] == "exit" || tokArgs[i] == NULL)
      {
        break;
      }
    }
  }
  return 0;
}
