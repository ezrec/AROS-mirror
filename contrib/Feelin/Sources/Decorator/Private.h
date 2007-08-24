/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/feelin.h>

#include <libraries/feelin.h>
#include <feelin/preference.h>

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct FeelinDGroup
{
    struct FeelinDGroup            *next;
    struct FeelinDGroup            *prev;

    FObject                         object;
    STRPTR                          name;
    STRPTR                          classname;
    uint32                          position;
};

struct p_LocalObjectData
{
    FObject                         grp_edit;
    FObject                         chooser;

    FObject                         moduleslist;
    FList                           dglist;

    STRPTR                         *cycle_array;

    FObject                         active_decorator;
};

