/*
 * storage_task.c
 *
 *  Created on: Oct 27, 2008
 *      Author: misc
 */

#define DEBUG 1
#include <aros/debug.h>
#include <devices/timer.h>
#include <devices/trackdisk.h>
#include <devices/scsidisk.h>
#include <devices/newstyle.h>
#include <exec/io.h>
#include <exec/ports.h>
#include <exec/devices.h>
#include <exec/errors.h>
#include "storage.h"

/*
 * Unit task.
 * Every single mass storage unit in the system becomes it's own handler task.
 * The task's job is to request periodically the media sense and report any potential
 * disk change soft interrupts. Additionally, it is used to perform all *slow*
 * IO operations.
 *
 * NOTE: Multi-lun usb devices will get more than one unit helper task.
 */
void StorageTask(OOP_Class *cl, OOP_Object *o, uint32_t unitnum, struct Task *parent)
{
	const char *name = FindTask(NULL)->tc_Node.ln_Name;
	StorageData *mss = OOP_INST_DATA(cl, o);
	uint8_t lun = unitnum & 0x0f;
	uint32_t sigset = SIGBREAKF_CTRL_C;
	uint32_t rcvd;
	mss_unit_t *unit;
	struct timerequest *timer_io;

	/* Make sure the access is exclusive as long as the init is not ready */
	ObtainSemaphore(&mss->lock);

	/* Initialize the unit */
	unit = (mss_unit_t *)AllocVecPooled(SD(cl)->MemPool, sizeof(mss_unit_t));
	unit->msu_unit.unit_MsgPort.mp_SigBit = AllocSignal(-1);
	unit->msu_unit.unit_MsgPort.mp_Flags = PA_SIGNAL;
	unit->msu_unit.unit_MsgPort.mp_SigTask = FindTask(NULL);
	unit->msu_unit.unit_MsgPort.mp_Node.ln_Type = NT_MSGPORT;
	unit->msu_class = cl;
	unit->msu_object = o;
	NEWLIST(&unit->msu_unit.unit_MsgPort.mp_MsgList);
	NEWLIST(&unit->msu_diskChangeList);

	sigset |= 1 << unit->msu_unit.unit_MsgPort.mp_SigBit;

	if (unit)
	{
		D(bug("[%s] Good morning!\n", name));

		if (HIDD_USBStorage_ReadCapacity(o, unitnum & 0xf, &unit->msu_blockCount, &unit->msu_blockSize))
		{
			D(bug("[MSS] Detected capacity: %dMB\n", (((unit->msu_blockCount+1) >> 10)*unit->msu_blockSize) >> 10));
		}

		unit->msu_unitNum = unitnum;

		/* timer request will be used to pool periodically for disk changes */
		timer_io = CreateIORequest(CreateMsgPort(), sizeof(struct timerequest));
		OpenDevice("timer.device", UNIT_MICROHZ, timer_io, 0);

		sigset |= 1 << timer_io->tr_node.io_Message.mn_ReplyPort->mp_SigBit;

		AddTail(&SD(cl)->unitList, unit);

		/* Get rid of exclusive access */
		ReleaseSemaphore(&mss->lock);

		/* Set up idle timer */
		timer_io->tr_node.io_Command = TR_ADDREQUEST;
		timer_io->tr_time.tv_secs = 2;
		timer_io->tr_time.tv_micro = 0;
		SendIO(timer_io);

		/* Tell the initiator that we're done with setup */
		Signal(parent, SIGF_SINGLE);

		/* Enter almost endless loop */
		do {
			rcvd = Wait(sigset);

			D(bug("[%s] sigset=%08x, got %08x\n", name, sigset, rcvd));

			/* Got a timer request? Handle it (TestUnitReady) now */
			if (rcvd & (1 << timer_io->tr_node.io_Message.mn_ReplyPort->mp_SigBit))
			{
				/* Timer request. Perform periodic media sense */

				while (GetMsg(timer_io->tr_node.io_Message.mn_ReplyPort));

				uint8_t unit_ready;
				/* Device is idling. Do media change sense */
				D(bug("[%s] Idle: Sensing...\n", name));

				unit_ready = HIDD_USBStorage_TestUnitReady(o, unitnum & 0xf);

				if (!unit_ready)
				{
					uint8_t sense[19];
					int i;
					D(bug("[%s] Unit was not ready. Requesting sense.\n", name));

					HIDD_USBStorage_RequestSense(o, unitnum & 0xf, sense, 19);

					D(bug("[%s] Sense: ", name));
					for (i=0; i < 19; i++)
						D(bug("%02x ", sense[i]));
					D(bug("\n"));
				}
			}
			else
			{
				/*
				 * Something else appeared? Cancel the timer request.
				 */
				D(bug("[%s] Aborting timer io\n", name));
				if (!CheckIO(timer_io))
					AbortIO(timer_io);
				WaitIO(timer_io);
				SetSignal(0, 1 << timer_io->tr_node.io_Message.mn_ReplyPort->mp_SigBit);
			}

			/* Message in IORequest queue? */
			if (rcvd & (1 << unit->msu_unit.unit_MsgPort.mp_SigBit))
			{
				/* If unit is active, do not handle the IO request */
				if (!(unit->msu_unit.unit_flags & UNITF_ACTIVE))
				{
					unit->msu_unit.unit_flags |= UNITF_ACTIVE;
					struct IORequest *msg = NULL;

					/* Empty the queue */
					while ((msg = (struct IORequest *)GetMsg(&unit->msu_unit.unit_MsgPort)))
					{
						D(bug("[%s] CMD=%04x\n", name, msg->io_Command));

						HandleIO(msg, msg->io_Device, unit);

						if (msg->io_Command != TD_ADDCHANGEINT)
							ReplyMsg((struct Message *)msg);
					}

					unit->msu_unit.unit_flags &= ~(UNITF_ACTIVE);
				}
			}

			/*
			 * Set up idle timer. The main loop of the task will be waken up
			 * after two seconds
			 */
			timer_io->tr_node.io_Command = TR_ADDREQUEST;
			timer_io->tr_time.tv_secs = 2;
			timer_io->tr_time.tv_micro = 0;
			SendIO(timer_io);
		} while(!(rcvd & SIGBREAKF_CTRL_C));

		if (!CheckIO(timer_io))
			AbortIO(timer_io);
		WaitIO(timer_io);

		/* CTRL_C sent? Make a nice cleanup. */
		DeleteMsgPort(timer_io->tr_node.io_Message.mn_ReplyPort);
		DeleteIORequest(timer_io);

		Remove(unit);
		FreeVecPooled(SD(cl)->MemPool, unit);
	}

	D(bug("[%s] Goodbye!\n", name));
}
