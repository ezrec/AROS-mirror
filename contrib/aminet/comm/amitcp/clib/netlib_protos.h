#ifndef CLIB_NETLIB_PROTOS_H
#define CLIB_NETLIB_PROTOS_H
/*
**      $Filename: clib/netlib_protos.h $
**	$Release$
**      $Revision$
**      $Date$
**
**	Prototypes for netlib utility functions
**
**	Copyright © 1993 AmiTCP/IP Group, <AmiTCP-Group@hut.fi>
**                  Helsinki University of Technology, Finland.
**                  All rights reserved.
*/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef _SYS_TYPES_H_
#include <sys/types.h>
#endif
#ifndef _CDEFS_H_
#include <sys/cdefs.h>
#endif
#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif
#ifndef _SYS_TIME_H_
#include <sys/time.h>
#endif
#ifndef _SYS_SOCKET_H_
#include <sys/socket.h>
#endif
#ifndef _NETDB_H_
#include <netdb.h>
#endif
#ifndef _NETINET_IN_H_
#include <netinet/in.h>
#endif
#ifndef _UNISTD_H_
#include <unistd.h>
#endif
#ifndef _SYS_STAT_H
#include <sys/stat.h>
#endif
#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif

#ifndef LINEREAD_H
#include <lineread.h>
#endif

/* Many of these functions are in ixemul.library already, so we'll #ifdef it out
if we can. LW */
#ifdef __SASC
/* access.c */
int __access(const char *name, int mode);

/* chmod.c */
int chmod(const char *path, int mode);

/* chown.c */
int chown(const char *name, uid_t uid, gid_t gid);

/* dostat.c */
void __dostat(struct FileInfoBlock *fib, struct stat *st);

/* dummy.c */
struct hostent  *gethostent(void);
struct netent  *getnetent(void);
struct servent  *getservent(void);
struct protoent *getprotoent(void);

/* fhopen.c */
int fhopen(long file, int mode);

/* fib.c */
extern struct FileInfoBlock __dostat_fib[1];

/* getopt.c */
extern int opterr;
extern int optind;
extern int optopt;
extern char *optarg;
int   getopt(int argc, char **argv, char *opts);

/* getpid.c */
pid_t getpid(void);

/* gettimeofday.c */
int   gettimeofday(struct timeval *tp, struct timezone *tzp);

/* herror.c */
void  herror(const char *banner);


/* ioctl.c */
int ioctl(int fd, unsigned int request, char *argp);

/* iomode.c */
int iomode(int fd, int mode);

/* isatty.c */
int isatty(int fd);

/* perror.c */
void  perror(const char *banner);

/* popen.c */
#ifndef _STDIO_H_
#include <stdio.h>
struct __iobuf; /* typedef struct __iobuf FILE */
#endif
struct __iobuf *popen(const char *cmd, const char *mode);
struct __iobuf *popenl(const char *arg0, ...);
int pclose(struct __iobuf *fptr);
void __stdargs _STD_4000_popen(void);
char *mktemp(char * template);

/* rcmd.c */
int   rcmd(char **, int, const char *, const char *, const char *, int *);
int   rresvport(int *alport);

/* setegid.c */
int setegid(gid_t g);

/* seteuid.c */
int seteuid(uid_t u);

/* sleep.c */
void sleep(unsigned int secs);

/* stat.c */
int stat(const char *name, struct stat *st);



/* usleep.c */
void usleep(unsigned int usecs);

/* utime.c */
#ifndef UTIME_H
struct utimbuf;
#endif
int utime(const char *name, const struct utimbuf *times);

#ifndef _IOS1_H
#include <ios1.h>
#endif

/* _allocufb.c */
struct UFB * __allocufb(int *fdp);

/* _chkufb.c */
long __stdargs _STI_1000_install_AmiTCP_callback(void);
long ASM SAVEDS fdCallback(REG(d0) int fd, REG(d1) int action);
struct UFB * __chkufb(int fd);

/* _close.c */
int __close(int fd);

/* _dup.c */
/* _dup2.c */

/* _fstat.c */
int fstat(int fd, struct stat *st);

/* _lseek.c */
long __lseek(int fd, long rpos, int mode);

/* _open.c */
__stdargs int __open(const char *name, int mode, ...);

/* _read.c */
int __read(int fd, void *buffer, unsigned int length);

/* _write.c */
int __write(int fd, const void *buffer, unsigned int length);


#endif /*__SASC */


/* lineread.c */
int   lineRead(struct LineRead * rl);

/* printfault.c */
void  PrintNetFault(LONG code, const UBYTE *banner);

/* printuserfault.c */
void  PrintUserFault(LONG code, const UBYTE *banner);

/* serveraccept.c */
long serveraccept(char *pname, struct sockaddr_in *ha);

/* set_socket_stdio.c */
int set_socket_stdio(int sock);

/* strerror.c */

char *strerror(int code);

/* stubs.c */
#if !defined(__SASC) || !defined(_OPTINLINE) /* these are inlined for SAS/C */
char * inet_ntoa(struct in_addr addr);
struct in_addr inet_makeaddr(int net, int host);
unsigned long inet_lnaof(struct in_addr addr);
unsigned long inet_netof(struct in_addr addr);
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exeptfds,
	   struct timeval *timeout);
#endif

/* syslog.c */
void  openlog(const char *ident, int logstat, int logfac);
void  closelog(void);
int   setlogmask(int pmask);

#endif /* !CLIB_NETLIB_PROTOS_H */

