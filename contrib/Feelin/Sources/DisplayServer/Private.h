#include <stdarg.h>

#include <libraries/feelin.h>
#include <feelin/preference.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/layers.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/feelin.h>

/****************************************************************************
*** Shared Variables ********************************************************
****************************************************************************/

extern struct FeelinBase           *FeelinBase;
#define       DOSBase               FeelinBase -> DOS
#define       LayersBase            FeelinBase -> Layers
#define       GfxBase               FeelinBase -> Graphics
#define       IntuitionBase         FeelinBase -> Intuition
#define       UtilityBase           FeelinBase -> Utility

extern struct ClassUserData        *CUD;

/****************************************************************************
*** Meta ********************************************************************
****************************************************************************/

struct ClassUserData
{
    bits32                          Flags;
    bits32                          Signals;

    struct Process                 *process;

    FObject                         Server;
    FObject                         Thread;
    
    uint32                          id_Send;
};

/****************************************************************************
*** Structures **************************************************************
****************************************************************************/


