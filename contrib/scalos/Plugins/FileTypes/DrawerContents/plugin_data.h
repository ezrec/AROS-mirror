// plugin_data.h
// $Date$
// $Revision$

#ifndef PLUGIN_DATA_H_INCLUDED
#define PLUGIN_DATA_H_INCLUDED

#include <defs.h>
#include <Year.h>

#define PLUGIN_TYPE	FILETYPE

#define LIB_BASETYPE	struct DrawerContentsBase

#define	LIB_VERSION	40
#define	LIB_REVISION	7

#define LIB_NAME	"drawercontents.plugin"
#define LIB_VERSTRING   "$VER: drawercontents.plugin " \
			STR(LIB_VERSION) "." STR(LIB_REVISION)  " (30.09.2005)" \
			COMPILER_STRING " ©2003" CURRENTYEAR \
			" The Scalos Team"


#define FT_INFOSTRING		LIBToolTipInfoString
#define FT_INFOSTRING_AROS	PluginBase_0_LIBToolTipInfoString

//----------------------------------------------------------------------------
// code and includes to define the structs and functions used above

#include "DrawerContents_base.h"

#endif /* PLUGUN_DATA_H_INCLUDED */

