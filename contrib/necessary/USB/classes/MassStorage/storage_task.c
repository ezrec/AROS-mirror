/*
 * storage_task.c
 *
 *  Created on: Oct 27, 2008
 *      Author: misc
 */

#define DEBUG 1
#include <aros/debug.h>
#include "storage.h"

void StorageTask(OOP_Class *cl, OOP_Object *o, uint8_t lun, struct Task *parent)
{
	const char *name = FindTask(NULL)->tc_Node.ln_Name;
	StorageData *mss = OOP_INST_DATA(cl, o);
	uint32_t sigset = SIGBREAKF_CTRL_C;
	uint32_t rcvd;
	uint32_t capacity[2];

	D(bug("[%s] Good morning!\n", name));

	if (HIDD_USBStorage_ReadCapacity(o, lun, &capacity[0], &capacity[1]))
	{
		D(bug("[MSS] Detected capacity: %dMB\n", (((capacity[0]+1) >> 20)*capacity[1])));
	}

	uint8_t buff[1024];
	D(bug("[MSS] trying Read\n"));

	HIDD_USBStorage_Read(o, lun, buff, 10, 2);

	int x,y;
	for (y=0; y < 1024/16; y++)
	{
		D(bug("[MSS] %04x:  ", y*16));
		for (x=0; x < 16; x++)
		{
			D(bug("%02x ", buff[16*y + x]));
		}
		D(bug("    "));
		for (x=0; x < 16; x++)
		{
			if (buff[16*y + x] >= 0x20 && buff[16*y + x] <= 0x7e)
				D(bug("%c", (buff[16*y + x]) ));
			else
				D(bug("."));
		}
		D(bug("\n"));
	}

	/* Tell the initiator that we're done with setup */
	Signal(parent, SIGF_SINGLE);
	/* Enter almost endless loop */
	do {
		rcvd = Wait(SIGBREAKF_CTRL_C);

	} while(!(rcvd & SIGBREAKF_CTRL_C));

	D(bug("[%s] Goodbye!\n", name));
}
