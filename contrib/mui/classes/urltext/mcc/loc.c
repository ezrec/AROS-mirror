
#include "class.h"

/****************************************************************************/
/*
 * Strings definitions
 */

#define Msg_Send_STR            "Reach url..."
#define Msg_Copy_STR            "Copy to clipbord"
#define Msg_OpenURLPrefs_STR    "OpenURL prefs..."

/****************************************************************************/
/*
 * Code/String array definition
 */

struct message
{
    LONG   id;
    STRPTR string;
};

struct message messages[] =
{
    { Msg_Send,               Msg_Send_STR, },
    { Msg_Copy,               Msg_Copy_STR, },
    { Msg_OpenURLPrefs,       Msg_OpenURLPrefs_STR, },

    { NULL },
};

/****************************************************************************/

CONST_STRPTR ASM
getString(REG(d0) ULONG id)
{
    struct message *array;
    CONST_STRPTR    string;

    for (array = messages;
         array->string && (array->id!=id);
         array++);

    string = array->string;
    if (UrltextBase->cat) string = GetCatalogStr(UrltextBase->cat,id,string);

    return string;
}

/****************************************************************************/
