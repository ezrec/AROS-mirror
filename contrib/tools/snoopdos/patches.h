/*
 *              PATCHES.H                                                                                       vi:ts=4
 *
 *      Copyright (c) Eddy Carroll, September 1994.
 *
 *              Contains definitions used by the patches module
 */

#ifdef __amigaos4__

#define reg_d0
#define reg_d1
#define reg_d2
#define reg_d3
#define reg_d4
#define reg_d5
#define reg_d6
#define reg_d7
#define reg_a0
#define reg_a1
#define reg_a2
#define reg_a3
#define reg_a4
#define reg_a5
#define reg_a6
#define __asm

#define REG_A7 15

#else

#define reg_d0  register __d0
#define reg_d1  register __d1
#define reg_d2  register __d2
#define reg_d3  register __d3
#define reg_d4  register __d4
#define reg_d5  register __d5
#define reg_d6  register __d6
#define reg_d7  register __d7
#define reg_a0  register __a0
#define reg_a1  register __a1
#define reg_a2  register __a2
#define reg_a3  register __a3
#define reg_a4  register __a4
#define reg_a5  register __a5
#define reg_a6  register __a6

#endif

typedef enum {
#ifdef __amigaos4__
		DISABLED = 0,
#endif
        DOS_LIB,
        EXEC_LIB,
        INTUITION_LIB,
        GRAPHICS_LIB,
        ICON_LIB,
        NUM_LIBS
} LibNumbers;

/*
 *              Now type definitions for all the functions to be replaced, so that we
 *              can easily call the original function from our replacement function.
 */
#define DFT(f)  typedef __asm ULONG (*FP_##f)   /* Define Function Type  */
#define LBASE   reg_a6 void *
typedef struct TagItem *TAGPTR;

LONG AttemptSemaphoreHeavely(struct SignalSemaphore *ss);

/*
 *              Dos prototypes
 */
DFT(AddDosEntry)        (reg_d1 struct DosList *, LBASE);
DFT(CurrentDir)         (reg_d1 BPTR,   LBASE);
DFT(DeleteFile)         (reg_d1 char *, LBASE);
DFT(Execute)            (reg_d1 char *, reg_d2 BPTR,    reg_d3 BPTR, LBASE);
DFT(GetVar)                     (reg_d1 char *, reg_d2 char *,  reg_d3 ULONG, reg_d4 ULONG,
                                         LBASE);
DFT(FindVar)            (reg_d1 char *, reg_d2 ULONG,   LBASE);
DFT(LoadSeg)            (reg_d1 char *, reg_d2 BPTR hunk, reg_d3 BPTR file, LBASE);
DFT(NewLoadSeg)         (reg_d1 char *, reg_d2 TAGPTR,  LBASE);
DFT(Lock)                       (reg_d1 char *, reg_d2 LONG,    LBASE);
DFT(Open)                       (reg_d1 char *, reg_d2 ULONG,   LBASE);
DFT(CreateDir)          (reg_d1 char *, LBASE);
DFT(MakeLink)           (reg_d1 char *, reg_d2 LONG,    reg_d3 LONG, LBASE);
DFT(Rename)                     (reg_d1 char *, reg_d2 char *,  LBASE);
DFT(RunCommand)         (reg_d1 BPTR,   reg_d2 ULONG,   reg_d3 char *,
                                         reg_d4 ULONG,  LBASE);
DFT(SetVar)                     (reg_d1 char *, reg_d2 char *,  reg_d3 ULONG, reg_d4 ULONG,
                                         LBASE);
DFT(DeleteVar)          (reg_d1 char *, reg_d2 ULONG,   LBASE);
DFT(SystemTagList)      (reg_d1 char *, reg_d2 TAGPTR,  LBASE);
                                
/*
 *              Exec prototypes
 */
DFT(FindPort)           (reg_a1 char *, LBASE);
DFT(FindResident)       (reg_a1 char *, LBASE);
DFT(FindResource)       (reg_a1 char *, LBASE);
DFT(FindSemaphore)      (reg_a1 char *, LBASE);
DFT(FindTask)           (reg_a1 char *, LBASE);
DFT(OpenDevice)         (reg_a0 char *, reg_d0 long,    reg_a1 struct IORequest *,
                                         reg_d1 long,   LBASE);
DFT(OpenLibrary)        (reg_a1 char *, reg_d0 long,    LBASE);
DFT(OpenResource)       (reg_a1 char *, LBASE);
DFT(GetMsg)                     (reg_a0 struct MsgPort *,               LBASE);
DFT(PutMsg)                     (reg_a0 struct MsgPort *, reg_a1 struct Message *, LBASE);

/*
 *              Graphics, Intuition and Icon library prototypes
 */
DFT(OpenFont)           (reg_a0 struct TextAttr *, LBASE);
DFT(LockPubScreen)      (reg_a0 char *,  LBASE);
DFT(FindToolType)       (reg_a0 char **, reg_a1 char *, LBASE);
DFT(MatchToolValue)     (reg_a0 char *,  reg_a1 char *, LBASE);

/*
 *              Dos library function offsets
 */
#define LVO_AddDosEntry         -678
#define LVO_CurrentDir          -126
#define LVO_DeleteFile          -72
#define LVO_Execute                     -222
#define LVO_GetVar                      -906
#define LVO_FindVar                     -918
#define LVO_LoadSeg                     -150
#define LVO_NewLoadSeg          -768
#define LVO_Lock                        -84
#define LVO_Open                        -30
#define LVO_CreateDir           -120
#define LVO_MakeLink            -444
#define LVO_Rename                      -78
#define LVO_RunCommand          -504
#define LVO_SetVar                      -900
#define LVO_DeleteVar           -912
#define LVO_SystemTagList       -606

/*
 *              Exec library function offsets
 */
#define LVO_FindPort            -390
#define LVO_FindResident        -96
#define LVO_FindSemaphore       -594
#define LVO_FindTask            -294
#define LVO_OpenDevice          -444
#define LVO_OpenLibrary         -552
#define LVO_OpenResource        -498
#define LVO_GetMsg                      -372
#define LVO_PutMsg                      -366

/*
 *              Graphics, Intuition and Icon library function offsets
 */
#define LVO_OpenFont            -72
#define LVO_LockPubScreen       -510
#define LVO_FindToolType        -96
#define LVO_MatchToolValue      -102
