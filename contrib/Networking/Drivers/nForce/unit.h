#ifndef _UNIT_H_
#define _UNIT_H_

#include <exec/types.h>
#include <exec/resident.h>
#include <exec/io.h>
#include <exec/ports.h>
#include <exec/errors.h>

#include <aros/debug.h>

#include <devices/sana2.h>
#include <devices/sana2specialstats.h>
#include <devices/newstyle.h>

#include <utility/utility.h>
#include <utility/tagitem.h>
#include <utility/hooks.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/battclock.h>

#include <stdlib.h>

#include "nforce.h"
#include LC_LIBDEFS_FILE

struct NFUnit *CreateUnit(LIBBASETYPEPTR LIBBASE, OOP_Object *device);
void DeleteUnit(struct NFBase *NforceBase, struct NFUnit *unit);
void FlushUnit(LIBBASETYPEPTR LIBBASE, struct NFUnit *unit, UBYTE last_queue, BYTE error);
BOOL AddressFilter(struct NFBase *NforceBase, struct NFUnit *unit, UBYTE *address);
VOID CopyPacket(struct NFBase *NforceBase, struct NFUnit *unit, 
	struct IOSana2Req *request, UWORD packet_size, UWORD packet_type,
	struct eth_frame *buffer);
VOID ReportEvents(struct NFBase *NforceBase, struct NFUnit *unit, ULONG events);
struct TypeStats *FindTypeStats(struct NFBase *NforceBase, struct NFUnit *unit, struct MinList *list,
   ULONG packet_type);
   
#endif //_UNIT_H_
