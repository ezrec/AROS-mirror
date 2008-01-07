#include <aros/symbolsets.h>

#include <proto/exec.h>
#include <string.h>
#include "base.h"

/****************************************************************************/

struct Library         *lib_base;

struct Catalog         *lib_cat = NULL;
ULONG                  lib_flags = 0;

struct MUI_CustomClass *lib_shape = NULL;
struct MUI_CustomClass *lib_class = NULL;

/****************************************************************************/

ADD2OPENLIB(initBase, 0);
ADD2CLOSELIB(freeBase, 0);

/****************************************************************************/
