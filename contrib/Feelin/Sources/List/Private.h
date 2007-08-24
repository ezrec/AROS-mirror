#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>
#include <feelin/preference.h>

#include "Public.h"

#define       GfxBase               FeelinBase -> Graphics
#define       UtilityBase           FeelinBase -> Utility

/****************************************************************************
*** IDs *********************************************************************
****************************************************************************/

enum    {

        FM_List_Compare,
        FM_List_Construct,
        FM_List_Destruct,
        FM_List_Display,
        FM_List_GetEntry,
        FM_List_Insert,
        FM_List_InsertSingle,
        FM_List_Remove,
        FM_List_FindString

        };

enum    {

        FA_List_Active,                         // [ISG] LONG - get/set the number of the active entry
        FA_List_Activation,                     // [..G]
        FA_List_CompareHook,                    // [IS.] struct Hook *
        FA_List_ConstructHook,                  // [IS.] struct Hook *
        FA_List_DestructHook,                   // [IS.] struct Hook *
        FA_List_DisplayHook,                    // [IS.] struct Hook *
        FA_List_Entries,                        // [..G] LONG - Number of entries in the list
        FA_List_Visible,
        FA_List_First,                          // [..G] LONG - Get the number of the first visible entry
        FA_List_Last,                           // [..G] LONG - Get the number of the last visible entry
        FA_List_Format,                         // [ISG] STRPTR
        FA_List_Pool,                           // [I..] APTR
        FA_List_PoolItemNumber,                 // [I..] uint32
        FA_List_PoolItemSize,                   // [I..] uint32
        FA_List_Quiet,                          // [.S.] BOOL - Toggle the FF_List_Quiet flag
        FA_List_SortMode,                       // [IS.] LONG - Set automatic sort of inserted lines
        FA_List_SourceArray,                    // [I..] APTR *
        FA_List_Title,                          // [ISG] STRPTR
        FA_List_ReadOnly,
        
        /* preferences */
 
        FA_List_Steps,
        FA_List_Spacing,                        // [ISG] BYTE - Number of pixels between lines
        FA_List_CursorActive

        };

/****************************************************************************
*** Structures **************************************************************
****************************************************************************/

/*** FColumn ***************************************************************/
 
typedef struct FeelinList_Column
{
    bits8                           Flags;
    
    uint16                          Width;
    uint16                          AdjustedWidth;

    uint8                           Padding;
    uint8                           Weight;
/*
    UWORD                           Position;   // Column number
*/
}
FColumn;

#define FF_COLUMN_BAR                           (1 << 0)
#define FF_COLUMN_FIXED                         (1 << 1)
#define FF_COLUMN_PREVBAR                       (1 << 2) // previous columns has a bar
#define FF_COLUMN_PADLEFT                       (1 << 3)
#define FF_COLUMN_PADRIGHT                      (1 << 4)

/* AdjustedWidth is the Width adjusted to the available space.  This  width
is  not  modified if FF_COLUMN_FIXED is set. Note that the width of padding
is also added to AdjustedWidth */

/*** FLine *****************************************************************/

typedef struct FeelinList_Line
{
    struct FeelinList_Line         *Next;
    struct FeelinList_Line         *Prev;
// end of FeelinNode header
    APTR                            Entry;
    STRPTR                         *Strings;    // The array of string follow the header of the structure
    STRPTR                         *PreParses;
    uint16                         *Widths;     // The widths of the column
    uint16                          Height;     // Line Height;
    bits16                          Flags;      // see below
//    uint16                          Parents;    // number of entries parent's, used for the list tree stuff

    /*
    'Position' *MUST* be at the end of the structure because the  array  of
    line's  strings  follows.  The  array  is sent to the display hook thus
    array[-1] gives the line number.

    'Position' is followed by the array of Strings, the array of  PreParses
    and the array of Widths.
    */

    uint32                           Position;
}
FLine;

#define FF_LINE_COMPUTED                        (1 << 0)
#define FF_LINE_SELECTED                        (1 << 1)

/* FF_LINE_COMPUTED is  set  when  the  dimensions  of  a  line  have  been
computed.  The flag is used to avoid multiple computing of a same line. The
flag is cleared during FM_Cleanup. */

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;

    bits32                          Flags;

    APTR                            Pool;               // This Pool is used by the user to allocate Entry data.
    uint32                          PoolItemSize;
    uint32                          PoolItemNumber;
    APTR                            LinePool;           // This Pool is used to allocate entries

    struct Hook                    *Hook_Compare;
    struct Hook                    *Hook_Construct;
    struct Hook                    *Hook_Destruct;
    struct Hook                    *Hook_Display;

// TitleBar and Columns

    FLine                          *TitleBar;       // See list_create_format()
    FObject                         TitleFrame;
    FFramePublic                   *TitleFramePublic;
    FPalette                       *TitleScheme;
    FColumn                        *Columns;
    uint32                          ColumnCount;

// Lines

    FList                           LineList;       // List of FeelinList_Line nodes
    uint32                          LineCount;
    FLine                          *First;          // First visible Line
    FLine                          *Last;           // Last visible Line
    FLine                          *Active;         // Active Line
    uint32                          Visible;        // # of visible entries
    uint8                           Spacing;        // Number of pixel between each line
    uint16                          MaximumH;       // Maximun height of a line
    uint16                          TitleBarH;
    
    int16                           Quiet;          // nested, >= 0 update real time, < 0 nothing is displayed. == 0 complete refresh
    uint16                          SortMode;
    uint16                          Activation;
    uint16                          Steps;

// Graphics

    FObject                         TDObj;
    FPalette                       *Scheme;
    FObject                         CursorActive;
    FObject                         CursorSelected;

// Draw

    FLine                          *Update;         // Previous active Line that should be redrawn along the new one
    FLine                          *LastFirst;

//  preferences

    STRPTR                          p_Spacing;
    STRPTR                          p_Steps;
    STRPTR                          p_CursorActive;

    STRPTR                          p_title_back;
    STRPTR                          p_title_frame;
    STRPTR                          p_title_scheme;
};

#define FF_LIST_READONLY                        (1 << 0) // input
#define FF_LIST_OWNPOOL                         (1 << 1) // Set if Pool is allocated by the Class
#define FF_LIST_MODIFIED                        (1 << 2)
#define FF_LIST_ACTIVATION_INTERNAL             (1 << 3)
#define FF_LIST_TITLE_INHERITED_BACK            (1 << 4)

/* FF_LIST_MODIFIED is set when the list is in  quiet  mode  and  has  been
modified. This flag is used to avoid unnecessary drawing */

/***************************************************************************/

#define FF_Draw_Line                            FF_Draw_Custom_1 // draw line LOD -> Update, request erase

/****************************************************************************
*** Prototypes **************************************************************
****************************************************************************/

/*** support.c ***/

void line_display(FClass *Class,FObject Obj,FLine *Line);
void line_compute_dimensions(FClass *Class,FObject Obj,FLine *line);
void list_update(FClass *Class,FObject Obj,int32 Notify);
void list_adjust_first(FClass *Class,FObject Obj,uint32 pos);
                
/*** titlebar.c ***/

int32 titlebar_new(FClass *Class,FObject Obj,STRPTR Fmt);
void titlebar_dispose(FClass *Class,FObject Obj);
void titlebar_compute_dimensions(FClass *Class,FObject Obj);
void titlebar_adjust_dimensions(FClass *Class,FObject Obj);
int32 titlebar_setup(FClass *Class,FObject Obj);
void titlebar_cleanup(FClass *Class,FObject Obj);
void titlebar_draw(FClass *Class,FObject Obj);

#define DEF_CELL_FRAME                          "<frame id='42' padding-width='2' />"
