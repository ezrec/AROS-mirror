
#include	<stdlib.h>
#include	<stdio.h>

#include	<proto/exec.h>
#include	<datatypes/datatypes.h>
#include	<cybergraphx/cybergraphics.h>
#include	<proto/cybergraphics.h>

#include	"system.h"
#include	"functions.h"
#include	"timer.h"
#include	"altivec.h"
#include	"memory.h"

struct	Library	*DataTypesBase;

void ExitSystem(void)
{
	if (CyberGfxBase)
		CloseLibrary(CyberGfxBase);

	if (DataTypesBase)
		CloseLibrary(DataTypesBase);

}

void InitSystem( int argc , char *argv[] )
{
	CyberGfxBase = OpenLibrary("cybergraphics.library", 0);
	DataTypesBase = OpenLibrary("datatypes.library", 0);
	ParamsInit(argc, argv);
	TimerInit();
#if defined(__MORPHOS__)
	AltivecInit();
	MemoryInit();
#endif
	ConversionInit();

	atexit(&ExitSystem);

}

