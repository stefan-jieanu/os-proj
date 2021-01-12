#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include "func.h"

/*
A very much unfinished version of the project.
*/

// gcc main.c func.c -o main -lreadline - use to compile the program

int main(int argc, char** argv)
{
    // Chars used for strtok
    const char separator[2] = " ";
    char *word;

    // The input text 
    char *input_string;

    // The input text split into words
    char **arguments;
    char arguments_count = 0;

    // Stores whether or not the program should teremiante
    // 0 - keep going
    // 1 - kill it with fire
    int should_terminate = 0;

    // Get the current directory
    char curr_dir[256];
    memset(curr_dir, '\0', sizeof(curr_dir));
    if (!(getcwd(curr_dir, sizeof(curr_dir)) != NULL))
        printf("Error getting working directory!\n");

    // The "input string" to show to the user
    char input_promp[256]; input_promp[0] = '\0';
    strcat(input_promp, curr_dir);
    strcat(input_promp, ">_");

    printf("hello\n");
    // Parsing the console for arguments
    while ((input_string = readline(input_promp)) != NULL)
    {
        // Adding the command to the history
        if (strlen(input_string) > 0)
            add_history(input_string);
        arguments = (char**)malloc(sizeof(char*) * sizeof(input_string) + 1);
        
        // Parsing the command and splitting it
        word = strtok(input_string, separator);

        arguments_count = 0;
        while (word != NULL)
        {
            arguments[arguments_count++] = word;

            word = strtok(NULL, separator);
        }
        arguments[arguments_count] = 0;

        // The status of the executed command
        int command_status;

        if (strcmp(arguments[0], "exit") == 0) should_terminate = 1;
        else if (strcmp(arguments[0], "version") == 0) print_version();
        else if (strcmp(arguments[0], "help") == 0) print_help();
        else if (strcmp(arguments[0], "cp") == 0) command_status = execute_cp(arguments, arguments_count);
        else if (strcmp(arguments[0], "tee") == 0) command_status = execute_tee(arguments, arguments_count);
        else if (strcmp(arguments[0], "dirname") == 0) command_status = execute_dirname(arguments, arguments_count);
        else command_status = execute_command(arguments, arguments_count);

        if (strcmp(strerror(command_status), "Success") != 0)
        {
            printf("%s\n", strerror(command_status));
        }

        // Freeing up the memory
        free(arguments);
        free(input_string);  
        free(word);

        // Get the current directory
        memset(curr_dir, '\0', sizeof(curr_dir));
        if (!(getcwd(curr_dir, sizeof(curr_dir)) != NULL))
            printf("Error getting working directory!\n");

        // The "input string" to show to the user
        input_promp[0] = '\0';
        strcat(input_promp, curr_dir);
        strcat(input_promp, ">_");

        if (should_terminate == 1)
            exit(0);
    }

    return 0;
}
