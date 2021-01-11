#include "helper.h"

#include <sys/fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

void redirect(char** args, int *argc)
{
     // The index of ">", "<" operators
    int out_redirect = 0;
    int in_redirect = 0;

    for (int i = 0; i < *argc; i++) 
    {
        if (strcmp(args[i], ">") == 0) out_redirect = i; 
        if (strcmp(args[i], "<") == 0) in_redirect = i; 
    }

    if (out_redirect != 0) {
            int file = open(args[out_redirect + 1], O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IRGRP | S_IWGRP |S_IWUSR);
            dup2(file, 1);
            close(file);

            *argc -= 2;
            args[out_redirect] = '\0';
        }
        else if (in_redirect != 0)
        {
            int file = open(args[in_redirect + 1], O_RDONLY);
            if (file < 0) {
                printf("Failed: %s\n", strerror(errno));
            }
            
            dup2(file, STDIN_FILENO);
            close(file);

            *argc -= 2;
            args[in_redirect] = '\0';
        }
}