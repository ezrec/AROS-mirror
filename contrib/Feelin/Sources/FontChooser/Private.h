/*** Includes **************************************************************/

#include <stdlib.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/diskfont.h>
#include <proto/feelin.h>

#include <libraries/diskfont.h>
#include <libraries/feelin.h>

#include "availfont.h"

#define       DOSBase               FeelinBase -> DOS
#define       GfxBase               FeelinBase -> Graphics
#define       UtilityBase           FeelinBase -> Utility

extern struct ClassUserData        *CUD;

/*** Structures ************************************************************/

struct ClassUserData
{
    struct FeelinAvailFontHeader   *AvailFontHeader;
    struct Hook                     name_display_hook;
    struct Hook                     size_construct_hook;
    struct Hook                     size_destruct_hook;
    struct Hook                     size_display_hook;
};

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;

    FObject                         name_list;
    FObject                         size_list;
    FObject                         name_string;
    FObject                         size_string;
    FObject                         preview;

    STRPTR                          spec;
};

/*** Dynamic ***************************************************************/

enum    {                                       // Attributes

        FA_FontChooser_Spec,
        FA_FontChooser_Choosed

        }; 

enum    {

        /* private */

        FM_FontChooser_ActivateName,
        FM_FontChooser_ActivateSize,
        FM_FontChooser_ActivateBoth

        };

struct  FS_FontChooser_ActivateName             { uint32 Pos; };
struct  FS_FontChooser_ActivateSize             { uint32 Pos; };
        
enum    {

        FM_List_Remove,
        FM_List_GetEntry,
        FM_List_InsertSingle,

        FA_List_Quiet

        }; // Auto

