#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

extern int errno;

void do_ls(char *);
void fileinfo(char *);

int main(int argc, char const *argv[])
{
    if (argc == 1)
    {
        // printf("Give at least one argument");
        do_ls(".");
    }
    else
    {
        int i = 0;
        while (++i < argc)
        {
            do_ls(argv[i]);
        }
    }
    // do_ls(argv[1]);
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

    printf("Mode \t Link \t user \t group \t size \t modtime \t name \n");
    while ((entry = readdir(dp)) != NULL)
    {
        if (entry == NULL && errno != 0)
        {
            printf("Error occured");
            exit(errno);
        }
        else
        {
            if (entry->d_name[0] == '.')
                continue;
            // printf("%s\t", entry->d_name);
            fileinfo(entry->d_name);
        }
    }
    printf("\n");
    closedir(dp);
}

void fileinfo(char *filename)
{
    struct stat info;
    int ret = lstat(filename, &info);

    if (ret == -1)
    {
        perror("Failed to get stats");
        exit(errno);
    }

    // printf("name:: %s\n", filename);
    // printf("mode:: %o\n", info.st_mode);
    // printf("link count:: %ld\n", info.st_nlink);
    // printf("user:: %d\n", info.st_uid);
    // printf("group:: %d\n", info.st_gid);
    // printf("size:: %ld\n", info.st_size);
    // printf("modtime:: %ld\n", info.st_mtime);

    printf("%o \t %ld \t %d \t %d \t %ld \t %ld \t %s \n", info.st_mode, info.st_nlink, info.st_uid, info.st_gid, info.st_size, info.st_mtime, filename);
}