#ifndef	PCHG_H
#define	PCHG_H
/*
**	$Filename: iff/pchg.h $
**	$Revision$
**
** Definitions for the PCHG chunk
*/

#define	ID_PCHG		MAKE_ID('P','C','H','G')

/* Compression modes */

#define PCHG_COMP_NONE 0
#define PCHG_COMP_HUFFMANN 1

/* Flags */

#define PCHGB_12BIT			(0)
#define PCHGB_32BIT			(1)
#define PCHGB_USE_ALPHA		(2)


#define PCHGF_12BIT			(1<<0)	/* Use SmallLineChanges */
#define PCHGF_32BIT			(1<<1)	/* Use BigLineChanges	*/
#define PCHGF_USE_ALPHA		(1<<2)	/* Meaningful only of PCHGB_32BIT is on: use the Alpha
													channel info */

/* Structures for the PCHG chunk */

struct PCHGHeader {
   UWORD Compression;
   UWORD Flags;
   WORD  StartLine;
   UWORD LineCount;
   UWORD ChangedLines;
   UWORD MinReg;
   UWORD MaxReg;
   UWORD MaxChanges;
   ULONG TotalChanges;
};

struct PCHGCompHeader {
   ULONG CompInfoSize;
   ULONG OriginalDataSize;
};

struct SmallLineChanges {
   UBYTE ChangeCount16;
   UBYTE ChangeCount32;
/*   UWORD PaletteChange[];*/
};

struct BigLineChanges {
   UWORD ChangeCount;
/*   struct BigPaletteChange PaletteChange[];*/
};

struct BigPaletteChange {
   UWORD Register;
   UBYTE Alpha, Red, Blue, Green;
};

/* Errors */

#define PCHGERR_NOMEM			(-1)
#define PCHGERR_UNKNOWNCOMP	(-2)

VOID /*  __regargs */ PCHG_CFastDecomp(APTR Source, APTR Dest, WORD *TreeCode, ULONG OriginalSize);
UBYTE * /* __regargs */ PCHG_CompHuffmann(APTR Source, ULONG SourceSize, ULONG *DataSize, ULONG *TreeSize);
VOID /* __regargs */ PCHG_SetUserCopList(WORD Offset, UWORD Length, struct ViewPort *ViewPort, struct PCHGHeader *PCHG, APTR LineMask, APTR LineData);
LONG /* __regargs */ PCHG_ParsePCHG(struct PCHGHeader *PCHG, struct ViewPort *ViewPort);
struct PCHGHeader * /* __regargs */ PCHG_SHAM2PCHG(UWORD *SHAMChunk, ULONG SHAMSize, WORD Increment);

#endif
