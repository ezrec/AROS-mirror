extern "C"
{

#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>

#include <aros/debug.h>

int access(const char *path, int mode)
{
    bug("Not Implemented %s\n", __func__);
}

int closedir(DIR *dir)
{
    bug("Not Implemented %s\n", __func__);
}
int chmod(const char *path, mode_t mode)
{
    bug("Not Implemented %s\n", __func__);
}

int creat(const char * filename, int mode)
{
    bug("Not Implemented %s\n", __func__);
}
int dup2(int oldfd, int newfd)
{
    bug("Not Implemented %s\n", __func__);
}
int execv(const char *path, char *const argv[])
{
    bug("Not Implemented %s\n", __func__);
}
int execve(const char *filename, char *const argv[], char *const envp[])
{
    bug("Not Implemented %s\n", __func__);
}

int fstat(int fd, struct stat *sb)
{
    bug("Not Implemented %s\n", __func__);
}

char *getcwd(char *buf, size_t size)
{
    bug("Not Implemented %s\n", __func__);
}

gid_t getgid(void)
{
    bug("Not Implemented %s\n", __func__);
}

int kill(pid_t, int)
{
    bug("Not Implemented %s\n", __func__);
}

int getrlimit(int resource, struct rlimit *rlp)
{
    bug("Not Implemented %s\n", __func__);
}

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/timer.h>

struct timezone;

int gettimeofday (struct timeval * tv,struct timezone * tz)
{
    struct MsgPort * timerport = CreateMsgPort();
    struct timerequest * timereq = (struct timerequest *)CreateIORequest(timerport, sizeof(*timereq));


    if (timereq)
    {
        if (OpenDevice("timer.device", UNIT_VBLANK, (struct IORequest *)timereq, 0) == 0)
        {
            #define TimerBase ((struct Device *)timereq->tr_node.io_Device)

            GetSysTime(tv);

            #undef TimerBase

            CloseDevice((struct IORequest *)timereq);
        }
    }

    DeleteIORequest((struct IORequest *)timereq);
    DeleteMsgPort(timerport);

    return 0;
}
uid_t getuid(void)
{
    bug("Not Implemented %s\n", __func__);
}

void __has_sse(void)
{
    bug("Not Implemented %s\n", __func__);
}

void __test_sse(void)
{
    bug("Not Implemented %s\n", __func__);
}

int isatty(int fd)
{
    bug("Not Implemented %s\n", __func__);
}

int link(const char *name1, const char *name2)
{
    bug("Not Implemented %s\n", __func__);
}

off_t lseek(int filedes, off_t offset, int whence)
{
    bug("Not Implemented %s\n", __func__);
}

int lstat(const char *path, struct stat *sb)
{
    bug("Not Implemented %s\n", __func__);
}

int mkdir(const char *path, mode_t mode)
{
    bug("Not Implemented %s\n", __func__);
}

int mkstemp(char *)
{
    bug("Not Implemented %s\n", __func__);
}

DIR *opendir(const char *filename)
{
    bug("Not Implemented %s\n", __func__);
}

ssize_t read(int d, void *buf, size_t nbytes)
{
    bug("Not Implemented %s\n", __func__);
}

struct dirent *readdir(DIR *dir)
{
    bug("Not Implemented %s\n", __func__);
}

int rmdir(const char *path)
{
    bug("Not Implemented %s\n", __func__);
}

int setrlimit(int resource, const struct rlimit *rlp)
{
    bug("Not Implemented %s\n", __func__);
}

int sigaction(int, const struct sigaction *, struct sigaction *)
{
    bug("Not Implemented %s\n", __func__);
}

int sigprocmask(int, const sigset_t *, sigset_t *)
{
    bug("Not Implemented %s\n", __func__);
}

int stat(const char * restrict path, struct stat * restrict sb)
{
    bug("Not Implemented %s\n", __func__);
}

int symlink(const char *name1, const char *name2)
{
    bug("Not Implemented %s\n", __func__);
}

int truncate(const char *path, off_t length)
{
    bug("Not Implemented %s\n", __func__);
}

mode_t umask(mode_t numask)
{
    bug("Not Implemented %s\n", __func__);
}

int uname(struct utsname *name)
{
    bug("Not Implemented %s\n", __func__);
}

int unlink(const char *path)
{
    bug("Not Implemented %s\n", __func__);
}

int usleep (useconds_t usec)
{
    bug("Not Implemented %s\n", __func__);
}

pid_t wait(int *status)
{
    bug("Not Implemented %s\n", __func__);
}

pid_t waitpid(pid_t pid, int *status, int options)
{
    bug("Not Implemented %s\n", __func__);
}

ssize_t write(int fd, const void *buf, size_t nbytes)
{
    bug("Not Implemented %s\n", __func__);
}

int sigfillset(sigset_t *)
{
    bug("Not Implemented %s\n", __func__);
}

int sigemptyset(sigset_t *)
{
    bug("Not Implemented %s\n", __func__);
}

}
