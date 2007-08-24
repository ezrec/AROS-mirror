/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/feelinclass.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>

#define DOSBase                     FeelinBase -> DOS

/****************************************************************************
*** Attributes, Methods, Values, Flags **************************************
****************************************************************************/

enum    {

        FA_ModulesList_Path,
        FA_ModulesList_Fast,

        FA_ModulesList_Classes,
        FA_ModulesList_Prefs,
        FA_ModulesList_DecoratorPrefs

        };

/****************************************************************************
*** Types *******************************************************************
****************************************************************************/

typedef struct FeelinModuleNode
{
    struct FeelinModuleNode        *Next;
    struct FeelinModuleNode        *Prev;
    STRPTR                          Name;
    bits32                          Flags;
}
FModuleNode;

#define FF_MODULE_CLASS                         (1 << 0)
#define FF_MODULE_METACLASS                     (1 << 1)
#define FF_MODULE_PREFS                         (1 << 2)
#define FF_MODULE_METAPREFS                     (1 << 3)
#define FF_MODULE_DECORATORPREFS                (1 << 4)
#define FF_MODULE_METADECORATORPREFS            (1 << 5)

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct LocalObjectData
{
    STRPTR                          path;
    STRPTR                          fast;
    APTR                            pool;
    FList                           list;
    FList                           list_class;
    FList                           list_prefs;
    FList                           list_decoratorprefs;
    STRPTR                          buffer;
};

