/*
**	GadTools layout toolkit
**
**	Copyright © 1993-1998 by Olaf `Olsen' Barthel
**		Freely distributable.
*/

#ifndef _GTLAYOUT_GLOBAL_H
#define _GTLAYOUT_GLOBAL_H 1

/****************************************************************************/

#ifndef __AROS__
#define SAVE_DS
#define ASM	__asm
#define REG(x)	register __ ## x
#define LIBENT	SAVE_DS ASM
#else
#define SAVE_DS
#define ASM
#define REG(x)
#define LIBENT
#define __stdargs
#define __chip
#endif

/****************************************************************************/

#ifndef __AROS__
#ifdef LINK_LIB
#undef SAVE_DS
#undef REG
#undef LIBENT
#define SAVE_DS __saveds
#define REG(x)
#define LIBENT
#endif	// LINK_LIB
#endif

/****************************************************************************/

#ifdef __AROS__
#include <intuition/sghooks.h>
#endif

#ifndef _GTLAYOUT_INCLUDES_H
#include "gtlayout_includes.h"
#endif	// _GTLAYOUT_INCLUDES_H

#ifndef _GTLAYOUT_H
#define GTLAYOUT_OBSOLETE 1
#define LayoutHandle PublicLayoutHandle
#include "gtlayout.h"
#undef LayoutHandle
#define LayoutHandle LayoutHandle
#endif	// _GTLAYOUT_H

#ifndef _GTLAYOUT_INTERNAL_H
#include "gtlayout_internal.h"
#endif	// _GTLAYOUT_INTERNAL_H

#ifndef _GTLAYOUT_DATA_H
#include "gtlayout_data.h"
#endif	// _GTLAYOUT_DATA_H

#ifndef _GTLAYOUT_LIBPROTOS_H
#include "gtlayout_libprotos.h"
#endif	// _GTLAYOUT_LIBPROTOS_H

/****************************************************************************/

#endif	// _GTLAYOUT_GLOBAL_H
