/*
	       unsit - Macintosh StuffIt file extractor

		     Version 1.5c, for StuffIt 1.5

			    August 3, 1989

This program will unpack a Macintosh StuffIt file into separate files.
The data fork of a StuffIt file contains both the data and resource
forks of the packed files.  The program will unpack each Mac file into
separate .data, .rsrc., and .info files that can be downloaded to a
Mac using macput.  The program is much like the "unpit" program for
breaking apart Packit archive files.

			***** IMPORTANT *****
To extract StuffIt files that have been compressed with the Lempel-Ziv
compression method, unsit pipes the data through the "compress"
program with the appropriate switches, rather than incorporate the
uncompression routines within "unsit".  Therefore, it is necessary to
have the "compress" program on the system and in the search path to
make "unsit" work.  "Compress" is available from the comp.sources.unix
archives.

The program syntax is much like unpit and macput/macget, with some added
options:

	unsit [-rdulvqfm] stuffit-file.data

The -r and -d flags will cause only the resource and data forks to be
written.  The -u flag will cause only the data fork to be written and
to have carriage return characters changed to Unix newline characters.
The -l flag will make the program only list the files in the StuffIt
file.  The -v flag causes the program to list the names, sizes, type,
and creators of the files it is writing.  The -q flag causes it to
list the name, type and size of each file and wait for a 'y' or 'n'
for either writing that file or skipping it, respectively.  The -m
flag is used when the input file in in the MacBinary format instead of
three separate .data, .info, and .rsrc files.  It causes the program
to skip the 128 byte MacBinary header before looking for the StuffIt
header.

Version 1.5 of the unsit supports extracting files and folders as
implemented by StuffIt 1.5's "Hierarchy Maintained Folder" feature.
Each folder is extracted as a subdirectory on the Unix system with the
files in the folder placed in the corresponding subdirectory.  The -f
option can be used to "flatten" out the hierarchy and unsit will store
all the files in the current directory.  If the query option (-q) is
used and a "n" response is given to a folder name, none of the files
or folders in that folder will be extraced.

Some of the program is borrowed from the macput.c/macget.c programs.
Many, many thanks to Raymond Lau, the author of StuffIt, for including
information on the format of the StuffIt archives in the
documentation.  Several changes and enhancements supplied by David
Shanks (cde@atelabs.UUCP) have been incorporated into the program for
doing things like supporting System V and recognizing MacBinary files.
I'm always glad to receive advice, suggestions, or comments about the
program so feel free to send whatever you think would be helpful


	Author: Allan G. Weber
		weber%brand.usc.edu@oberon.usc.edu
		...sdcrdcf!usc-oberon!brand!weber
	Date:   April 3, 1989

                              - - - - - - - - - -

                        Amiga port (Saturday 10-Aug-91)
                              mods by Anthon Pang

     1) '-u' option not supported (converting CR's to LF's in text files)
     2) prints compression id when unknown (during extract)
     3) '-l' option -- changed format of listings
     4) ".sit" extension is optional for "filename.sit", unless file without
        extension also exists (ie user should specify ".sit" explicitly)
     5) included version 5.3 of "compress"...ported by Matt Dillon for uucp
     6) changed extensions: ".data" -> ".d", ".rsrc" -> ".r", ".info" -> ".i"
        (mainly because ".info" is already in use on the Amiga for icons)
     7) some ANSI-cizing and reformatting

                     Addendum to Amiga Port (Sept 8, 1991)

     8) Corrected code for uncompressed lzw files, with spaces in names.
     9) Using popen, pclose emulation package.
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#ifdef __AROS__
#define SHORTNAMES
#include <exec/types.h>
#include <sys/stat.h>
#include <libraries/dosextens.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "unstuffit.h"

/*
 * The following defines the name of the compress program that is used for the
 * uncompression of Lempel-Ziv compressed files.  If the path is set up to
 * include the right directory, this should work.
 */
#define COMPRESS  "compress"

#define IOBUFSIZ  4096

#define MACBINHDRSIZE  128L

#define INIT_CRC  0L

#define INFOBYTES  128

#define BYTEMASK 0xff

#define S_SIGNATURE    0
#define S_NUMFILES     4
#define S_ARCLENGTH    6
#define S_SIGNATURE2  10
#define S_VERSION     14
#define SITHDRSIZE    22

#define F_COMPRMETHOD    0
#define F_COMPDMETHOD    1
#define F_FNAME          2
#define F_FTYPE         66
#define F_CREATOR       70
#define F_FNDRFLAGS     74
#define F_CREATIONDATE  76
#define F_MODDATE       80
#define F_RSRCLENGTH    84
#define F_DATALENGTH    88
#define F_COMPRLENGTH   92
#define F_COMPDLENGTH   96
#define F_RSRCCRC      100
#define F_DATACRC      102
#define F_HDRCRC       110
#define FILEHDRSIZE    112

#define F_NAMELEN  63
#ifdef SHORTNAMES
#ifdef __AROS__
#define I_NAMELEN  31 /* 30 char file names + '\0' terminator */
#else
#define I_NAMELEN  15 /* 14 char file names + '\0' terminator */
#endif
#else
#define I_NAMELEN  69 /* 63 + strlen (".info") + 1 */
#endif

/* The following are copied out of macput.c/macget.c */
#define I_NAMEOFF  1
/* 65 <-> 80 is the FInfo structure */
#define I_TYPEOFF  65
#define I_AUTHOFF  69
#define I_FLAGOFF  73
#define I_LOCKOFF  81
#define I_DLENOFF  83
#define I_RLENOFF  87
#define I_CTIMOFF  91
#define I_MTIMOFF  95

#define INITED_BUG
#define INITED_OFF  I_FLAGOFF /* offset to byte with Inited flag */
#define INITED_MASK  (~1)     /* mask to '&' with byte to reset it */

#define TEXT 0
#define DATA 1
#define RSRC 2
#define FULL 3
#define DUMP 4

#define NODECODE 0
#define DECODE   1

#define H_ERROR -1
#define H_EOF    0
#define H_WRITE  1
#define H_SKIP   2

struct node {
    int         flag, byte;
    struct node *one, *zero;
  } nodelist[512], *nodeptr, *read_tree(); /* 512 should be big enough */

struct sitHdr sithdr;

/* filenames */
char f_info[I_NAMELEN];
char f_data[I_NAMELEN];
char f_rsrc[I_NAMELEN];

char info[INFOBYTES];
char mname[F_NAMELEN+1];
char uname[F_NAMELEN+1];
char iobuf[IOBUFSIZ];

static char *CompStr[] = {
     "NOT", "RLE", "LZW", "HUF", "???"
};

int mode, txtmode, listonly, verbose, query, flatten;
int bit, chkcrc, numfiles, depth;
FILE *infp;


/* function prototypes */
extern unsigned short updcrc(unsigned short, unsigned char *, int);
#ifndef HAVE_GETOPT
extern int getopt(int argc, char *argv[], char *optstring);
#endif
void usage();
int extract(char *parent, int skip);
int extractfile(struct fileHdr *fh, int skip);
int readsithdr(struct sitHdr *s);
int readfilehdr(struct fileHdr *f, int skip);
check_access(char *fname);
unsigned short write_file(char *, long, long, unsigned char);
void outc(char *p, int n, FILE *fp);
long get4(char *bp);
short get2(char *bp);
void copy(char *p1, char *p2, int n);
struct node *read_tree();
int getbit();
int gethuffbyte(int decode);


void main(int argc, char *argv[]) {
    extern int optind;
    extern char *optarg;

    int status;
    int c;
    int errflg;
    int macbin;
    char temp[256];

    mode = FULL;
    errflg = 0;
    macbin = 0;
    flatten = 0;
    numfiles = 0;
    depth = 0;

    while ((c = getopt(argc, argv, "dflmqruvx")) != EOF)
      switch (c) {
        case 'r':
          mode = RSRC;
          break;
        case 'd':
          mode = DATA;
          break;
        case 'u':
          mode = TEXT;
          break;
        case 'l':
          listonly++;
          break;
        case 'q':
          query++;
          break;
        case 'v':
          verbose++;
          break;
        case 'x':        /* undocumented */
          mode = DUMP;
          break;
        case 'm':
          macbin = 1;
          break;
        case 'f':
          flatten = 1;
          break;
        case '?':
          errflg++;
          break;
      }

    if (errflg) {
      usage();
      exit(1);
    }

    if (optind == argc) {
      usage();
      exit(1);
    } else {
      if ((infp = fopen(argv[optind], "r")) == NULL) {
        sprintf(temp, "%s.sit", argv[optind]);
        if ((infp = fopen(temp, "r")) == NULL) {        
           fprintf(stderr,"Can't open input file \"%s\"\n",argv[optind]);
           exit(1);
        }
      }
    }

    if (macbin) {
      if (fseek(infp, MACBINHDRSIZE, 0) == -1) {
        fprintf(stderr, "Can't skip over MacBinary header\n");
        exit(1);
      }
    }

    if (readsithdr(&sithdr) == 0) {
      fprintf(stderr, "Can't read file header\n");
      exit(1);
    }
    if (listonly) {
      printf("---------- Data Fork ----------- --------- Resource Fork -------- --------\n"
             "Original Packed Ratio Type  CRC  Original Packed Ratio Type  CRC  Name\n"
             "-------- ------  ---  ----- ---- -------- ------  ---  ----- ---- --------\n");

    }
    /* 
    printf("numfiles=%d, arclength=%ld\n", sithdr.numFiles, sithdr.arcLength);
    */
 
    status = extract("", 0);
    exit((status < 0) ? 1 : 0);
}

void usage() {
     fprintf(stderr, "Usage: unsit [-rdulvqmf] filename\n");
}

/*
  extract(parent, skip) - Extract all files from the current folder.
  char *parent;           name of parent folder
  int  skip;              1 to skip all files and folders in this one
                          0 to extract them

  returns 1 if came an endFolder record
          0 if EOF
  -1 if error (bad fileHdr, bad file, etc.)
*/

int extract(char *parent, int skip) {
    struct fileHdr filehdr;
    struct stat sbuf;
    int status, rstat, sstat, skipit;
    char name[256];

    while (1) {
      rstat = readfilehdr(&filehdr, skip);
      if (rstat == H_ERROR || rstat == H_EOF) {
        status = rstat;
        break;
      }
 /*
 printf("compr=%d, compd=%d, rsrclen=%ld, datalen=%ld, rsrccrc=%d, datacrc=%d\n",
        filehdr.compRMethod, filehdr.compDMethod,
        filehdr.compRLength, filehdr.compDLength,
        filehdr.rsrcCRC, filehdr.dataCRC);
 */

      skipit = (rstat == H_SKIP) ? 1 : 0;

      if (filehdr.compRMethod == endFolder && 
        filehdr.compDMethod == endFolder) {
        status = 1;  /* finished with this folder */
        break;
      } else if (filehdr.compRMethod == startFolder && 
                 filehdr.compDMethod == startFolder) {
        if (!listonly && rstat == H_WRITE && !flatten) {
          sstat = stat(uname, &sbuf);
          if (sstat == -1) { /* directory doesn't exist */
            if (mkdir(uname, 0777) == -1) {
              fprintf(stderr, "Can't create subdirectory %s\n", uname);
              return(-1);
            }
          } else {  /* something exists with this name */
            fprintf(stderr, "Directory name %s already in use\n", uname);
            return(-1);
          }
#ifndef __AROS__
          if (chdir(uname) == -1) {
            fprintf(stderr, "Can't chdir to %s\n", uname);
            return(-1);
          }
#endif
          sprintf(name,"%s:%s", parent, uname);
        }
        depth++;
        status = extract(name, skipit);
        depth--;
        if (status != 1)
          break;  /* problem with folder */
        if (depth == 0) /* count how many top-level files done */
          numfiles++;
#ifndef __AROS__
        if (!flatten)
          chdir("..");
#endif
      } else {
        if ((status = extractfile(&filehdr, skipit)) != 1)
          break;
        if (depth == 0) /* count how many top-level files done */
          numfiles++;
      }
      if (numfiles == sithdr.numFiles)
        break;
    }
    return(status);
}

int extractfile(struct fileHdr *fh, int skip) {
    unsigned short crc;
    FILE *fp;

    f_data[0] = f_rsrc[0] = f_info[0] = '\0'; /* assume no output files */
    /* figure out what file names to use and what to do */
    if (!listonly && !skip) {
      switch (mode) {
        case FULL:  /* do both rsrc and data forks */
          sprintf(f_data, "%.*s.d", I_NAMELEN - 2, uname);
          sprintf(f_rsrc, "%.*s.r", I_NAMELEN - 2, uname);
          sprintf(f_info, "%.*s.i", I_NAMELEN - 2, uname);
          break;
        case RSRC:  /* rsrc fork only */
          sprintf(f_rsrc, "%.*s.r", I_NAMELEN - 2, uname);
          break;
        case DATA:  /* data fork only */
        case TEXT:
          sprintf(f_data, "%.*s", I_NAMELEN - 1, uname);
          break;
        case DUMP:  /* for debugging, dump data as is */
          sprintf(f_data, "%.*s.ddump", I_NAMELEN - 7, uname);
          sprintf(f_rsrc, "%.*s.rdump", I_NAMELEN - 7, uname);
          fh->compRMethod = fh->compDMethod = noComp;
          break;
      }
    }

    if (f_info[0] != '\0' && check_access(f_info) != -1) {
      fp = fopen(f_info, "w");
      if (fp == NULL) {
        perror(f_info);
        exit(1);
      }
      fwrite(info, 1, INFOBYTES, fp);
      fclose(fp);
    }

    if (f_rsrc[0] != '\0') {
      txtmode = 0;
      crc = write_file(f_rsrc, fh->compRLength, fh->rsrcLength, fh->compRMethod);
      if (chkcrc && fh->rsrcCRC != crc) {
        fprintf(stderr, "CRC error on resource fork: need 0x%04x, got 0x%04x\n", fh->rsrcCRC, crc);
        return(-1);
      }
    } else {
      fseek(infp, (long) fh->compRLength, 1);
    }

    if (f_data[0] != '\0') {
      txtmode = (mode == TEXT);
      crc = write_file(f_data, fh->compDLength, fh->dataLength, fh->compDMethod);
      if (chkcrc && fh->dataCRC != crc) {
        fprintf(stderr, "CRC error on data fork: need 0x%04x, got 0x%04x\n", fh->dataCRC, crc);
        return(-1);
      }
    } else {
      fseek(infp, (long) fh->compDLength, 1);
    }
    return(1);
}

int readsithdr(struct sitHdr *s) {
    char temp[FILEHDRSIZE];
    int count = 0;

    for (;;) {
      if (fread(temp, 1, SITHDRSIZE, infp) != SITHDRSIZE) {
        fprintf(stderr, "Can't read file header\n");
        return(0);
      }
     
      if (strncmp(temp + S_SIGNATURE,  "SIT!", 4) == 0 &&
               strncmp(temp + S_SIGNATURE2, "rLau", 4) == 0) {
        s->numFiles = get2(temp + S_NUMFILES);
        s->arcLength = get4(temp + S_ARCLENGTH);
        return(1);
      }
    
      if (++count == 2) {
        fprintf(stderr, "Not a StuffIt file\n");
        return(0);
      }
 
      if (fread(&temp[SITHDRSIZE], 1, FILEHDRSIZE - SITHDRSIZE, infp) !=
                                                FILEHDRSIZE - SITHDRSIZE) {
        fprintf(stderr, "Can't read file header\n");
        return(0);
      }
    
      if (strncmp(temp + I_TYPEOFF, "SIT!", 4) == 0 &&
          strncmp(temp + I_AUTHOFF, "SIT!", 4) == 0) { /* MacBinary format */
        fseek(infp, (long)(INFOBYTES-FILEHDRSIZE), 1); /* Skip over header */
      }
   }
}

/*
  readfilehdr - reads the file header for each file and the folder start
  and end records.

  returns: H_ERROR = error
    H_EOF   = EOF
    H_WRITE = write file/folder
    H_SKIP  = skip file/folder
*/

int readfilehdr(struct fileHdr *f, int skip) {
    unsigned short crc;
    int i, n, write_it, isfolder;
    char hdr[FILEHDRSIZE];
    char ch, *mp, *up;
    char *tp, temp[10];

    for (i = 0; i < INFOBYTES; i++)
      info[i] = '\0';

    /* read in the next file header, which could be folder start/end record */
    n = fread(hdr, 1, FILEHDRSIZE, infp);
    if (n == 0)   /* return 0 on EOF */
      return(H_EOF);
    else if (n != FILEHDRSIZE) {
      fprintf(stderr, "Can't read file header\n");
      return(H_ERROR);
    }

    /* check the CRC for the file header */
    crc = INIT_CRC;
    crc = updcrc(crc, (unsigned char *)hdr, FILEHDRSIZE - 2L);
    f->hdrCRC = get2(hdr + F_HDRCRC);
    if (f->hdrCRC != crc) {
      fprintf(stderr, "Header CRC mismatch: got 0x%04x, need 0x%04x\n", f->hdrCRC, crc);
      return(H_ERROR);
    }

    /* grab the name of the file or folder */
    n = hdr[F_FNAME] & BYTEMASK;
    if (n > F_NAMELEN)
      n = F_NAMELEN;
    info[I_NAMEOFF] = n;
    copy(info + I_NAMEOFF + 1, hdr + F_FNAME + 1, n);
    strncpy(mname, hdr + F_FNAME + 1, n);
    mname[n] = '\0';
    /* copy to a string with no illegal Unix characters in the file name */
    mp = mname;
    up = uname;
    while ((ch = *mp++) != '\0') {
#ifndef __AROS__
      if (ch <= ' ' || ch > '~' || index("/!()[]*<>?\\\"$\';&`", ch) != NULL)
        ch = '_';
#endif
      *up++ = ch;
    }
    *up = '\0';

    /* get lots of other stuff from the header */
    f->compRMethod = hdr[F_COMPRMETHOD];
    f->compDMethod = hdr[F_COMPDMETHOD];
    f->rsrcLength = get4(hdr + F_RSRCLENGTH);
    f->dataLength = get4(hdr + F_DATALENGTH);
    f->compRLength = get4(hdr + F_COMPRLENGTH);
    f->compDLength = get4(hdr + F_COMPDLENGTH);
    f->rsrcCRC = get2(hdr + F_RSRCCRC);
    f->dataCRC = get2(hdr + F_DATACRC);

    /* if it's an end folder record, don't need to do any more */
    if (f->compRMethod == endFolder && f->compDMethod == endFolder)
      return(H_WRITE);

    /* prepare an info file in case its needed */

    copy(info + I_TYPEOFF, hdr + F_FTYPE, 4);
    copy(info + I_AUTHOFF, hdr + F_CREATOR, 4);
    copy(info + I_FLAGOFF, hdr + F_FNDRFLAGS, 2);
#ifdef INITED_BUG
    info[INITED_OFF] &= INITED_MASK; /* reset init bit */
#endif
    copy(info + I_DLENOFF, hdr + F_DATALENGTH, 4);
    copy(info + I_RLENOFF, hdr + F_RSRCLENGTH, 4);
    copy(info + I_CTIMOFF, hdr + F_CREATIONDATE, 4);
    copy(info + I_MTIMOFF, hdr + F_MODDATE, 4);

    isfolder = f->compRMethod == startFolder && f->compDMethod == startFolder;
 
    /* list the file name if verbose or listonly mode, also if query mode */
    if (skip)   /* skip = 1 if skipping all in this folder */
      write_it = 0;
    else {
      write_it = 1;
      if (listonly || verbose || query) {
        for (i = 0; i < depth; i++)
          putchar(' ');

        if (isfolder) {
           printf("Folder: \"%s\"", uname);
        } else {
          if (listonly) {
            /* data fork, resource fork, name */
/*            
Original Packed Ratio Type  CRC  Original Packed Ratio Type  CRC  Name
-------- ------  ---  ----- ---- -------- ------  ---  ----- ---- ------------
*/
            printf("  %6d %6d %3d%%  -%3s- %04x ",
                   f->dataLength, f->compDLength,
                   (f->dataLength != 0) ? (100*f->compDLength)/f->dataLength : 0,
                   CompStr[ (f->compDMethod < 4) ? f->compDMethod : 4 ],
                   f->dataCRC);
            printf("  %6d %6d %3d%%  -%3s- %04x %s",
                   f->rsrcLength, f->compRLength,
                   (f->rsrcLength != 0) ? (100*f->compRLength)/f->rsrcLength : 0,
                   CompStr[ (f->compRMethod < 4) ? f->compRMethod : 4 ],
                   f->rsrcCRC, uname);
          } else {
              printf("name=\"%s\", type=%4.4s, author=%4.4s, data=%ld, rsrc=%ld",
              uname, hdr + F_FTYPE, hdr + F_CREATOR, f->dataLength, f->rsrcLength);
          }
        }
        if (query) { /* if querying, check with the boss */
          printf(" ? ");
          fgets(temp, sizeof(temp) - 1, stdin);
          tp = temp;
          write_it = 0;
          while (*tp != '\0') {
            if (*tp == 'y' || *tp == 'Y') {
              write_it = 1;
              break;
            } else
              tp++;
          }
        } else  /* otherwise, terminate the line */
          putchar('\n');
      }
    }
    return(write_it ? H_WRITE : H_SKIP);
}

/* return 0 if OK to write on file fname, -1 otherwise */
check_access(char *fname) {
    char temp[10], *tp;

    if (access(fname, 0) == -1) {
      return(0);
    } else {
      printf("%s exists.  Overwrite? ", fname);
      fgets(temp, sizeof(temp) - 1, stdin);
      tp = temp;
      while (*tp != '\0') {
        if (*tp == 'y' || *tp == 'Y') {
          return(0);
        } else
          tp++;
      }
    }
    return(-1);
}

unsigned short
write_file(char *fname, long ibytes, long obytes, unsigned char type) {
    unsigned short crc;
    int i, n, ch, lastch;
    FILE *outf;
    char temp[256];
#ifdef __AROS__
    BPTR lock;
#endif

    crc = INIT_CRC;
    chkcrc = 1;   /* usually can check the CRC */

    if (check_access(fname) == -1) {
      fseek(infp, ibytes, 1);
      chkcrc = 0;  /* inhibit crc check if file not written */
      return(-1);
    }
 
    switch (type) {
      case noComp:  /* no compression */
        outf = fopen(fname, "w");
        if (outf == NULL) {
          perror(fname);
          exit(1);
        }
        while (ibytes > 0) {
          n = (ibytes > IOBUFSIZ) ? IOBUFSIZ : ibytes;
          n = fread(iobuf, 1, n, infp);
          if (n == 0)
            break;
          crc = updcrc(crc, (unsigned char *)iobuf, n);
          outc(iobuf, n, outf);
          ibytes -= n;
        }
        fclose(outf);
        break;

      case rleComp:  /* run length encoding */
        outf = fopen(fname, "w");
        if (outf == NULL) {
          perror(fname);
          exit(1);
        }
        while (ibytes > 0) {
          ch = getc(infp) & 0xff;
          ibytes--;
          if (ch == 0x90) { /* see if its the repeat marker */
            n = getc(infp) & 0xff; /* get the repeat count */
            ibytes--;
            if (n == 0) { /* 0x90 was really an 0x90 */
              iobuf[0] = 0x90;
              crc = updcrc(crc, (unsigned char *)iobuf, 1);
              outc(iobuf, 1, outf);
            } else {
              n--;
              for (i = 0; i < n; i++)
                iobuf[i] = lastch;
              crc = updcrc(crc, (unsigned char *)iobuf, n);
              outc(iobuf, n, outf);
            }
          } else {
            iobuf[0] = ch;
            crc = updcrc(crc, (unsigned char *)iobuf, 1);
            lastch = ch;
            outc(iobuf, 1, outf);
          }
        }
        fclose(outf);
        break;

      case lzwComp:  /* LZW compression */

#ifdef __AROS__
       sprintf(temp, "%s%s%s%s", COMPRESS, " >\"", fname, "\" -d -c -n -b 14");
       /* sorry, we don't support txtmode yet */
#else
  	  sprintf(temp, "%s%s", COMPRESS, " -d -c -n -b 14 ");
	  if (txtmode) {
	      strcat(temp, "| tr \'\\015\' \'\\012\' ");
	      chkcrc = 0;		/* can't check CRC in this case */
	  }

	  strcat(temp, "> '");
	  strcat(temp, fname);
	  strcat(temp, "'");
#endif
	  outf = fopen(temp, "w");
	  if (outf == NULL) {
	    perror(fname);
	    exit(1);
	  }
	  while (ibytes > 0) {
	    n = (ibytes > IOBUFSIZ) ? IOBUFSIZ : ibytes;
	    n = fread(iobuf, 1, n, infp);
	    if (n == 0)
		break;
	    fwrite(iobuf, 1, n, outf);
	    ibytes -= n;
	  }
	  fclose(outf);
 	  if (chkcrc) {
	    outf = fopen(fname, "r"); /* read the file to get CRC value */
	    if (outf == NULL) {
		 perror(fname);
		 exit(1);
	    }
	    while (1) {
		 n = fread(iobuf, 1, IOBUFSIZ, outf);
		 if (n == 0)
		    break;
		 crc = updcrc(crc, (unsigned char *)iobuf, n);
	    }
	    fclose(outf);
	  }
	  break;

      case hufComp:  /* Huffman compression */
        outf = fopen(fname, "w");
        if (outf == NULL) {
          perror(fname);
          exit(1);
        }
        nodeptr = nodelist;
        bit = 0;  /* put us on a byte boundary */
        read_tree();
        while (obytes > 0) {
          n = (obytes > IOBUFSIZ) ? IOBUFSIZ : obytes;
          for (i = 0; i < n; i++)
            iobuf[i] = gethuffbyte(DECODE);
          crc = updcrc(crc, (unsigned char *)iobuf, n);
          outc(iobuf, n, outf);
          obytes -= n;
        }
        fclose(outf);
        break;

      default:
        fprintf(stderr, "Unknown/unsupported compression method: %d\n", type );
        chkcrc = 0;  /* inhibit crc check if file not written */
        return(-1);
    }

    return(crc & 0xffff);
}

void outc(char *p, int n, FILE *fp) {
    register char *p1;
    register int i;

    if (txtmode) {
      for (i = 0, p1 = p; i < n; i++, p1++)
        if ((*p1 & BYTEMASK) == '\r')
          *p1 = '\n';
    }
    fwrite(p, 1, n, fp);
}

long get4(char *bp) {
    register int i;
    long value = 0;

    for (i = 0; i < 4; i++) {
      value <<= 8;
      value |= (*bp & BYTEMASK);
      bp++;
    }
    return(value);
}

short get2(char *bp) {
    register int i;
    int value = 0;

    for (i = 0; i < 2; i++) {
      value <<= 8;
      value |= (*bp & BYTEMASK);
      bp++;
    }
    return(value);
}

void copy(char *p1, char *p2, int n) {
   while (n-- > 0)
     *p1++ = *p2++;
}

/* This routine recursively reads the Huffman encoding table and builds
   and decoding tree. */

struct node *read_tree()
{
   struct node *np;

   np = nodeptr++;
   if (getbit() == 1) {
      np->flag = 1;
      np->byte = gethuffbyte(NODECODE);
   } else {
      np->flag = 0;
      np->zero = read_tree();
      np->one  = read_tree();
   }
   return(np);
}


/* This routine returns the next bit in the input stream (MSB first) */
int getbit()
{
   static char b;

   if (bit == 0) {
      b = getc(infp) & 0xff;
      bit = 8;
   }
   bit--;
   return((b >> bit) & 1);
}

/* This routine returns the next 8 bits.  If decoding is on, it finds the
byte in the decoding tree based on the bits from the input stream.  If
decoding is not on, it either gets it directly from the input stream or
puts it together from 8 calls to getbit(), depending on whether or not we
are currently on a byte boundary
*/
int gethuffbyte(int decode) {
   register struct node *np;
   register int i, b;

   if (decode == DECODE) {
      np = nodelist;
      while (np->flag == 0)
         np = (getbit()) ? np->one : np->zero;
      b = np->byte;
   } else {
      if (bit == 0) /* on byte boundary? */
         b = getc(infp) & 0xff;
      else {  /* no, put a byte together */
         b = 0;
         for (i = 8; i > 0; i--) {
            b = (b << 1) + getbit();
         }
      }
   }
   return(b);
}
