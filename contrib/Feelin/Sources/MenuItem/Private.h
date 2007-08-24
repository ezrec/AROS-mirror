/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <libraries/feelin.h>

#include <proto/utility.h>
#include <proto/feelin.h>

#define       SysBase               FeelinBase -> SYS
#define       DOSBase               FeelinBase -> DOS
#define       LayersBase            FeelinBase -> Layers
#define       GfxBase               FeelinBase -> Graphics
#define       IntuitionBase         FeelinBase -> Intuition
#define       UtilityBase           FeelinBase -> Utility

/****************************************************************************
*** Attributes & Methods ****************************************************
****************************************************************************/

enum    {

        FA_Item_Title
/*
        FA_Item_Short,
        FA_Item_CommandString,
        FA_Item_Sensitive,
        FA_Item_Active,
        FA_Item_Selected,
        FA_Item_Check,
        FA_Item_Checked,
        FA_Item_Toggle,
        FA_Item_Enabled,
        FA_Item_Exclude
*/

        };

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct LocalObjectData
{
    FBox                            Box;
};

/***************************************************************************/
/*** Preferences ***********************************************************/
/***************************************************************************/
/*
struct p_LocalObjectData
{
};
*/

