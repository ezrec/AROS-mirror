/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/datatypes.h>
#include <proto/intuition.h>
#include <proto/feelin.h>

#include <exec/memory.h>
#include <graphics/gfx.h>
#include <dos/dosextens.h>
#include <datatypes/datatypes.h>
#include <datatypes/pictureclass.h>
#include <intuition/screens.h>
#include <libraries/feelin.h>

#include <feelin/coremedia.h>

#define DOSBase                     FeelinBase -> DOS
#define GfxBase                     FeelinBase -> Graphics
#define IntuitionBase               FeelinBase -> Intuition
#define UtilityBase                 FeelinBase -> Utility

extern struct ClassUserData        *CUD;

/****************************************************************************
*** Attributes, Methods and Values ******************************************
****************************************************************************/

enum    {

        FA_Picture_Name

        };

/****************************************************************************
*** Class *******************************************************************
****************************************************************************/

struct ClassUserData
{
    FObject                         arbitrer;

    struct FeelinSharedPixels      *shared_pixels;
};


/*

A SharedPicture is created / obtained by the Picture.SharedObtain()  method
and deleted / released by the Picture.SharedRelease() method.

A SharedPicture is the raw representation of a  Picture,  independant  from
the  environnement.  The  struct  hold  general information such as picture
width, height, aspect informations and a user count (as raw representations
are shared).

A SharedPicture  may  have  different  RastPort  relative  representations,
called FeelinBitMap.

*/

struct FeelinSharedPixels
{
    struct FeelinSharedPixels      *next;
    char                            path[1024];

    uint32                          count;

    FCorePicturePublic              core_picture_public;
};

/*

FeelinBitMap is  the  representation  of  a  FeelinSharedPicture  whitin  a
specific   context   (relative   to   a  Screen).  As  FeelinSharedPicture,
FeelinBitMap can be shared within the same context.

* /

struct FeelinSharedBitMap
{
    struct FeelinSharedBitMap      *Next;
    struct Screen                  *Screen;     // Screen which the BitMap depends on
    struct BitMap                  *BitMap;     // Representation of the BitMap depending on Screen
    APTR                            Data;       // either a Datatype object or a DrawHandle (when GuiGFX is requested by the user)
    ULONG                           UserCount;
};

*/

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct LocalObjectData
{
    struct FeelinSharedPixels      *shared_pixels;
    //struct FeelinSharedBitMap      *SMap;
};

