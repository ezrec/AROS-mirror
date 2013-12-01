// plugin_data.h
// $Date$
// $Revision$

#ifndef PLUGIN_DATA_H_INCLUDED
#define PLUGIN_DATA_H_INCLUDED

#include <defs.h>
#include <Year.h>

#define PLUGIN_TYPE		OOP

#define	LIB_VERSION		39
#define	LIB_REVISION		8

#define LIB_NAME 		"volumegauge.plugin"
#define LIB_VERSTRING 		"$VER: volumegauge.plugin " \
				STR(LIB_VERSION) "." STR(LIB_REVISION) \
				" (02.01.2007) " COMPILER_STRING \
				" ©1999" CURRENTYEAR " The Scalos Team"

#define CI_CLASSNAME 		"Window.sca"
#define CI_SUPERCLASSNAME 	"Window.sca"
#define CI_PLUGINNAME 		"Scalos VolumeGauge Class"
#define CI_DESCRIPTION 		"This class gives root windows a volume gauge" COMPILER_STRING
#define CI_AUTHOR 		"Jürgen Lachmann"
#define CI_INSTSIZE 		sizeof(struct VolumeGaugeInst)
#define CI_HOOKFUNC 		myHookFunc

//----------------------------------------------------------------------------
// code and includes to define the structs and functions used above

#include "volumegauge.h"

#endif /* PLUGUN_DATA_H_INCLUDED */

