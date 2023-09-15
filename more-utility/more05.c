#include <stdio.h>
#include <stdlib.h>

// getting when using space bar

#define LINELEN 512
#define PAGELEN 20

void showFileContents(FILE *);
int getInput(FILE *, int);
int countTotalLines(FILE *);

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        showFileContents(stdin);
        // // printf("Redirecting to man page of more");
        // char cmd[] = "man more";
        // system(cmd); // used to run terminal commands in c program
        // exit(0);
    }

    // making mutiple files to handle in more

    FILE *fp;
    int i = 0;
    while (++i < argc) // now we can see multiple file using this program
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
{printf("\033[1A \033[2K \033[1G");
    int rval;
    int percentage;
    int totalLines = 0;
    int linesforwarding = 0;
    char buffer[LINELEN];
    int LinesInFile = countTotalLines(fp);
    printf("%d\n", LinesInFile);
    FILE *fpStdin = fopen("/dev//tty", "r"); // taking input from terminal for flags

    rewind(fp); // resetting the file pointer after counting total lines pointer become at end

    while (fgets(buffer, LINELEN, fp) != NULL)
    {
        linesforwarding++;
        percentage = (linesforwarding * 100) / LinesInFile;
        fputs(buffer, stdout);
        totalLines++;

        if (totalLines == PAGELEN)
        {
            rval = getInput(fpStdin, percentage);
            if (rval == 0) // user pressed q
            {
                // line that removes previous line --more--(%) from terminal
                printf("\033[1A \033[2K \033[1G");
                break;
            }
            else if (rval == 1) // user presses space bar
            {
                printf("\033[1A \033[2K \033[1G");
                // printf("\033[1A \033[2K \033[1G");
                totalLines = totalLines - PAGELEN;
                // printf("%d\n", totalLines);
            }
            else if (rval == 2) // user pressed enter
            {
                printf("\033[1A \033[2K \033[1G");
                totalLines = totalLines - 1;
            }
            else if (rval == 3) // user gives invalid character
            {
                printf("\033[1A \033[2K \033[1G");
                break;
            }
        }
    }
    // printf("\033[1A \033[2K \033[1G");
}

int getInput(FILE *cmdStream, int percentage)
{
    printf("\n \033[7m --more--(%d%%) \033[m", percentage); // line used to display --more--(%) on screen we have to remove it in the parent function
    int i;
    i = getc(cmdStream);
    if (i == 'q')
    {
        return 0;
    }
    else if (i == ' ')
    {
        return 1;
    }
    else if (i == '\n')
    {
        return 2;
    }
    else
    {
        return 3;
    }
    return 0;
}

// calculating total lines of file opened

int countTotalLines(FILE *fp)
{

    int totalLinesInFile = 0;
    char buf[LINELEN];

    while (fgets(buf, LINELEN, fp) != NULL)
    {
        totalLinesInFile++;
    }

    return totalLinesInFile;
}