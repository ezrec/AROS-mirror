#ifndef DISKFONT_INTERN_H
#define DISKFONT_INTERN_H

#undef	AROS_ALMOST_COMPATIBLE
#define AROS_ALMOST_COMPATIBLE

#ifndef   PROTO_EXEC_H
#	include  <proto/exec.h>
#endif
#ifndef		EXEC_LIBRARIES_H
#	include <exec/libraries.h>
#endif
#ifndef   DISKFONT_DISKFONT_H
#	include  <diskfont/diskfont.h>
#endif
#ifndef   EXEC_MEMORY_H
#	include  <exec/memory.h>
#endif
#ifndef   EXEC_LISTS_H
#	include  <exec/lists.h>
#endif
#ifndef		AROS_ASMCALL_H
#	include	<aros/asmcall.h>
#endif
#ifndef		UTILITY_HOOKS_H
#	include <utility/hooks.h>
#endif



/**************/
/*	      */
/* AVAILFONTS */
/*	      */
/**************/


/**************/
/* Constants  */
/**************/

/* Number of font hooks used */
#define NUMFONTHOOKS 2

/* Flags for the FontInfoNode->Flags field */


#define FDF_REUSENAME   (1 << 0)
#define FDF_REUSETAGS   (1 << 1)

/* Possible returnvalues for AvailFonts hooks
( or an or'ed combination of these )
*/

#define FH_SUCCESS	(1 << 0)
#define FH_SCANFINISHED (1 << 1)

#define FH_REUSENAME	(1 << 2)
#define FH_REUSETAGS	(1 << 3)

/* Non-fatal error */
#define FH_SINGLEERROR	(1 << 4)


/* The different AvailFonts hook commands */
	
#define FHC_AF_INIT         	0
#define FHC_AF_READFONTINFO 	1
#define FHC_AF_CLEANUP      	2
#define FHC_AF_GETDATE      	3
#define FHC_ODF_INIT			4
#define FHC_ODF_CLEANUP			5
#define FHC_ODF_GETMATCHINFO	6
#define FHC_ODF_OPENFONT		7

/* ID for cache-file */
#define CACHE_IDSTR "AROS_FC"

/* Path to the cachefile */
#define CACHE_FILE "SYS:Fonts/cachefile"
#define FONTSDIR "SYS:Fonts/"
/* Structure for storing TAvailFonts elements */
struct FontInfoNode
{
    struct MinNode NodeHeader;
  
    struct TAvailFonts  TAF;
    
    /* or-ed combo of FDF_REUSENAME and FDF_REUSETAGS  */
    UBYTE   Flags;
  
    STRPTR 			NameInBuf;
    UWORD			NameLength; /* !!!! Includes 0 terminator */
    struct TagItem	*TagsInBuf;
	UWORD			NumTags;
};


/* Structure describing the hooks that shall be used
for reading font descriptions in AvailFonts()
*/

struct AFHookDescr
{
    /* Flags that must match the flags input to AvailFonts
    for this hook to be executed. (AFF_MEMORY, AFF_DISK, etc..)
    */

    ULONG	  ahd_Flags;
    struct Hook   ahd_Hook;
};



/* Hook structure message structure for sending data to
the font description reading routines */

struct FontHookCommand
{
    ULONG				fhc_Command;

	/* Used both by OpenDiskFont and AvailFonts for returning font descriptions from th hook */
	struct TTextAttr	fhc_DestTAttr;
	
	/* Used by OpenDiskFont only */
	struct TTextAttr 	*fhc_ReqAttr;
	struct TextFont		*fhc_TextFont;
	
    /* This field can be filled out by the hook. It will not be changed outside the hook */
    APTR				fhc_UserData;

};
struct FontDescrHeader
{
    UWORD   NumEntries;
    BOOL    Tagged;
    struct  TTextAttr *TAttrArray;
};




/* States we can be in if the buffer is filled */
#define BFSTATE_FONTINFO    0
#define BFSTATE_FONTNAME    1
#define BFSTATE_FONTTAGS    2


/* structure keepin the current state of copying fonts into the buffer */

struct CopyState
{
    /* What node was currently being written when buffer was full ? */
    struct FontInfoNode      *BufferFullNode;
    
    /* What state were we in when the buffer was full ? */
    UWORD		BufferFullState;

    /* Pointer into where we should start simulating copying into the buffer
    for reading bytes needed
    */
    APTR                BufferFullPtr;
};





/**************/
/* Prototypes */
/**************/

struct DiskfontBase_intern; /* prerefrence */

AROS_UFP3(IPTR, MemoryFontFunc,
    AROS_UFPA(struct Hook *,				h, 				A0),
    AROS_UFPA(struct FontHookCommand *,		fhc,			A2),
    AROS_UFPA(struct DiskfontBase_intern *,	DiskfontBase,	A1)
);

AROS_UFP3(IPTR, DiskFontFunc,
    AROS_UFPA(struct Hook *,				h, 				A0),
    AROS_UFPA(struct FontHookCommand *,		fhc,			A2),
    AROS_UFPA(struct DiskfontBase_intern *,	DiskfontBase,	A1)
);

BOOL  ScanFontInfo(ULONG, struct MinList *, struct DiskfontBase_intern *);


BOOL  CopyDescrToBuffer (UBYTE *, ULONG, ULONG, struct MinList *, struct CopyState *, struct DiskfontBase_intern *);
ULONG CountBytesNeeded  (UBYTE *, ULONG, struct CopyState *, 	struct DiskfontBase_intern *);
VOID  UpdatePointers    (UBYTE *, ULONG, struct MinList *, 		struct DiskfontBase_intern *);



struct TextFont *ReadDiskFont(struct TTextAttr *, struct DiskfontBase_intern *);
/* Functions for .font file I/O */
struct FontDescrHeader *ReadFontDescr(STRPTR,  struct DiskfontBase_intern *);
VOID FreeFontDescr(struct FontDescrHeader *,    struct DiskfontBase_intern *);

ULONG NumTags(struct TagItem *, struct DiskfontBase_intern *);
ULONG CopyTagItems(struct TagItem *, struct TagItem *, struct DiskfontBase_intern *);

struct TagItem *ReadTags  (BPTR, ULONG, struct DiskfontBase_intern *);
BOOL  WriteTags(BPTR, struct TagItem *, struct DiskfontBase_intern *);

BOOL ReadCache(ULONG, struct MinList *, struct DiskfontBase_intern *);
BOOL WriteCache(struct MinList *, struct DiskfontBase_intern *);

BOOL OKToReadCache(struct DiskfontBase_intern *);

struct FontInfoNode *AllocFIN(struct MinList *, struct DiskfontBase_intern *);
VOID FreeFIN(struct FontInfoNode *, struct DiskfontBase_intern *);
VOID FreeFontList(struct MinList *, struct DiskfontBase_intern *);

AROS_UFP3(LONG, dosstreamhook,
    AROS_UFPA(struct Hook *,   hook, A0),
    AROS_UFPA(BPTR,            file, A2),
    AROS_UFPA(ULONG *,         msg, A1)
);

/********************/
/* Some nice macros */
/********************/

#undef	AFH
#define AFH(p)  ((struct AvailFontsHeader *)p)

#undef	AVF
#define AVF(p)  ((struct AvailFonts       *)p)

#undef	TAVF
#define TAVF(p) ((struct TAvailFonts      *)p)

#undef	FIN
#define FIN(n)  ((struct FontInfoNode     *)n)

#undef	TI
#define TI(t)   ((struct TagItem          *)t)

#undef	UB
#define UB(p)   ((UBYTE                   *)p)

#undef	TFE
#define TFE(t)  ((struct TextFontExtension*)t)

/* Some external stuff (diskfont_init.c) */

/* Internal prototypes */


struct DiskfontBase_intern
{
    struct Library    library;
    /* struct ExecBase * sysbase; */
    BPTR	      seglist;

/*    struct Library	 * dosbase; */
    struct GfxBase	 * gfxbase;
    struct Library	 * utilitybase;

    /* dosstreamhandler hook neede for endian io funcs */
    struct Hook 	dsh;
    struct AFHookDescr hdescr[NUMFONTHOOKS];
};

/* The following typedefs are necessary, because the names of the global
   variables storing the library base pointers	and the corresponding
   structs are equal.
   This is a hack, of course. */
typedef struct GfxBase GraphicsBase;

#define DFB(dfb)        ((struct DiskfontBase_intern *)dfb)
/* #undef SysBase
#define SysBase (DFB(DiskfontBase)->sysbase) */
/* #undef DOSBase
#define DOSBase (DFB(DiskfontBase)->dosbase) */
#undef GfxBase
#define GfxBase (DFB(DiskfontBase)->gfxbase)
#undef UtilityBase
#define UtilityBase (DFB(DiskfontBase)->utilitybase)

#define expunge() \
AROS_LC0(BPTR, expunge, struct DiskfontBase_intern *, DiskfontBase, 3, Diskfont)


#endif /* diskfont_intern.h */
