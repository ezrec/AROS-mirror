/* unsit.h: contains declarations for StuffIT (SIT) headers */

typedef long OSType;

typedef struct sitHdr {         /* 22 bytes */
     OSType         signature;  /* = 'SIT!' -- for verification */
     unsigned short numFiles;   /* number of files in archive */
     unsigned long  arcLength;  /* length of entire archive incl.
                                    hdr. -- for verification */
     OSType         signature2; /* = 'rLau' -- for verification */
     unsigned char  version;    /* version number */
     char           reserved[7];
} sitHdr;

typedef struct fileHdr { /* 112 bytes */
     unsigned char  compRMethod; /* rsrc fork compression method */
     unsigned char  compDMethod; /* data fork compression method */
     unsigned char  fName[64];   /* a STR63 */
     OSType         fType;       /* file type */
     OSType         fCreator;    /* er... */
     short          FndrFlags;   /* copy of Finder flags.  For our
                                    purposes, we can clear:
                                    busy, onDesk */
     unsigned long  creationDate;
     unsigned long  modDate;     /* !restored-compat w/backup prgms */
     unsigned long  rsrcLength;  /* decompressed lengths */
     unsigned long  dataLength;
     unsigned long  compRLength; /* compressed lengths */
     unsigned long  compDLength;
     unsigned short rsrcCRC;     /* crc of rsrc fork */
     unsigned short dataCRC;     /* crc of data fork */
     char           reserved[6];
     unsigned short hdrCRC;      /* crc of file header */
} fileHdr;


/* file format is:
     sitArchiveHdr
          file1Hdr
               file1RsrcFork
               file1DataFork
          file2Hdr
               file2RsrcFork
               file2DataFork
          .
          .
          .
          fileNHdr
               fileNRsrcFork
               fileNDataFork
*/


/* compression methods */
#define noComp  0 /* just read each byte and write it to archive */
#define rleComp 1 /* RLE compression */
#define lzwComp 2 /* LZW compression */
#define hufComp 3 /* Huffman compression */

/* proprietary compression methods...Deluxe Stuffit */
#define deluxe0 4 /* unsupported/unknown */
#define deluxe1 5 /* unsupported/unknown */


#define encrypted   16 /* bit set if encrypted. ex: encrypted+lpzComp */

#define startFolder 32 /* marks start of a new folder */
#define endFolder   33 /* marks end of the last folder "started" */

/* all other numbers are reserved */
