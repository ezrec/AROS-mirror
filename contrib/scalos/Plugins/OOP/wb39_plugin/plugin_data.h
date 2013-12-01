// plugin_data.h
// $Date$
// $Revision$

#ifndef PLUGIN_DATA_H_INCLUDED
#define PLUGIN_DATA_H_INCLUDED

#include <defs.h>
#include <Year.h>

#define PLUGIN_TYPE		OOP

#define	LIB_VERSION		45
#define	LIB_REVISION		34

#define LIB_NAME 		"wb39.plugin"
#define LIB_VERSTRING 		"$VER: wb39.plugin " \
				STR(LIB_VERSION) "." STR(LIB_REVISION) \
				" (11.02.2008) " COMPILER_STRING \
				" ©1999" CURRENTYEAR " The Scalos Team"


#define CI_CLASSNAME 		"Window.sca"
#define CI_SUPERCLASSNAME 	"Window.sca"
#define CI_PLUGINNAME 		"Scalos API Extension"
#define CI_DESCRIPTION 		"Extends Scalos API towards OS3.9. " COMPILER_STRING
#define CI_AUTHOR 		"Jürgen Lachmann"
#define CI_HOOKFUNC 		myHookFunc

//----------------------------------------------------------------------------
// code and includes to define the structs and functions used above

#include "wb39.h"

#endif /* PLUGUN_DATA_H_INCLUDED */

