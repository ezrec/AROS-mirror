#ifndef _SYS_STAT_INCLUDED
#define _SYS_STAT_INCLUDED

/*
 * ** stat structure used by fstat() and stat()
 * ** will not be compatible with your compiler's stat() and fstat()
 */
#include <sys/types.h>

#ifndef _BOOL_T
#define _BOOL_T
     typedef int bool_t;
#endif /* _DEV_T */

#ifndef _ENUM_T
#define _ENUM_T
     typedef int enum_t;		/* For device numbers */
#endif /* _DEV_T */

#ifndef _TIME_T
#define _TIME_T
     typedef long time_t;		/* For times in seconds */
#endif /* _TIME_T */

#ifndef _SIZE_T
#define _SIZE_T
     typedef unsigned int size_t;	/* Type returned by sizeof() */
#endif /* _SIZE_T */

#ifndef _SSIZE_T
#define _SSIZE_T
      typedef int ssize_t;		/* Signed version of size_t */
#endif /* _SSIZE_T */

#ifndef _SITE_T
#define _SITE_T
     typedef unsigned short __site_t;	/* see stat.h */
#endif /* _SITE_T */

#ifndef _CNODE_T
#define _CNODE_T
     typedef unsigned short __cnode_t;	/* see stat.h */
#endif /* _CNODE_T */

   typedef unsigned char  ubit8;
   typedef unsigned short ubit16;
   typedef unsigned long  ubit32;
   typedef char 	  sbit8;
   typedef short	  sbit16;
   typedef long 	  sbit32;

   typedef __site_t	  site_t;
   typedef __cnode_t	  cnode_t;

   typedef long 	  paddr_t;
   typedef short	  cnt_t;
   typedef unsigned int   space_t;
   typedef unsigned int   prot_t;
   typedef unsigned long  cdno_t;
   typedef unsigned short use_t;

   typedef struct _physadr { int r[1]; } *physadr;
   typedef struct _quad { long val[2]; } quad;

   typedef char spu_t;

struct stat
{
  dev_t st_dev;
  ino_t st_ino;
  mode_t st_mode;
  nlink_t st_nlink;
  unsigned short st_reserved1;	/* old st_uid, replaced spare positions */
  unsigned short st_reserved2;	/* old st_gid, replaced spare positions */
  dev_t st_rdev;
  off_t st_size;
  time_t st_atime;
  int st_spare1;
  time_t st_mtime;
  int st_spare2;
  time_t st_ctime;
  int st_spare3;
  long st_blksize;
  long st_blocks;
  unsigned int st_pad:30;
  unsigned int st_acl:1;	/* set if there are optional ACL entries */
  unsigned int st_remote:1;	/* Set if file is remote */
  dev_t st_netdev;		/* ID of device containing */
  /* network special file */
  ino_t st_netino;		/* Inode number of network special file */
  __cnode_t st_cnode;
  __cnode_t st_rcnode;
  /* The site where the network device lives                      */
  __site_t st_netsite;
  short st_fstype;
  /* Real device number of device containing the inode for this file */
  dev_t st_realdev;
  /* Steal three spare for the device site number                   */
  unsigned short st_basemode;
  unsigned short st_spareshort;
  uid_t st_uid;
  gid_t st_gid;
#define _SPARE4_SIZE 3
  long st_spare4[_SPARE4_SIZE];
};

/* st_mode will have bits set as follows */
/* the least significant 9 bits will be the unix
 * ** rwxrwxrwx bits (octal 777).
 */

#ifndef S_IRWXU			/* fcntl.h might have already defined these */
#define S_ISUID 0004000		/* set user ID on execution */
#define S_ISGID 0002000		/* set group ID on execution */

#define S_IRWXU 0000700		/* read, write, execute permission (owner) */
#define S_IRUSR 0000400		/* read permission (owner) */
#define S_IWUSR 0000200		/* write permission (owner) */
#define S_IXUSR 0000100		/* execute permission (owner) */

#define S_IRWXG 0000070		/* read, write, execute permission (group) */
#define S_IRGRP 0000040		/* read permission (group) */
#define S_IWGRP 0000020		/* write permission (group) */
#define S_IXGRP 0000010		/* execute permission (group) */

#define S_IRWXO 0000007		/* read, write, execute permission (other) */
#define S_IROTH 0000004		/* read permission (other) */
#define S_IWOTH 0000002		/* write permission (other) */
#define S_IXOTH 0000001		/* execute permission (other) */
#endif /* S_IRWXU */

#define _S_IFMT   0170000	/* type of file */
#define _S_IFREG  0100000	/* regular */
#define _S_IFBLK  0060000	/* block special */
#define _S_IFCHR  0020000	/* character special */
#define _S_IFDIR  0040000	/* directory */
#define _S_IFIFO  0010000	/* pipe or FIFO */
#define S_IFMT	  _S_IFMT	/* type of file */
#define S_IFBLK   _S_IFBLK	/* block special */
#define S_IFCHR   _S_IFCHR	/* character special */
#define S_IFDIR   _S_IFDIR	/* directory */
#define S_IFIFO   _S_IFIFO	/* pipe or FIFO */
#define S_IFREG   _S_IFREG	/* regular */

#define S_IFSOCK  0140000	/* socket */
#define S_IFLNK   0120000	/* symbolic link */
#define S_IFNWK   0110000	/* network special */

#define S_ISDIR(_M)  ((_M & _S_IFMT)==_S_IFDIR)		/* test for directory */
#define S_ISCHR(_M)  ((_M & _S_IFMT)==_S_IFCHR)		/* test for char special */
#define S_ISBLK(_M)  ((_M & _S_IFMT)==_S_IFBLK)		/* test for block special */
#define S_ISREG(_M)  ((_M & _S_IFMT)==_S_IFREG)		/* test for regular file */
#define S_ISFIFO(_M) ((_M & _S_IFMT)==_S_IFIFO)		/* test for pipe or FIFO */
#define S_ISSOCK(_M) ((_M & S_IFMT)==S_IFSOCK)	/* test for socket */
#define S_ISLNK(_M)  ((_M & S_IFMT)==S_IFLNK)	/* test for symbolic link */

int stat(const char *name, struct stat *buf);
//int lstat(const char *name, struct stat *buf);
int fstat(int fd, struct stat *buf);

#ifndef S_IREAD
#define S_IREAD      S_IRUSR
#define S_IWRITE     S_IWUSR
#define S_IEXEC      S_IXUSR
#endif

#define btod(p, t) ((t)(((long)p)<<2))
#define dtob(p) ((BPTR)((long)(p)>>2))

#define EBADF		9	/* bad file handle			*/
#define ENOMEM		12	/* no memory				*/

#endif /* _SYS_STAT_INCLUDED */
