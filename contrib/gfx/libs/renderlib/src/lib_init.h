
#ifndef _LIBRARY_INIT_H
#define _LIBRARY_INIT_H

#ifdef __SASC
#include <m68881.h>
#endif

#include <exec/libraries.h>
#include <exec/execbase.h>
#include <exec/resident.h>
#include <dos/dosextens.h>

#define LIB_VERSION  		40
#define LIB_REVISION 		3
#define LIB_NAME			"render.library"
#define LIB_DATE 			"(21.12.2002)"

typedef struct RenderBase
{
	struct Library LibNode;
	struct ExecBase *ExecBase;
	BPTR LibSegment;
	struct SignalSemaphore LockSemaphore;

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
