/****** unix/stat *********************************************
*
*   NAME
*	stat -- get file status
*
*   SYNOPSIS
*	#include <sys/types.h>
*	#include "stat.h"
*
*	return = stat (filename, buf)
*
*	int stat (const char *, struct stat *);
*
*   FUNCTION
*	stat() obtains information about the named file.
*
*	The stat structure pointer to by "buf" is filled in. The
*	following fields are used:
*
*		u_short 	st_mode;
*		short		st_uid;
*		short		st_gid;
*		long		st_size;
*		long		st_mtime;
*		long		st_atime;	 same as st_mtime
*		short		st_nlink;	 always 1
*		long		st_blksize;
*		long		st_blocks;
*
*   INPUTS
*	filename	full or relative pathname of file
*	buf		pointer to a stat structure
*
*   RESULT
*	On success,  0 is returned.
*	On failure, -1 is returned, and errno is set.
*
*   EXAMPLE
*	struct stat buf;
*
*	if (stat ("devs:foo", &buf) == -1)
*	{
*	    printf ("Error: file not found\n");
*	    exit (1);
*	}
*
*   NOTES
*	On Amiga files, st_uid and st_gid will be set to that
*	of the local user.
*
*   BUGS
*
*   SEE ALSO
*
*
******************************************************************************
*
*/

#include <sys/types.h>
#include <exec/types.h>
#include <exec/exec.h>
#include <dos/dosextens.h>
#include <dos/bptr.h>
#include <exec/memory.h>
#include <proto/dos.h>
#include <proto/exec.h>

#include <dos/dos.h>
#include <string.h>
#include "lharc.h"

static int GetMyFib(BPTR lock, struct FileInfoBlock *inf);
static int lock_stat(BPTR l, struct stat *buf);
static void fill_in_buf(struct FileInfoBlock *fp, struct stat *buf);

//extern int __io2errno(int);

static long blockSize;
/*
int stat(const char *name, struct stat *buf)
{
  BPTR l;
  int success;

  //Chk_Abort();

  if ((l = Lock(name, ACCESS_READ)) == 0)
  {
    //errno = __io2errno(IoErr());
    return -1;
  }

  success = lock_stat(l, buf);
  UnLock(l);

  return success;
}
*/
static void fill_in_buf(struct FileInfoBlock *fp, struct stat *buf)
{
  long
    mode = 0;
  long
    prot = fp->fib_Protection;

  bzero(buf, sizeof(struct stat));

  buf->st_mtime = fp->fib_Date.ds_Days * ((24 * 60) * 60) +
    fp->fib_Date.ds_Minute * 60 +
    fp->fib_Date.ds_Tick / TICKS_PER_SECOND;
  buf->st_atime = buf->st_mtime;
  buf->st_size = fp->fib_Size;
  buf->st_blksize = blockSize;
  buf->st_blocks = fp->fib_NumBlocks;
  buf->st_uid = fp->fib_OwnerUID;
  buf->st_gid = fp->fib_OwnerGID;

  if (!(prot & FIBF_READ))
    mode |= 0400;
  if ((prot & FIBF_GRP_READ))
    mode |= 0040;
  if ((prot & FIBF_OTR_READ))
    mode |= 0004;

  if (!(prot & FIBF_WRITE))
    mode |= 0200;
  if ((prot & FIBF_GRP_WRITE))
    mode |= 0020;
  if ((prot & FIBF_OTR_WRITE))
    mode |= 0002;

  if (!(prot & FIBF_EXECUTE))
    mode |= 0100;
  if ((prot & FIBF_GRP_EXECUTE))
    mode |= 0010;
  if ((prot & FIBF_OTR_EXECUTE))
    mode |= 0001;

  if (fp->fib_DirEntryType > 0)
    mode |= S_IFDIR;
  else
    mode |= S_IFREG;

  buf->st_mode = mode;
  buf->st_nlink = 1;		/* Hard coded */
}

static int lock_stat(BPTR l, struct stat *buf)
{
  struct FileInfoBlock *fp;

  fp = (struct FileInfoBlock *) AllocVec(sizeof(struct FileInfoBlock), MEMF_CLEAR);

  if (fp == NULL)
  {
    errno = ENOMEM;
    return -1;
  }

  if (GetMyFib(l, fp) == 0)
  {
    FreeVec(fp);
    return -1;
  }

  fill_in_buf(fp, buf);

  FreeVec(fp);

  return 0;
}

static int GetMyFib(BPTR lock, struct FileInfoBlock *inf)
{
  struct MsgPort *dev;
  LONG result;
  struct InfoData id;

  if (lock != 0)
  {
    //dev = btod(lock, struct FileLock *)->fl_Task;
  }
  else
  {
    dev = (struct MsgPort *) (((struct Process *) FindTask(0))->pr_FileSystemTask);
  }

  result = DoPkt(dev, ACTION_EXAMINE_OBJECT, lock, MKBADDR(inf), 0, 0, 0);
  if (!result)
  {
    //errno = __io2errno(IoErr());
    return 0;
  }

  memset(&id, 0, sizeof(struct InfoData));

  result = DoPkt(dev, ACTION_INFO, lock, MKBADDR(&id), NULL, NULL, NULL);

  if (result)
    blockSize = id.id_BytesPerBlock;
  else
    blockSize = 512;	/* assume */

  return 1;
}
