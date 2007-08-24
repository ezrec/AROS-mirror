/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <devices/input.h>
#include <graphics/gels.h>
#include <libraries/feelin.h>
#include <feelin/preference.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/layers.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/feelin.h>

#define       DOSBase               FeelinBase -> DOS
#define       LayersBase            FeelinBase -> Layers
#define       GfxBase               FeelinBase -> Graphics
#define       IntuitionBase         FeelinBase -> Intuition
#define       UtilityBase           FeelinBase -> Utility

/****************************************************************************
*** Attributes & Methods ****************************************************
****************************************************************************/

enum    {   // methods

        FM_AppServer_DnDHandle

        };

struct  FS_AppServer_DnDHandle                  { FObject Source; FBox *Box; FRender *Render; };

enum    {   // attributes

        FA_AppServer_Preference

        };

enum    {   // actions

        FV_Thread_DnD = FV_Thread_Dummy,
        FV_Thread_Help

        };

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct LocalObjectData
{
    bits32                          Flags;

    FObject                         Server;
    FObject                         Thread;
    FObject                         Preference;

    struct FeelinDnDInfo           *DnDInfo; 

    // thread datas

    struct FeelinBase              *Feelin;

    struct Process                 *process;
    bits32                          signals;

    struct Interrupt                input_handler;
    struct MsgPort                 *input_port;
    struct IOStdReq                *input_request;

    // bubble help

    struct timeval                  tv;
    struct Screen                  *help_screen;
    int16                           help_mousex;
    int16                           help_mousey;
    FObject                         help_window;

    // misc

    uint32                          id_Send;
};

#define FF_APPSERVER_INITIATED                  (1 << 0)
#define FF_AppServer_Help                       (1 << 1)
#define FF_APPSERVER_DRAGGING                   (1 << 2)

/* FF_AppServer_Help is set the first time  the  method  FM_Window_Help  is
sent. This method will not be sent again, until the flag is cleared */

enum    {

        FV_Thread_Help_Show = FV_Thread_Dummy,
        FV_Thread_Help_Hide

        };

/****************************************************************************
*** Structures **************************************************************
****************************************************************************/

struct FeelinBob
{
    FBox                            box;        // current bob position and size

    struct RastPort                 rp;         // Rastport for our BOB
    struct RastPort                 brp;        // Rastport for our backup

    struct Region                  *region;

    struct RastPort                *srp;        // Source Rastport (bitmap to copy on screen)
    struct RastPort                *drp;        // Destination Rastport (screen)
    struct BitMap                  *backup;     // Bitmap for our backup
    struct BitMap                  *bitmap;     // Bitmap for our BOB
};

/*
struct FeelinBob
{
    UWORD                           Width;
    UWORD                           Height;
    
    struct RastPort                 MainRast;
    struct ViewPort                *MainView;
    
    struct VSprite                  vsprite;        // VSprite used for bob
    struct Bob                      bob;            // Data for a single bob
    struct RastPort                 rport;          // Rastport for our BOB
    struct BitMap                   bitmap;         // Bitmap for our BOB

    uint32                           chipsize;       // Total size of allocated CHIP
    BYTE                           *chipdata;       // Pointer to all CHIP RAM data

    struct GelsInfo                 gel_info;           // GelInfo for entire structure
    UWORD                           gel_nextline[8];    // Nextline data
    int32                            gel_lastcolor[8];   // Last colour data, PTR TO PTR TO INT
    struct collTable                gel_colltable;      // Collision table
    struct VSprite                  gel_vshead;         // Head sprite anchor
    struct VSprite                  gel_vstail;         // Tail sprite anchor
};
*/

typedef struct FeelinDnDInfo
{
    FObject                         Source;
    FObject                         Target;
    FBox                            SourceBox;
    FBox                            TargetBox;
    struct Window                  *SourceWin;
    struct Window                  *TargetWin;

    struct Screen                  *Scr;
    FRender                        *Render;
    FEventHandler                   Handler;
    
    UWORD                           msex,msey;
    UWORD                           offx,offy;
    
    struct FeelinBob               *Bob;
    FBox                            BobBox;
}
FDnDInfo;

/****************************************************************************
*** Prototypes **************************************************************
****************************************************************************/

/* bob.c */

#if 0
struct FeelinBob *  Bob_Create      (struct RastPort *RPort,struct ViewPort *VPort,uint32 Width,uint32 Height,int32 Transp);
#endif
struct FeelinBob *  Bob_Create      (struct RastPort *RPort, FBox * SourceBox, int32 Transp);
void                Bob_Delete      (struct FeelinBob *Bob);
void                Bob_Update      (struct FeelinBob *Bob, int32 x, int32 y);
void                Bob_Add         (struct FeelinBob *Bob);
void                Bob_Rem         (struct FeelinBob *Bob);
void                Bob_MakeGhost   (struct FeelinBob *Bob);
void                Bob_MakeSolid   (struct FeelinBob *Bob);
void                xblit           (struct BitMap *sbm, int32 sx, int32 sy, struct BitMap *dbm, int32 x, int32 y, int32 w, int32 h);

/****************************************************************************
*** Thread ******************************************************************
****************************************************************************/

extern F_THREAD_ENTRY_PROTO(Thread_Main);
