#include <string.h>

#include <libraries/feelin.h>
#include <exec/memory.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/feelin.h>

#define DOSBase                     FeelinBase -> DOS
#define GfxBase                     FeelinBase -> Graphics
#define IntuitionBase               FeelinBase -> Intuition
#define UtilityBase                 FeelinBase -> Utility

extern struct ClassUserData        *CUD;

/*** Structures ************************************************************/

struct ClassUserData
{
    APTR                            Pool;
};

typedef struct in_FeelinTD_Color
{
    struct in_FeelinTD_Color       *Next;
    struct in_FeelinTD_Color       *Prev;
 
    STRPTR                          Spec;
    uint16                          Type;
    uint16                          Pen;
    FColor                         *Color;
}
FTDColor;

enum    {

        FV_TD_COLOR_PEN,
        FV_TD_COLOR_RGB,
        FV_TD_COLOR_SPEC,

        };

///FTDContextFont
typedef struct in_FeelinTD_Context_Font
{

    STRPTR                          face;
    uint8                           size;
    FTDColor                       *color;

   /* setup data */
    
    uint8                           setup_size;
    uint8                           setup_size_type;

    struct TextFont                *setup_font;
}
FTDContextFont;

enum    {

        FV_TD_CONTEXT_FONT_SIZE_SAME,
        FV_TD_CONTEXT_FONT_SIZE_PIXELS,
        FV_TD_CONTEXT_FONT_SIZE_PERCENT

        };
//+
///FTDContextPens
typedef struct in_FeelinTD_Context_Pens
{
   FTDColor                        *down;
   FTDColor                        *light;
   FTDColor                        *shadow;
   FTDColor                        *text;
   FTDColor                        *up;
}
FTDContextPens;

enum  {

      FV_TD_CONTEXT_PENS_DOWN,
      FV_TD_CONTEXT_PENS_LIGHT,
      FV_TD_CONTEXT_PENS_SHADOW,
      FV_TD_CONTEXT_PENS_TEXT,
      FV_TD_CONTEXT_PENS_UP,

      FV_TD_CONTEXT_PENS_COUNT

      };
//+

///FTDChunk
struct in_FeelinTD_Chunk
{
    struct in_FeelinTD_Chunk       *Next;
    struct in_FeelinTD_Chunk       *Prev;

    uint16                          Type;
    bits16                          Flags;                  // free to be used
    uint16                          Width;
    uint16                          Height;
};

typedef struct in_FeelinTD_Chunk                FTDChunk;

enum    {

        FV_TD_CHUNK_TEXT = 1,
        FV_TD_CHUNK_IMAGE

        };

#define _td_chunk_type                          ((FTDChunk *)(chunk)) -> Type
#define _td_chunk_flags                         ((FTDChunk *)(chunk)) -> Flags
#define _td_chunk_width                         ((FTDChunk *)(chunk)) -> Width
#define _td_chunk_height                        ((FTDChunk *)(chunk)) -> Height
//+
///FTDChunkText

struct in_FeelinTD_Chunk_Text
{
    FTDChunk                        Header;

    STRPTR                          text;
    uint16                          chars;

    uint8                           style;
    uint8                           mode;

    uint32                          hold;

    FTDContextFont                 *font;
    FTDContextPens                 *pens;
};

typedef struct in_FeelinTD_Chunk_Text           FTDChunkText;

#define FF_TD_CHUNK_TEXT_SHORTCUT               (1 << 0)


#define FF_TD_CHUNK_TEXT_HOLD_FONT              (1 << 0)
#define FF_TD_CHUNK_TEXT_HOLD_PENS              (1 << 1)

#define _td_chunk_text                          ((FTDChunkText *)(chunk))
#define _td_chunk_text_text                     _td_chunk_text -> text
#define _td_chunk_text_chars                    _td_chunk_text -> chars
#define _td_chunk_text_hold                     _td_chunk_text -> hold
#define _td_chunk_text_hold_font                (FF_TD_CHUNK_TEXT_HOLD_FONT & _td_chunk_text_hold)
#define _td_chunk_text_font                     _td_chunk_text -> font

/* flags must be in pens order, easier to resolve */
//+
///FTDChunkImage

struct in_FeelinTD_Chunk_Image
{
    FTDChunk                        Header;

    STRPTR                          spec;
    uint16                          w;
    uint16                          h;
    FObject                         object;
};

typedef struct in_FeelinTD_Chunk_Image          FTDChunkImage;

#define _td_chunk_image                         ((FTDChunkImage *)(chunk))
#define _td_chunk_image_spec                    _td_chunk_image -> spec
#define _td_chunk_image_w                       _td_chunk_image -> w
#define _td_chunk_image_h                       _td_chunk_image -> h
#define _td_chunk_image_object                  _td_chunk_image -> object
//+

///FTDHRule

struct in_TD_HRule
{
    uint8                           align;
    int8                            compact;
    int8                            noshade;
    uint8                           shine;
    uint8                           shadow;
    uint8                           size;
    uint8                           width;          // percentage or pixels

    uint8                           spacing;        // pixel before and after the separation
    int8                            percent;        // if TRUE, width is a percentage, otherwise pixels
};

typedef struct in_TD_HRule                      FTDHRule;

enum    {

        FV_TD_HR_CENTER,
        FV_TD_HR_LEFT,
        FV_TD_HR_RIGHT

        };
//+

///FTDLine

struct in_FeelinTD_Line
{
    struct in_FeelinTD_Line        *Next;
    struct in_FeelinTD_Line        *Prev;

    /* end of FeelinNode header */

    uint16                          Width;
    uint16                          Height;

    struct FeelinList               ChunkList;

    uint8                           align;
    uint8                           spacing;            // number of pixels between two lines
    uint8                           baseline;

    uint16                          adjust_width;       // restricted width of the line
    uint16                          adjust_chars;       // restricted number of character printable
    uint16                          adjust_image;       // restricted amount of space used by images

    uint8                           space_width;
    uint8                           space_loss;

    FTDHRule                       *hr;                 // if line ends with an horizontal rule, the rule's structure is attached here
};

enum    {

        FV_TD_LINE_ALIGN_LEFT,
        FV_TD_LINE_ALIGN_CENTER,
        FV_TD_LINE_ALIGN_RIGHT,
        FV_TD_LINE_ALIGN_JUSTIFY

        };

typedef struct in_FeelinTD_Line                 FTDLine;

//+

struct LocalObjectData
{
    STRPTR                          Prep;
    STRPTR                          Text;

    FRender                        *Render;

    struct TextFont                *Font;
    struct FeelinList               LineList;
    FList                           ColorsList;

    uint16                          Width;
    uint16                          Height;
    uint16                          limit_w;
    uint16                          limit_h;

    bits8                           Flags;
    uint8                           Shortcut;
};

#define FF_TD_SHORTCUT                          (1 << 0)
#define FF_TD_EXTRA_TOPLEFT                     (1 << 1)
#define FF_TD_EXTRA_BOTTOMRIGHT                 (1 << 2)
#define FV_TD_NOLIMIT                           0xFFFF

///td_line_adjust

struct in_TD_Adjust
{
    struct TextExtent               te;
    struct TextFont                *font;

    uint16                          limit_w;
    uint16                          limit_h;
};

typedef struct in_TD_Adjust                     FTDAdjust;

void td_line_adjust(FTDLine *Line,FTDAdjust *Adjust);

//+

/*** Private ***************************************************************/

uint32  td_text_extent  (struct TextFont *Font,STRPTR Str,uint32 Count,struct TextExtent *te);

