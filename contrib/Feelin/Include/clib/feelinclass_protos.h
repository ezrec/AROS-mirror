#ifndef CLIB_FEELINCLASS_PROTOS_H
#define CLIB_FEELINCLASS_PROTOS_H

/*
**    $VER: <clib/feelinclass_protos.h> 8.0 (2004/12/18)
**
**    C prototype definitions
**
**    © 2001-2004 Olivier LAVIALE (gofromiel@gofromiel.com)
*/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif
#ifndef FEELIN_TYPES_H
#include <feelin/types.h>
#endif

struct TagItem *        F_Query                 (ULONG Which,struct FeelinBase *Feelin);

#endif   /* CLIB_FEELINCLASS_PROTOS_H */
