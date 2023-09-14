#include <stdio.h>
#include <stdlib.h>

#define LINELEN 512

void showFileContents(FILE *);

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        // printf("Redirecting to man page of more");
        char cmd[] = "man more";
        system(cmd); // used to run terminal commands in c program
        exit(0);
    }

    // making mutiple files to handle in more

    FILE *fp;
    int i = 0;
    while (++i < argc)  // now we can see multiple file using this program
    {
        fp = fopen(argv[i], "r");

        if (fp == NULL)
        {
            perror("Something went wrong!");
            exit(1);
        }
        showFileContents(fp);
        fclose(fp);
    }

    return 0;
}

// functions reading file contents
void showFileContents(FILE *fp)
{
    char buffer[LINELEN];
    while (fgets(buffer, LINELEN, fp) != NULL)
    {
        fputs(buffer, stdout);
    }
}