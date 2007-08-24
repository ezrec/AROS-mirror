/*** Includes **************************************************************/

#include <stdlib.h>

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/iffparse.h>
#include <proto/feelin.h>

#include <libraries/iffparse.h>
#include <libraries/feelin.h>
#include <feelin/preference.h>

#define GfxBase                     FeelinBase -> Graphics
#define IntuitionBase               FeelinBase -> Intuition
#define UtilityBase                 FeelinBase -> Utility

/****************************************************************************
*** IDs *********************************************************************
****************************************************************************/

enum    {                                       // Attributes

        FA_String_Accept,
        FA_String_AdvanceOnCR,
        FA_String_Blink,
        FA_String_BlinkSpeed,
        FA_String_Changed,
        FA_String_Contents,
        FA_String_Cursor,
        FA_String_Format,
        FA_String_Integer,
        FA_String_Max,
        FA_String_Reject,
        FA_String_Secret,
        FA_String_TextActive,
        FA_String_TextBlock,
        FA_String_TextInactive,
        FA_String_AttachedList

        };

/****************************************************************************
*** Structures **************************************************************
****************************************************************************/

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;

    STRPTR                          String;
    STRPTR                          Undo;
    STRPTR                          Accept;
    STRPTR                          Reject;

    uint8                           Justify;
    uint8                           reserved1;
    uint16                          Flags;
    uint16                          Allocated;

    int16                           TextX;
    uint16                          TextW;
    int16                           CursorX;
    uint16                          CursorW;

    uint16                          Pos;
    uint16                          Cur;
    uint16                          Len;
    uint16                          Max;
    uint16                          Old;
    uint16                          Sel;
    uint16                          Dsp;
    uint8                           BlinkSpeed;
    uint8                           BlinkElapsed;

    FColor                         *APen;       // Active Pen
    FColor                         *BPen;       // Block Pen
    FColor                         *IPen;       // Inactive Pen

    FObject                         Cursor;     // Cursor ImageDisplayObject
    FObject                         Blink;      // Blink ImageDisplayObject
    FObject                         AttachedList;

    //APTR                            attached_list_notify_handler;
//  Preferences
    STRPTR                          p_ASpec;
    STRPTR                          p_BSpec;
    STRPTR                          p_ISpec;
    STRPTR                          p_Cursor;
    STRPTR                          p_Blink;
    STRPTR                          p_BlinkSpeed;
};

struct FeelinString_TextFit
{
    uint32                          Position;       // Read / Written
    STRPTR                          String;         // Read / Written
    uint16                          TextWidth;      // Read / Written
    uint16                          Displayable;
    BOOL                            Clip;
};

#define FF_String_Secret                        (1 << 0)
#define FF_String_AdvanceOnCR                   (1 << 1)
#define FF_String_Blink                         (1 << 2)

#define FF_Draw_Move                            FF_Draw_Custom_1
#define FF_Draw_Insert                          FF_Draw_Custom_2
#define FF_Draw_Delete                          FF_Draw_Custom_3
#define FF_Draw_Backspace                       FF_Draw_Custom_4
#define FF_Draw_Blink                           FF_Draw_Custom_5

enum    {

        REPAIR_NOTHING,
        REPAIR_ALL,
        REPAIR_OLDC,
        REPAIR_FROM_OLDC,
        REPAIR_WITHCUR,
        REPAIR_FROMCUR,
        REPAIR_TOCUR
        
        };

/*** Prototypes ************************************************************/

int32 String_Insert(FClass *Class,FObject Obj,uint8 Char);

/*** Preferences ***********************************************************/

#define DEF_STRING_CURSOR                       "highlight"
#define DEF_STRING_BLINK                        NULL
#define DEF_STRING_BLINK_SPEED                  0
#define DEF_STRING_TEXT_ACTIVE                  "dark"
#define DEF_STRING_TEXT_BLOCK                   "shadow"
#define DEF_STRING_TEXT_INACTIVE                "shine"
