
#ifndef _LIBRARY_INIT_H
#define _LIBRARY_INIT_H

#ifdef __SASC
#ifdef _M68881
#include <m68881.h>
#endif
#endif

#include <exec/libraries.h>
#include <exec/execbase.h>
#include <exec/resident.h>
#include <dos/dosextens.h>

#ifdef __AROS__
#if !AROS_BIG_ENDIAN
#include <aros/macros.h>
#define RENDER_LITTLE_ENDIAN
#endif
#endif

#define LIB_VERSION  		40
#define LIB_REVISION 		8
#define LIB_NAME			"render.library"
#define LIB_DATE 			"(30.03.2003)"

typedef struct RenderBase
{
	struct Library LibNode;
	struct SignalSemaphore LockSemaphore;
	struct ExecBase *ExecBase;
	BPTR LibSegment;

} LIB_BASE_T;

#ifndef LIBAPI
#define LIBAPI
#endif

LIBAPI BOOL Library_Init(LIB_BASE_T *LibBase);
LIBAPI void Library_Exit(LIB_BASE_T *LibBase);

#include "rnd_memory.h"
#include "rnd_histogram.h"
#include "rnd_palette.h"
#include "rnd_quant.h"
#include "rnd_mapping.h"
#include "rnd_conversion.h"
#include "rnd_diversity.h"
#include "rnd_alpha.h"
#include "rnd_scale.h"
#include "rnd_render.h"

#endif
