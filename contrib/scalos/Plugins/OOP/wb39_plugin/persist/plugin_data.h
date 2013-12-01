// plugin_data.h
// $Date$
// $Revision$

#ifndef PLUGIN_DATA_H_INCLUDED
#define PLUGIN_DATA_H_INCLUDED

#include <defs.h>
#include <Year.h>

#define PLUGIN_TYPE		OOP

#define	LIB_VERSION		39
#define	LIB_REVISION		26

#define LIB_NAME 		"persist.plugin"
#define LIB_VERSTRING 		"$VER: persist.plugin " \
				STR(LIB_VERSION) "." STR(LIB_REVISION) \
				" " __DATE__ " " COMPILER_STRING \
				" ©1999" CURRENTYEAR " The Scalos Team"


#define CI_CLASSNAME 		"Window.sca"
#define CI_SUPERCLASSNAME 	"Window.sca"
#define CI_PLUGINNAME 		"Persistant Windows"
#define CI_DESCRIPTION 		"Remembers all open Workbench windows" COMPILER_STRING
#define CI_AUTHOR 		"Jürgen Lachmann"
#define CI_HOOKFUNC 		persistHookFunc

//----------------------------------------------------------------------------
// code and includes to define the structs and functions used above

#include "Persist.h"

#endif /* PLUGUN_DATA_H_INCLUDED */

