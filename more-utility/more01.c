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

    FILE *fp;
    fp = fopen(argv[1], "r"); // fopen call to open the file in argument first

    if (fp == NULL)
    {
        perror("Something went wrong!");
        exit(1);
    }

    showFileContents(fp);
    fclose(fp);

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