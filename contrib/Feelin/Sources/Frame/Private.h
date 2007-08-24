/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <string.h>

#include <libraries/feelin.h>
#include <graphics/gfxmacros.h>

#include <proto/exec.h>
#include <proto/feelin.h>
#include <proto/graphics.h>
#include <proto/utility.h>

#define GfxBase                     FeelinBase -> Graphics
#define UtilityBase                 FeelinBase -> Utility

extern struct ClassUserData        *CUD;

/****************************************************************************
*** Methods & Attributes ****************************************************
****************************************************************************/
 
enum    {

        FV_Position_Left,
        FV_Position_Center,
        FV_Position_Right

        };

enum    {

        FA_Document_Source,
        FA_Document_SourceType,
        FM_Document_AddIDs,
        FM_Document_Resolve,

        FA_XMLDocument_Markups

        };

enum    {

        FV_XML_ID_FRAME = FV_XMLDOCUMENT_ID_DUMMY,
        FV_XML_ID_ID,
        FV_XML_ID_PADDING,
        FV_XML_ID_PADDING_WIDTH,
        FV_XML_ID_PADDING_HEIGHT,
        FV_XML_ID_PADDING_LEFT,
        FV_XML_ID_PADDING_RIGHT,
        FV_XML_ID_PADDING_TOP,
        FV_XML_ID_PADDING_BOTTOM

        };

/****************************************************************************
*** Types *******************************************************************
****************************************************************************/
 
struct ClassUserData
{
    FObject                         XMLDocument;
    FDynamicEntry                  *XMLIDs;
};

typedef struct FeelinFrameTitle
{
    STRPTR                          Title;
    STRPTR                          Prep;
    struct TextFont                *Font;
    FObject                         TD;
    int8                            Down;       // BOOL
    uint8                           Position;
    uint8                           FrameBorderH[2];
    uint16                          Height;
    uint16                          Width;
// Preferences
    STRPTR                          p_Font;
    STRPTR                          p_Prep;
    STRPTR                          p_Position;
}
FFrameTitle;

/* FFrameTitle is allocated only when needed (e.g. the frame has a  title),
otherwise, a simple FFrameData is used. */

struct LocalObjectData
{
    FFramePublic                    Public;
    FFrameTitle                    *TitleData;
    FInner                          UserInner;

    uint16                          ID[2];

// Preferences
    STRPTR                          p_frame;
    STRPTR                          p_back;
};

/* if 'Title' is != NULL 'Public' is a pointer to a FFrameTitle  structure,
otherwise, it's a pointer to a FFrameData structure */

