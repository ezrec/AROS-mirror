#ifndef _BLOCKSTRUCTURE_H
#define _BLOCKSTRUCTURE_H

#include <exec/types.h>

/* a BLCK is a block number.  Blocksize in bytes = SectorSize * SectorsPerBlock.

   BLCK pointers are used throughout the filesystem.  All structures which
   require to associate themselves with another structure do so using BLCK
   pointers.  Byte pointers are not used. */

typedef unsigned long BLCK;
typedef BLCK BLCKn;


/* Below is the standard block header.  This header is found before EVERY
   type of block used in the filesystem, except data blocks.

   The id field is used to check if the block is of the correct type when
   it is being referred to using a BLCK pointer.

   The checksum field is the SUM of all LONGs in a block plus one, and then
   negated.  When applying a checksum the checksum field itself should be
   set to zero.  To check to see if the checksum is okay, the sum of all
   longs in a block should be zero.

   The ownblock BLCK pointer points to the block itself.  This field is an
   extra safety check to ensure we are using a valid block. */

struct fsBlockHeader {
  ULONG id;         /* 4 character id string of this block */
  ULONG checksum;   /* The checksum */
  BLCK  ownblock;   /* The blocknumber of the block this block is stored at */
};



/* Now follows the structure of the Boot block.  The Boot block is always
   located at block offset 0.  It contains only a version number at the
   moment to identify the block structure of the disk. */

/*
struct fsBootBlock {
  struct fsBlockHeader bheader;

  UWORD version;                   // Version number of the filesystem block structure
};
*/

#define STRUCTURE_VERSION (3)



/* The fsRootInfo structure has all kinds of information about the format
   of the disk. */

struct fsRootInfo {
  ULONG deletedblocks;             /* Amount in blocks which deleted files consume. */
  ULONG deletedfiles;              /* Number of deleted files in recycled. */
  ULONG freeblocks;                /* Cached number of free blocks on disk. */

  ULONG datecreated;

  BLCK  lastallocatedblock;        /* Block which was most recently allocated */
  BLCK  lastallocatedadminspace;   /* AdminSpaceContainer which most recently was used to allocate a block */
  ULONG lastallocatedextentnode;   /* ExtentNode which was most recently created */
  ULONG lastallocatedobjectnode;   /* ObjectNode which was most recently created */

  BLCK  rovingpointer;
};



/* An SFS disk has two Root blocks, one located at the start of
   the partition and one at the end.  On startup the fs will check
   both Roots to see if it is a valid SFS disk.  If either one is
   missing SFS can still continue.

   A Root block could be missing on purpose.  For example, if you
   extend the partition at the end (adding a few MB's) then SFS
   can detect this with the information stored in the Root block
   located at the beginning (since only the end-offset has changed).
   Same goes for the other way around, as long as you don't change
   start and end point at the same time.

   When a Root block is missing because the partition has been
   made a bit larger, then SFS will in the future be able to
   'resize' itself without re-formatting the disk.

   Note: ownblock pointers won't be correct anymore when start of
         partition has changed... */

struct fsRootBlock {
  struct fsBlockHeader bheader;

  UWORD version;              /* Version number of the filesystem block structure */
  UWORD sequencenumber;       /* The Root with the highest sequencenumber is valid */

  ULONG datecreated;          /* Creation date (when first formatted).  Cannot be changed. */
  UBYTE bits;                 /* various settings, see defines below. */
  UBYTE pad1;
  UWORD pad2;

  ULONG reserved1[2];

  ULONG firstbyteh;           /* The first byte of our partition from the start of the */
  ULONG firstbyte;            /* disk.  firstbyteh = upper 32 bits, firstbyte = lower 32 bits. */

  ULONG lastbyteh;            /* The last byte of our partition, excluding this one. */
  ULONG lastbyte;

  BLCK  totalblocks;          /* size of this partition in blocks */
  ULONG blocksize;            /* blocksize used */

  ULONG reserved2[2];
  ULONG reserved3[8];

  BLCK  bitmapbase;           /* location of the bitmap */
  BLCK  adminspacecontainer;  /* location of first adminspace container */
  BLCK  rootobjectcontainer;  /* location of the root objectcontainer */
  BLCK  extentbnoderoot;      /* location of the root of the extentbnode B-tree */
  BLCK  objectnoderoot;       /* location of the root of the objectnode tree */

  ULONG reserved4[3];
};

#define ROOTBITS_CASESENSITIVE (128)   /* When set, filesystem names are treated case
                                          insensitive. */
#define ROOTBITS_RECYCLED      (64)    /* When set, files being deleted will first be
                                          moved to the Recycled directory. */

struct fsExtentBNode {
  ULONG key;     /* data! */
  ULONG next;
  ULONG prev;
  UWORD blocks;  /* The size in blocks of the region this Extent controls */
};

#endif // _BLOCKSTRUCTURE_H
