#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/resident.h>

#include "shutdown.library.h"

int entry(void) 
{
	return -1;
}

extern char LibName[];
extern unsigned char LibID[];
extern struct InitTable LibInitTab;

extern int ShutDown_EndCode;

struct Resident InitDesc = 
{
	RTC_MATCHWORD,
	(struct Resident *)&InitDesc,
	&ShutDown_EndCode,
	RTF_AUTOINIT,
	VERSION,
	NT_LIBRARY,
	0,
	LibName,
	LibID,
	&LibInitTab
};
