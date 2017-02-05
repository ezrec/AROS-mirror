#include "cdxlplay.h"
#include <exec/exec.h>
#include <devices/timer.h>
#include <proto/exec.h>
#ifdef __AMIGAOS4__
#include <inline4/exec.h>
#endif

static struct MsgPort *timer_mp;
static struct timerequest *timer_io;
static struct Device *timer_base;

int setup_usleep(void) {
	timer_mp = CreateMsgPort();
	timer_io = (struct timerequest *)CreateIORequest(timer_mp, sizeof(*timer_io));
	if (timer_io) {
		if (!OpenDevice("timer.device", UNIT_MICROHZ, &timer_io->tr_node, 0)) {
			timer_base = timer_io->tr_node.io_Device;
			return 1;
		}
	}
	return 0;
}

void cleanup_usleep(void) {
	if (timer_base) {
		CloseDevice(&timer_io->tr_node);
		timer_base = NULL;
	}
	DeleteIORequest((struct IORequest *)timer_io);
	timer_io = NULL;
	DeleteMsgPort(timer_mp);
	timer_mp = NULL;
}

int usleep(useconds_t usec) {
	timer_io->tr_node.io_Command = TR_ADDREQUEST;
	timer_io->tr_time.tv_sec = usec / 1000000UL;
	timer_io->tr_time.tv_usec = usec % 1000000UL;
	return DoIO(&timer_io->tr_node) ? -1 : 0;
}
