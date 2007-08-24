#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>
#include <exec/memory.h> 
#include <graphics/gfxmacros.h> 
#include <feelin/preference.h>
 
#define GfxBase                     FeelinBase -> Graphics 
#define IntuitionBase               FeelinBase -> Intuition 
#define UtilityBase                 FeelinBase -> Utility 

/***************************************************************************/
/*** Structures ************************************************************/
/***************************************************************************/

typedef struct FeelinPH_BubbleData
{
    FObject                         TD;
    struct Window                  *Win;        // Bubble Window
    struct TextFont                *Font;
    struct RastPort                 RPort;
    FRender                        *Render;
    FPalette                       *Scheme;
    APTR                            Pointer;    // Window Pointer Sprite

    uint16                          text_w;
    uint16                          text_h;
}                                                           FBubble;
 
struct LocalObjectData 
{ 
    STRPTR                          Text;
    FObject                         Window;
    FBubble                        *Bubble;
/* Preferences */ 
    STRPTR                          p_Font; 
    STRPTR                          p_Scheme;
}; 
 
/***************************************************************************/
/*** Values ****************************************************************/
/***************************************************************************/
 
#define HSPACE   8 
#define SHADOW   0

#define FV_BUBBLE_DATA_W                        20
#define FV_BUBBLE_DATA_H                        20
#define FV_ARROW_H                              16 // fake 20 in fact
#define FV_ARROW_REAL_H                         20
#define FV_ARROW_W                              20
#define FV_ARROW_OFF                            40 // pixels from the border of the bubble to the point of the arrow

enum {  FV_ARROW_NONE,
        FV_ARROW_TL,
        FV_ARROW_TR,
        FV_ARROW_BL,
        FV_ARROW_BR                             };

/***************************************************************************/
/*** Private ***************************************************************/ 
/***************************************************************************/
 
BOOL    PH_Open         (struct FeelinClass *Class,APTR Obj);
void    PH_Close        (struct FeelinClass *Class,APTR Obj);
void    PH_Draw         (struct FeelinClass *Class,APTR Obj,struct Screen *scr,UWORD x,UWORD y,UWORD w,UWORD h,UBYTE ArrowPosition);

/***************************************************************************/
/*** Dynamic ***************************************************************/ 
/***************************************************************************/
 
enum    {

        FA_PopHelp_Text,
        FA_PopHelp_Font,
        FA_PopHelp_Open,
        FA_PopHelp_Window,
        FA_PopHelp_Scheme

        };        

