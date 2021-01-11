#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "func.h"


void execute_cp(char **args, int argc)
{
    int i_flag = 0, r_flag = 0, t_flag = 0, v_flag = 0;
    char* destination;
    char source[256]; source[0] = '\0';
    int source_size = 0;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(args[i], "-i") == 0) 
            i_flag = 1;
        else if (strcmp(args[i], "-r") == 0 || strcmp(args[i], "-R") == 0) 
            r_flag = 1;
        else if (strcmp(args[i], "-t") == 0)
        {
            t_flag = 1;
            destination = (char*) malloc(sizeof(args[i+1]) + 1);
            destination = args[i+1];
            i++;
            destination[strlen(destination)] = 0;
        }
        else if (strcmp(args[i], "-v") == 0)
            v_flag = 1;
        else 
        {
            if (t_flag == 1)
            {
                if (strcmp(args[i], destination) != 0)
                {
                    strcat(source, args[i]);
                    strcat(source, " ");
                    source_size++;
                }
            }
            else if (t_flag == 0)
                if (i != argc - 1)
                {
                    strcat(source, args[i]);
                    strcat(source, " ");
                    source_size++;
                }
        }
    }
    
    // if no desination if specified with -t 
    // use the last specified argument(should be a dir) as the desination
    if (t_flag == 0)
    {
        destination = (char*) malloc(sizeof(args[argc - 1] + 1));
        destination = args[argc - 1];
        destination[strlen(destination)] = 0;
    }

    // Getting each file / directory that we need to copy
    const char* separator = " ";
    char* filename;
    filename = strtok(source, separator);
    while (filename != NULL)
    {
        FILE* source_file;
        source_file = fopen(filename, "r");

        if (source_file == NULL)
        {
            printf("Couldn't open source file: %s\n", filename);
            exit(1);
        }

        char target_file_path[256]; target_file_path[0] = 0;
        strcat(target_file_path, "/");
        strcat(target_file_path, destination);
        if (target_file_path[strlen(target_file_path) - 1] != '/')
            target_file_path[strlen(target_file_path) - 1] = '/';
        strcat(target_file_path, filename);

        FILE* target_file;
        target_file = fopen(target_file_path, "w");

        if (target_file == NULL)
        {
            printf("Couldn't open destination: %s\n", target_file_path);
            exit(1);
        }

        char ch;
        while ((ch = fgetc(source_file)) != EOF)
            fputc(ch, target_file);

        fclose(source_file);
        fclose(target_file);
        filename = strtok(NULL, separator);
    }

    // free(destination);
    // for (int i = 0; i < source_size; i++) {
    //     free(source[i]);
    // }
}

void execute_tee(char **args, int argc)
{
    char input[216];
    fgets(input, sizeof(input), stdin);

    // Looking for '-a' argument to determine the file open mode
    const char* file_open_mode = "w";
    for (int i = 0; i < argc; i++) 
    {
        if (strcmp(args[i], "-a") == 0)
            file_open_mode = "a";
    }

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (strcmp(args[i], "-a") == 0)
                continue;

            FILE *file;

            file = fopen(args[i], file_open_mode);
            if (file == NULL)
            {
                printf("Error opening file");
                exit(1);
            }

            fprintf(file, "%s", input);

            fclose(file);
        }
    }

    printf("%s\n", input);
}

void execute_dirname(char **args, int argc)
{
    if (argc - 1 > 1) 
        printf("too many arguments\n");
    else if (argc - 1 < 1)
        printf("too few arguments\n");
    else 
    {
        char result[216];

        if (args[1][strlen(args[1]) - 1] == '/') 
            args[1][strlen(args[1]) - 1] = '\0';

        // Count the slashes, it there are none just output '.'
        int slashes = 0;
        for (int i = 0; i < strlen(args[1]); i++)
            if (args[1][i] == '/')
                slashes++;
        
        if (slashes == 0) 
        {
            result[0] = '.';
            result[1] = '\0';
        }
        else if (slashes == 1 && args[1][0] == '/') 
        {
            result[0] = '/';
            result[1] = '\0';
        }
        else
        {
            int removed_chars = 0;
            int result_size = 0;

            for (int i = strlen(args[1]) - 1; i >= 0; i--) 
            {
                if (args[1][i] == '/')
                {
                    for (int j = 0; j < i; j++)
                        result[result_size++] = args[1][j];
                    break;
                }
                
            }
            result[result_size] = '\0';
        }

        
        printf("%s\n", result);
    }
}

void execute_command(char **args, int argc)
{
    // Stores the process id for the process which will execute the command
    int status;
    pid_t pid;

    // The index of ">", "<", "|" operators
    int greater_operator = 0;
    int greater2_operator = 0;
    int pipe_operator = 0;

    for (int i = 0; i < argc; i++) 
    {
        if (strcmp(args[i], ">") == 0) greater_operator = 0; 
        if (strcmp(args[i], ">>") == 0) greater2_operator = 0; 
        if (strcmp(args[i], "|") == 0) pipe_operator = 0; 
    }

    // If we have no operators just print the output to the console
    if (greater_operator == 0 && greater2_operator == 0 && pipe_operator == 0)
    {
        if ((pid = fork()) < 0)
        {
            printf("Fork error");
            exit(1);
        }

        if (pid == 0)
        {   
            if (execvp(args[0], args) < 0) {
                printf("Command failed: %s\n", strerror(errno));
                exit(1);
            }
        }
        else 
        {
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                // printf("%d\n", WEXITSTATUS(status));
            }
        }
    }
}