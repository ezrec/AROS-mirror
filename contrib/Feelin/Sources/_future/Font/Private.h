/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/ttengine.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>

extern struct FeelinBase           *FeelinBase;

#define       DOSBase               FeelinBase -> DOS
#define       LayersBase            FeelinBase -> Layers
#define       GfxBase               FeelinBase -> Graphics
#define       IntuitionBase         FeelinBase -> Intuition
#define       UtilityBase           FeelinBase -> Utility

extern struct Library              *TTEngineBase;

/****************************************************************************
*** Attributes, Methods, Values, Flags **************************************
****************************************************************************/

/****************************************************************************
*** Structures **************************************************************
****************************************************************************/

struct LocalObjectData
{
};

/****************************************************************************
*** Preferences *************************************************************
****************************************************************************/

struct p_LocalObjectData
{
};
