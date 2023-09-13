#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

extern int errno;

void do_ls(char const *dir);
void fileinfo(char *filename);
char filetype(struct stat info);
char getpermissions(struct stat info, char* per);
void getusername(int uid,char *username);
void getgroupname(int gid,char *groupname);
void settime(long secs,char *time);


int main(int argc, char const *argv[])
{
    errno = 0;
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

void do_ls(char const *dir)
{
    struct dirent *entry;
    DIR *dp = opendir(dir);

    if (dp == NULL)
    {
        fprintf(stderr, "Cannot open %s\n", dir);
    }

    errno = 0;

    // printf("Mode \t Link \t user \t group \t size \t modtime \t name \n");
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

    if (lstat(filename, &info) == -1)
    {
        perror("Failed to get stats");
        exit(errno);
    }

    char per[10];
    getpermissions(info, per);
    // per[0] = filetype(info);
    char ft = filetype(info);

    char user[32];
    char group[32];

    long secs = info.st_mtime;
    char* timer = ctime(&secs);

    // settime(info.st_mtime, time);
    getusername(info.st_uid, user);
    getgroupname(info.st_gid, group);

    // printf("total %ld \t", info.st_blocks);

    printf("%c%s %ld %s %s %-5ld %-10.15s %s \n", ft, per, info.st_nlink, user, group, info.st_size, timer+4, filename);
    // printf("%s\n", per);
}

void getusername(int uid,char *username){
    struct passwd* pwd = getpwuid(uid);
    // memset(username, '\0', 32);
    strcpy(username, pwd->pw_name);
}

void getgroupname(int gid, char *groupname){
    struct group * grp = getgrgid(gid);
    // memset(groupname, '\0', 32);
    strcpy(groupname, grp->gr_name);
}

void settime(long secs, char* times){
    char timer[17]; 
    memset(times, '-', sizeof(char) * 16);
    timer[16] = '\0';
    sprintf(timer, "%s", ctime(&secs));
    memcpy(times, timer, 16);
}

char filetype(struct stat info){

// These are macros defined in sys.stat.h
// #define S_IFMT   0170000  // Bitmask to extract file type
// #define S_IFBLK  0060000  // Block device file
// #define S_IFCHR  0020000  // Character device file
// #define S_IFDIR  0040000  // Directory
// #define S_IFIFO  0010000  // FIFO (named pipe)
// #define S_IFLNK  0120000  // Symbolic link
// #define S_IFREG  0100000  // Regular file
// #define S_IFSOCK 0140000  // Socket file

    switch (info.st_mode & __S_IFMT)
    {
    case __S_IFBLK : return 'b';
    case __S_IFIFO : return 'p';
    case __S_IFCHR : return 'c';
    case __S_IFDIR : return 'd';
    case __S_IFLNK : return 'l';
    case __S_IFREG : return '-';
    case __S_IFSOCK : return 's';
    default:          return '?';
    }
}

char getpermissions(struct stat info, char* per){
    // memset(per, '\0', 10);
    strcpy(per, "----------");

    if((info.st_mode & 0000400) == 0000400) per[0] = 'r';
    if((info.st_mode & 0000200) == 0000200) per[1] = 'w';
    if((info.st_mode & 0000100) == 0000100) per[2] = 'x';

    if((info.st_mode & 0000040) == 0000040) per[3] = 'r';
    if((info.st_mode & 0000020) == 0000020) per[4] = 'w';
    if((info.st_mode & 0000010) == 0000010) per[5] = 'x';

    if((info.st_mode & 0000004) == 0000004) per[6] = 'r';
    if((info.st_mode & 0000002) == 0000002) per[7] = 'w';
    if((info.st_mode & 0000001) == 0000001) per[8] = 'x';

    if((info.st_mode & 0004000) == 0004000) per[2] = 's';
    if((info.st_mode & 0002000) == 0002000) per[5] = 's';
    if((info.st_mode & 0001000) == 0001000) per[8] = 't';
    per[9] = '\0';
}
