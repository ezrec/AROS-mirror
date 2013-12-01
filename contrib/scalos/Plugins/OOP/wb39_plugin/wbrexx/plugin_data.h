// plugin_data.h
// $Date$
// $Revision$

#ifndef PLUGIN_DATA_H_INCLUDED
#define PLUGIN_DATA_H_INCLUDED

#include <defs.h>
#include <Year.h>

#define PLUGIN_TYPE		OOP

#define	LIB_VERSION		39
#define	LIB_REVISION		22

#define LIB_NAME 		"wbrexx.plugin"
#define LIB_VERSTRING 		"$VER: wbrexx.plugin " \
				STR(LIB_VERSION) "." STR(LIB_REVISION) \
				" (20.12.2009) " COMPILER_STRING \
				" ©1999" CURRENTYEAR " The Scalos Team"

#define CI_PRIORITY 		-79
#define CI_CLASSNAME 		"IconWindow.sca"
#define CI_SUPERCLASSNAME 	"IconWindow.sca"
#define CI_PLUGINNAME 		"Scalos WBRexx Class"
#define CI_DESCRIPTION 		"Workbench ARexx support" COMPILER_STRING
#define CI_AUTHOR 		"Jürgen Lachmann"
#define CI_HOOKFUNC		myHookFunc

//----------------------------------------------------------------------------
// code and includes to define the structs and functions used above

#include "wbrexx.h"

#endif /* PLUGUN_DATA_H_INCLUDED */

