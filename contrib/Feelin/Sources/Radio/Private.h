/*** Includes **************************************************************/

#include <proto/feelin.h>

#include <libraries/feelin.h>

/*** Structures ************************************************************/

struct LocalObjectData
{
    uint16                          Active;
    uint16                          Num;
    struct FeelinRadio_Couple      *Couples;
//  preferences
    STRPTR                          p_PreParse;
};

struct FeelinRadio_Couple
{
    FObject                       Image;
    FObject                       Text;
};

#define DEF_RADIO_HSPACING                      5
#define DEF_RADIO_HSPACING                      5

/***************************************************************************/
/*** Dynamics **************************************************************/
/***************************************************************************/

enum    {

        FA_Radio_Active,
        FA_Radio_Entries,
        FA_Radio_PreParse,
        FA_Radio_EntriesType

        };

