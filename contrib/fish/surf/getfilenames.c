#include <aros/oldprograms.h>
#include <intuition/intuition.h>
#include "scrnio.ih"
#include <exec/memory.h>
#ifdef MANX
#include <functions.h>
#endif

#include "scrndef.h"
#include "mytypes.h"


#define StringSize 40
#define ROW 8
#define COL 8
#define TextX COL
#define TextY (ROW/2)
#define StringX 8
#define StringY 12
#define ReqSizeX 50*COL
#define ReqSizeY 6*ROW

#define CodeGo 100
#define CodeCancel 101

/*
 * declarations for a cancel button to be used by both
 * requestors.
 */
static struct IntuiText TextCancel = {
    1,-1,JAM1, 2, 1, NULL,(UBYTE *) "Cancel", NULL };

static short S_Cancel[] = {
    -2,-1,  -2,ROW+1,  6*COL+2,ROW+1,  6*COL+2,-1, -2,-1
 };

static struct Border B_Cancel = { 0, 0, 1, 0, JAM1, 5, S_Cancel, NULL };

static struct Gadget G_Cancel = {
    NULL,
    10*COL, ROW *4, 6*COL, ROW, /* loc and size of hit box */
    GADGHBOX,    /* complemented when pressed */
    RELVERIFY,    /* just get gadget up messages */
    BOOLGADGET | REQGADGET,
    (APTR)&B_Cancel, NULL,
    &TextCancel,
    0, NULL,
    (int)CodeCancel,
    NULL
 };

/*
 * String gadget to get ilbm filename
 */
static char OutTitle[] = { "output filename:" };
static char InTitle[] = { "input filename:" };

static struct IntuiText TextOutFile = {
    1,1,JAM1, TextX, TextY, NULL,
    (UBYTE *)OutTitle, NULL
 };
static struct IntuiText TextInFile = {
    1,1,JAM1, TextX, TextY, NULL,
    (UBYTE *)InTitle, NULL
 };


static char OutNameBuff[StringSize] = { "out.ilbm" };
static char InNameBuff[StringSize] =  { "in.ilbm" };
static char undo[StringSize];

static struct StringInfo S_OutFile = {
    (UBYTE *)OutNameBuff,
    (UBYTE *)undo,
    0,
    sizeof( OutNameBuff),
    0,
    0,
    13,
    0,
    0,0,NULL,0, NULL
};

static struct StringInfo S_InFile = {
    (UBYTE *)InNameBuff,
    (UBYTE *)undo,
    0,
    sizeof( InNameBuff),
    0,
    0,
    13,
    0,
    0,0,NULL,0, NULL
};

static short BD_InOut[] = {
    -2,-1,  -2, ROW,  (StringSize-1)*COL+1,ROW,
    (StringSize-1)*COL+1,-1, -2, -1
 };

static struct Border B_InOut = { 0, 0, 1, 0, JAM1, 5, BD_InOut, NULL };

static struct Gadget G_OutFile = {
    &G_Cancel,
    StringX , StringY,   /* loc */
    sizeof(OutNameBuff)*COL, ROW, /* size */
    GADGHCOMP,
    RELVERIFY /* | STRINGCENTER */,
    STRGADGET | REQGADGET,
    (APTR)&B_InOut, /* border */
    NULL, /* high lighted */
    &TextOutFile,
    0,
    (APTR) &S_OutFile,
    (int)CodeGo,
    NULL
 };


static struct Gadget G_InFile = {
    &G_Cancel,
    StringX , StringY,   /* loc */
    sizeof(InNameBuff)*COL, ROW, /* size */
    GADGHCOMP,
    RELVERIFY/* | STRINGCENTER */,
    STRGADGET | REQGADGET,
    (APTR)&B_InOut, /* border */
    NULL, /* high lighted */
    NULL, /* text */
    0,
    (APTR) &S_InFile,
    (int)CodeGo,
    NULL
 };

static struct Requester R_InFile = {
    NULL,
    COL*10, ROW*4, ReqSizeX, ReqSizeY,
    0, 0,
    &G_InFile,
    NULL,
    &TextInFile,
    NULL,
    2, /* backfill */
    NULL,
    {NULL},
    NULL,
    NULL,
    NULL,
};




static struct Requester R_OutFile = {
    NULL,
    COL*10, ROW*4, ReqSizeX, ReqSizeY,
    0, 0,
    &G_OutFile,
    NULL,
    &TextOutFile,
    NULL,
    2, /* backfill */
    NULL,
    { NULL },
    NULL,
    NULL,
    NULL,
};

static bool
WaitForUser() {
    struct IntuiMessage mycopy,
                        *orig;
    long wakeupmask;

    for(;;) {
        wakeupmask = Wait( 1<< CntrlWin->UserPort->mp_SigBit );

        /*
         * handle messages for the control window
         */

        while( (orig =(struct IntuiMessage *) GetMsg( CntrlWin->UserPort )) ) {

            mycopy = *orig;
            ReplyMsg( orig );

            if( mycopy.Class == GADGETUP ) {
                USHORT code;

                code = ((struct Gadget*)mycopy.IAddress)->GadgetID;
                if( code == CodeGo ) return( true );
                if( code == CodeCancel) return(false);
            }
        }
    }
}



char *GetInFile()
{
    bool answer;
    Request( &R_InFile, CntrlWin);
    answer = WaitForUser();
    EndRequest( &R_InFile, CntrlWin);
    return( answer?InNameBuff: NULL);
}


char *GetOutFile()
{
    bool answer;
    Request( &R_OutFile, CntrlWin);
    answer = WaitForUser();
    EndRequest( &R_OutFile, CntrlWin);
    return( answer?OutNameBuff: NULL);
}




