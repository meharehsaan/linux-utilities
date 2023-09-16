#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

void mydaemon();

int main(int argc, char const *argv[])
{
    mydaemon();
    return 0;
}

void mydaemon()
{
    // creating new process by fork() call
    pid_t cpid = fork();
    if (cpid != 0) // its the parent process
    {
        exit(0);
    }

    // closing file descriptors
    struct rlimit r;
    getrlimit(RLIMIT_NOFILE, &r);
    for (int fd = 3; fd < r.rlim_max; fd++)
    {
        close(fd);
    }

    // locking process instance so that no one can run it again and again
    int fd = open("f1.txt", O_CREAT | O_TRUNC | O_RDWR, 0666);
    if (fd == -1)
    {
        perror("open");
    }

    struct flock lock;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    int rv = fcntl(fd, F_SETLK, &lock);
    if (rv == -1)
    {
        printf("This process is already running\n");
        close(fd);
        exit(1);
    }
    fprintf(stderr, "Daemon has started running with PID: %d\n", getpid());

    // make std descriptor pointing to /dev/null
    int fd0 = open("/dev/null", O_RDWR);
    dup2(fd0, 0);
    dup2(fd0, 1);
    dup2(fd0, 2);
    close(fd0);

    // making daemon session leader so it not become the child of someone
    setsid(); // detaching terminal

    // changing dir to / to ignore mounting issues
    umask(0); // zero the umask so that process created by daemon remain same permissions as given to them
    chdir("/");

    // ignoring signals so that no one can kill our daemon
    signal(SIGHUP, SIG_IGN);
}

// check my github.com/meharehsaan/system-programming repo in daemons directory to go in detail how to get out of lock in the file
// lock.c
