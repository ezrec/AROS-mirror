/*
#define _WIN_32
#define _UNIX
*/

/*
#define LITTLE_ENDIAN
*/

#define FALSE 0
#define TRUE  1

#ifdef _WIN_32

typedef unsigned char    UBYTE;
typedef unsigned short   UWORD;
typedef unsigned int     UDWORD;

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <dos.h>
#include <io.h>
#include <time.h>
#include <sys/stat.h>
#include <signal.h>
#include <dos.h>
#include <dir.h>
#include <dirent.h>
#include <share.h>

#define ENABLE_ACCESS

#define HOST_OS     WIN_32

#define FM_NORMAL   0x00
#define FM_RDONLY   0x01
#define FM_HIDDEN   0x02
#define FM_SYSTEM   0x04
#define FM_LABEL    0x08
#define FM_DIREC    0x10
#define FM_ARCH     0x20

#define PATHDIVIDER  "\\"
#define CPATHDIVIDER '\\'
#define MASKALL      "*.*"

#define READBINARY   "rb"
#define READTEXT     "rt"
#define UPDATEBINARY "r+b"
#define CREATEBINARY "w+b"
#define APPENDTEXT   "at"

#endif

#ifdef _UNIX

typedef unsigned char    UBYTE;
typedef unsigned short   UWORD;
typedef unsigned int     UDWORD;

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <signal.h>
#include <utime.h>

UDWORD UnixTimeToDos(time_t UnixTime);
time_t DosTimeToUnix(UDWORD DosTime);

#define ENABLE_ACCESS

#define HOST_OS     UNIX

#define FM_LABEL    0x0000
#define FM_DIREC    0x4000

#define PATHDIVIDER  "/"
#define CPATHDIVIDER '/'
#define MASKALL      "*.*"

#define READBINARY   "r"
#define READTEXT     "r"
#define UPDATEBINARY "r+"
#define CREATEBINARY "w+"
#define APPENDTEXT   "a"

#endif
