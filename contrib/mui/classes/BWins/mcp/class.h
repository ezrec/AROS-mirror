#ifndef _CLASS_H
#define _CLASS_H

#define __USE_SYSBASE
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/locale.h>

#include <clib/alib_protos.h>

#ifndef __AROS__
#include <clib/debug_protos.h>

#include <mui/muiundoc.h>
#include <dos.h>
#endif

#include <string.h>

#include <BWin_private_mcc.h>

#include "base.h"
#define CATCOMP_NUMBERS
#include "loc.h"

/***********************************************************************/

extern char LIBNAME[];

extern STRPTR titlesStrings[];
extern STRPTR shapeStrings[];

/***********************************************************************/
/*
** MCCs macros
*/

#define ShapeObject NewObject(lib_shape->mcc_Class,NULL

/***********************************************************************/

#include "class_protos.h"

/***********************************************************************/

#endif /* _CLASS_H */
