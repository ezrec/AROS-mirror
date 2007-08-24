/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/cybergraphics.h>
#include <proto/feelin.h>

#include <exec/memory.h>
#include <cybergraphx/cybergraphics.h>
#include <libraries/feelin.h>

/****************************************************************************
*** Shared Variables ********************************************************
****************************************************************************/

#define       LayersBase            FeelinBase -> Layers
#define       GfxBase               FeelinBase -> Graphics
#define       IntuitionBase         FeelinBase -> Intuition
#define       UtilityBase           FeelinBase -> Utility
extern struct Library              *CyberGfxBase;

extern struct ClassUserData        *CUD;

/***************************************************************************/
/*** Structures ************************************************************/
/***************************************************************************/

struct ClassUserData
{
    FObject                         Arbitrer;
    struct FeelinTraceLayer        *TraceLayers;
};

struct LocalObjectData
{
    FObject                         Application;        // Valid between FM_Setup / FM_Cleanup
    FObject                         Display;
    FObject                         Window;             // Valid between FM_Setup / FM_Cleanup
    struct RastPort                *RPort;              // Valid between FM_Show  / FM_Hide
    FPalette                       *Palette;            // Set at will
    bits32                          Flags;

/* end of public data */

    int32                           ForbidNest;
};

/*** clipping ***/

struct FeelinTraceLayer
{
    struct FeelinTraceLayer        *Next;
    struct Layer                   *Layer;
    struct FeelinList               TraceClipList;
};

struct FeelinTraceClip
{
    struct FeelinTraceClip         *Next;
    struct FeelinTraceClip         *Prev;
    struct Region                  *Region;
    struct Region                  *PreviousRegion;
};

