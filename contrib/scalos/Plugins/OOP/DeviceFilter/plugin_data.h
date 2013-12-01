// plugin_data.h
// $Date$
// $Revision$

#ifndef PLUGIN_DATA_H_INCLUDED
#define PLUGIN_DATA_H_INCLUDED

#include <defs.h>
#include <Year.h>

#define PLUGIN_TYPE		OOP

#define LIB_BASETYPE		struct DeviceFilterBase

#define	LIB_VERSION		39
#define	LIB_REVISION		19

#define LIB_NAME 		"devicefilter.plugin"
#define LIB_VERSTRING 		"$VER: devicefilter.plugin " \
				STR(LIB_VERSION) "." STR(LIB_REVISION) \
				" (13.03.2007) " COMPILER_STRING \
				" ©1999" CURRENTYEAR " The Scalos Team"


#define CI_CLASSNAME 		"DeviceList.sca"
#define CI_SUPERCLASSNAME 	"DeviceList.sca"
#define CI_PLUGINNAME 		"Device Filter"
#define CI_DESCRIPTION 		"Filters out BAD and NDOS disks and devices " \
				"in the hidden list (in Workbench prefs) " COMPILER_STRING
#define CI_AUTHOR 		"Scalos Team"
#define CI_HOOKFUNC 		Dispatcher

//----------------------------------------------------------------------------
// code and includes to define the structs and functions used above

#include "devicefilter.h"

#endif /* PLUGUN_DATA_H_INCLUDED */

