#ifndef AROS_FILEIO_H
#define AROS_FILEIO_H

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif

struct ArosFile
{
  BPTR          File;
  LONG          Type;
  UBYTE         Name[256];
  unsigned char *Data;
  int           Length;
  int           Offset;
  unsigned int  CRC;
};

#define FILETYPE_NORMAL 0
#define FILETYPE_TMP    1
#define FILETYPE_ZIP    2
#define FILETYPE_CUSTOM 3

extern char *nvdir, *hidir, *cfgdir, *inpdir, *stadir, *memcarddir;
extern char *artworkdir, *screenshotdir, *alternate_name;


#define ReadFile(a,b,c)   Read(a,b,c)
#define WriteFile(a,b,c)  Write(a,b,c)
#define SeekFile(a,b,c)   Seek(a,b,c)

#endif
