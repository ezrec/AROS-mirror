/*
** libbase.h
 *
 * $Date$
 * $Revision$
 *
** 5th May 2000
** David McMinn
** The structure defining the library base of our example plugin
** (since plugins are nothing more than shared libraries)
*/

#ifndef LIBBASE_H
#define LIBBASE_H

#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/semaphores.h>
#include <dos/notify.h>
#include <defs.h>

#include "plugin.h"

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack(2)
#endif /* __GNUC__ */

#define FORMAT_SIZE 32

struct myLibBase
{
	struct PluginBase       lb_PluginBase;         /* The actual Library structure for our librarybase, always first in a libbase */
	char    		lb_datestr[FORMAT_SIZE];
	char    		lb_timestr[FORMAT_SIZE];
	struct NotifyRequest	lb_NotifyRequest;
	struct MsgPort		*lb_NotifyPort;
};

#if defined __GNUC__ && !defined(mc68000)
#pragma pack()
#endif /* __GNUC__ */

#endif

