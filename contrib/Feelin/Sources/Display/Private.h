/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/cybergraphics.h>
#include <proto/feelin.h>

#include <exec/execbase.h>
#include <exec/memory.h>
#include <dos/notify.h>
#include <cybergraphx/cybergraphics.h>
#include <libraries/feelin.h>

/****************************************************************************
*** Shared Variables ********************************************************
****************************************************************************/

#define DOSBase                     FeelinBase -> DOS
#define GfxBase                     FeelinBase -> Graphics
#define IntuitionBase               FeelinBase -> Intuition
#define UtilityBase                 FeelinBase -> Utility

extern struct ClassUserData        *CUD;

/****************************************************************************
*** Meta ********************************************************************
****************************************************************************/

struct FeelinObjectList
{
    FObject                         Head;
    FObject                         Tail;
};

struct ClassUserData
{
    FObject                         Semaphore;

    APTR                            ColorPool;
    APTR                            PalettePool;

    struct FeelinObjectList         DisplayList;

    FObject                         XMLScheme;
    FDynamicEntry                  *XMLIDs;
};

enum    {

        FA_Document_Source,
        FA_Document_SourceType,
        FM_Document_AddIDs,
        FM_Document_Resolve,
        FA_XMLDocument_Markups

        };

/***************************************************************************/
/*** Object ****************************************************************/
/***************************************************************************/

enum    {

        dummy_New,
        dummy_Dispose,
        dummy_Get,
        dummy_AddMember,
        dummy_RemMember,

        FM_Display_Find,
        FM_Display_Create,
        FM_Display_Delete,
        
        dummy_AddColor,
        dummy_RemColor,
        dummy_AddPalette,
        dummy_RemPalette,
        
        FM_Display_DecodeColor,

        dummy_CreateColor,
        dummy_CreateColorScheme,

        FM_Display_PixelRead,
        FM_Display_PixelWrite

        };

enum    {

        FA_Display_Background,
        FA_Display_ColorMap,
        FA_Display_Depth,
        FA_Display_BitMap,
        FA_Display_DrawInfo,
        FA_Display_Height,
        FA_Display_Name,
        FA_Display_Screen,
        FA_Display_ScreenMode,
        FA_Display_Spec,
        FA_Display_Title,
        FA_Display_Width

        };

#define FF_Display_Created          (1L << 0)

struct LocalObjectData
{
    bits32                          Flags;
    uint32                          UserCount;

    FObject                         Next;
    FObject                         Prev;

    struct Screen                  *Screen;
    struct ColorMap                *ColorMap;
    struct DrawInfo                *DrawInfo;
    FList                           ColorList;
    FList                           PaletteList;

    uint16                          Width;      // SA_Width
    uint16                          Height;     // SA_Height
    uint16                          Depth;      // SA_Depth
    uint16                          Type;       // FA_Display_Type

    STRPTR                          PublicName; // SA_PubName
    STRPTR                          ScreenName; // SA_Title

//    ULONG                           DisplayID;  // SA_DisplayID
//    struct TextFont                *Font;
};

/***************************************************************************/
/*** FeelinColor ***********************************************************/
/***************************************************************************/

struct in_FeelinColor
{
    struct in_FeelinColor          *Next;
    struct in_FeelinColor          *Prev;
// end of FeelinNode header
    uint32                          Pen;
    uint32                          ARGB;
    uint32                          UserCount;
    bits32                          Flags;
};

#define FF_Color_Shared             (1L << 0)

/***************************************************************************/
/*** FeelinPalette *********************************************************/
/***************************************************************************/

struct in_FeelinPalette
{
    struct in_FeelinPalette        *Next;
    struct in_FeelinPalette        *Prev;
// end of FeelinNode header
    uint32                          UserCount;
    uint32                          ColorCount;
    uint32                         *Pens;
    uint32                         *ARGB;
    struct in_FeelinColor         **Colors;
// array of PensCount * sizeof (ULONG) -> Pens
// array of PensCount * sizeof (ULONG) -> ARGB
// array of PensCount * sizeof (ULONG) -> Colors
};

#define MakeXGrad(orgb1,orgb2) ((((((0x00FF0000 & orgb1) >> 16) + ((0x00FF0000 & orgb2) >> 16)) / 2) << 16) | (((((0x0000FF00 & orgb1) >> 8) + ((0x0000FF00 & orgb2) >> 8)) / 2) << 8) | (((((0x000000FF & orgb1) >> 0) + ((0x000000FF & orgb2) >> 0)) / 2) << 0))

