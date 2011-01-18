/*
 *		PATCHES.H											vi:ts=4
 *
 *      Copyright (c) Eddy Carroll, September 1994.
 *
 *		Contains definitions used by the patches module
 */

typedef enum {
	DOS_LIB,
	EXEC_LIB,
	INTUITION_LIB,
	GRAPHICS_LIB,
	ICON_LIB,
	NUM_LIBS
} LibNumbers;

/*
 *		Now type definitions for all the functions to be replaced, so that we
 *		can easily call the original function from our replacement function.
 */
#define DFT(f)	typedef ASMCALL ULONG (*FP_##f)	/* Define Function Type  */
#define LBASE	REG(a6, void *)
typedef struct TagItem *TAGPTR;

/*
 *		Dos prototypes
 */
DFT(AddDosEntry)	(REG(d1, struct DosList *), LBASE);
DFT(CurrentDir)		(REG(d1, BPTR),  	LBASE);
DFT(DeleteFile)		(REG(d1, char *),	LBASE);
DFT(Execute)		(REG(d1, char *),	REG(d2, BPTR),	REG(d3, BPTR),	LBASE);
DFT(GetVar)			(REG(d1, char *),	REG(d2, char *),REG(d3, ULONG),	REG(d4, ULONG),
					 LBASE);
DFT(FindVar)		(REG(d1, char *),	REG(d2, ULONG),	LBASE);
DFT(LoadSeg)		(REG(d1, char *),	LBASE);
DFT(NewLoadSeg)		(REG(d1, char *),	REG(d2, TAGPTR),LBASE);
DFT(Lock)			(REG(d1, char *),	REG(d2, LONG), 	LBASE);
DFT(Open)			(REG(d1, char *),	REG(d2, ULONG),	LBASE);
DFT(CreateDir)		(REG(d1, char *),	LBASE);
DFT(MakeLink)		(REG(d1, char *),	REG(d2, LONG), 	REG(d3, LONG), LBASE);
DFT(Rename)			(REG(d1, char *),	REG(d2, char *),LBASE);
DFT(RunCommand)		(REG(d1, BPTR),		REG(d2, ULONG),	REG(d3, char *),
					 REG(d4, ULONG),LBASE);
DFT(SetVar)			(REG(d1, char *),	REG(d2, char *),REG(d3, ULONG), REG(d4, ULONG),
					 LBASE);
DFT(DeleteVar)		(REG(d1, char *),	REG(d2, ULONG),		LBASE);
DFT(SystemTagList)	(REG(d1, char *),	REG(d2, TAGPTR),	LBASE);
				
/*
 *		Exec prototypes
 */
DFT(FindPort)		(REG(a1, char *),	LBASE);
DFT(FindResident)	(REG(a1, char *),	LBASE);
DFT(FindResource)	(REG(a1, char *),	LBASE);
DFT(FindSemaphore)	(REG(a1, char *),	LBASE);
DFT(FindTask)		(REG(a1, char *),	LBASE);
DFT(OpenDevice)		(REG(a0, char *),	REG(d0, long),	REG(a1, struct IORequest *),
					 REG(d1, long),		LBASE);
DFT(OpenLibrary)	(REG(a1, char *),	REG(d0, long),	LBASE);
DFT(OpenResource)	(REG(a1, char *),	LBASE);
DFT(GetMsg)			(REG(a0, struct MsgPort *),			LBASE);
DFT(PutMsg)			(REG(a0, struct MsgPort *),			REG(a1, struct Message *), LBASE);

/*
 *		Graphics, Intuition and Icon library prototypes
 */
DFT(OpenFont)		(REG(a0, struct TextAttr *), LBASE);
DFT(LockPubScreen)	(REG(a0, char *),  LBASE);
DFT(FindToolType)	(REG(a0, char **), REG(a1, char *),	LBASE);
DFT(MatchToolValue)	(REG(a0, char *),  REG(a1, char *), LBASE);

/*
 *		Dos library function offsets
 */
#define LVO_AddDosEntry		-678
#define LVO_CurrentDir		-126
#define LVO_DeleteFile		-72
#define LVO_Execute			-222
#define LVO_GetVar			-906
#define LVO_FindVar			-918
#define LVO_LoadSeg			-150
#define LVO_NewLoadSeg		-768
#define LVO_Lock			-84
#define LVO_Open			-30
#define LVO_CreateDir		-120
#define LVO_MakeLink		-444
#define LVO_Rename			-78
#define LVO_RunCommand		-504
#define LVO_SetVar			-900
#define LVO_DeleteVar		-912
#define LVO_SystemTagList	-606

/*
 *		Exec library function offsets
 */
#define LVO_FindPort		-390
#define LVO_FindResident	-96
#define LVO_FindSemaphore	-594
#define LVO_FindTask		-294
#define LVO_OpenDevice		-444
#define LVO_OpenLibrary		-552
#define LVO_OpenResource	-498
#define LVO_GetMsg			-372
#define LVO_PutMsg			-366

/*
 *		Graphics, Intuition and Icon library function offsets
 */
#define LVO_OpenFont		-72
#define LVO_LockPubScreen	-510
#define LVO_FindToolType	-96
#define LVO_MatchToolValue	-102
