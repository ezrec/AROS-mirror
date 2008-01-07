#include <aros/symbolsets.h>

#include "base.h"
#include "class.h"

/****************************************************************************/

struct Library         *lib_base;
struct Catalog         *lib_cat = NULL;
ULONG                  lib_flags = 0;

Class                  *lib_sgad = NULL;
struct MUI_CustomClass *lib_boopsi = NULL;
struct MUI_CustomClass *lib_contents = NULL;
struct MUI_CustomClass *lib_mcc = NULL;

/****************************************************************************/

ADD2OPENLIB(initBase, 0);
ADD2CLOSELIB(freeBase, 0);

/****************************************************************************/
