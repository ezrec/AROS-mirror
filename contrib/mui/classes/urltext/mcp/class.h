#ifndef _CLASS_H
#define _CLASS_H

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/locale.h>

#include <clib/alib_protos.h>
#include <clib/debug_protos.h>

#include <mui/muiundoc.h>
#include <dos.h>
#include <string.h>

#include "Urltext_mcc_private.h"
#include "msg.h"
#include "base.h"

/***********************************************************************/

#define SysBase         (UrltextBase->sysBase)
#define DOSBase         (UrltextBase->dosBase)
#define IntuitionBase   (UrltextBase->intuitionBase)
#define UtilityBase     (UrltextBase->utilityBase)
#define LocaleBase      (UrltextBase->localeBase)
#define OpenURLBase     (UrltextBase->openURLBase)
#define MUIMasterBase   (UrltextBase->muiMasterBase)

extern struct UrltextBase *UrltextBase;
extern char libName[];
extern char author[];

/***********************************************************************/

#include "class_protos.h"

/***********************************************************************/

#endif /* _CLASS_H */
