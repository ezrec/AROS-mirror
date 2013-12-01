// DrawerContents.h
// $Date$
// $Revision$

#ifndef __DRAWERCONTENTS_BASE_H
#define __DRAWERCONTENTS_BASE_H

#include <libraries/locale.h>

#include "plugin.h"

struct DrawerContentsBase
{
	struct PluginBase dcb_LibNode;

	struct	Locale *dcb_Locale;
	struct	Catalog *dcb_Catalog;
};

#endif // __DRAWERCONTENTS_BASE_H

