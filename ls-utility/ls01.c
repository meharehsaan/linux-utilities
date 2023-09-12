#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

extern int errno;

int main(int argc, char const *argv[])
{
    if (argc == 1)
    {
        printf("Give at least one argument");
        // do_ls(".");
    }
    else
    {
        do_ls(argv[1]);
    }

    return 0;
}

void do_ls(char *dir)
{
    struct dirent *entry;
    DIR *dp = opendir(dir);

    if (dp == NULL)
    {
        fprintf(stderr, "Cannot open %s\n", dir);
    }

    errno = 0;

    while ((entry = readdir(dp)) != NULL)
    {
        if (entry == NULL && errno != 0)
        {
            printf("Error occured");
        }
        else
        {
            printf("%s\n", entry->d_name);
        }
    }
    closedir(dp);
}
