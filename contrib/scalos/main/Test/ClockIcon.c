#include <exec/libraries.h>
#include <devices/timer.h>
#include <intuition/intuition.h>
#include <utility/date.h>
#include <workbench/workbench.h>
#include <dos/dos.h>
#include <stdarg.h>

#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/utility_protos.h>
#include <clib/timer_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/wb_protos.h>

#include <pragmas/graphics_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/utility_pragmas.h>
#include <pragmas/timer_pragmas.h>
#include <pragmas/exec_sysbase_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/wb_pragmas.h>

/****************************************************************************/

#define OK (0)

/****************************************************************************/

void kprintf(const char *,...);

/****************************************************************************/

extern struct Library * IntuitionBase;
extern struct Library * GfxBase;
extern struct Library * UtilityBase;
extern struct Library * WorkbenchBase;
extern struct Library * SysBase;

/****************************************************************************/

struct Device * TimerBase;
struct MsgPort * TimePort;
struct timerequest * TimeRequest;

/****************************************************************************/

BOOL WorkbenchControlA( STRPTR name, struct TagItem *tags );
BOOL WorkbenchControl( STRPTR name, ... );

#pragma libcall WorkbenchBase WorkbenchControlA 6c 9802
#pragma tagcall WorkbenchBase WorkbenchControl 6c 9802

/****************************************************************************/

UWORD __chip ClockI1Data[] =
{
	/* Plane 0 */
	0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x8000,0x8000,0x0000,0x0000,
	0x0000,0x0000,0x8000,0x0000,0x0000,0x0000,0x0000,0x8000,
	0x0000,0x0000,0x0000,0x0000,0x8000,0x0000,0x0000,0x0000,
	0x0000,0x8000,0x0000,0x0000,0x0000,0x0000,0x8000,0x0000,
	0x0000,0x0000,0x0000,0x8000,0x0000,0x0000,0x0000,0x0000,
	0x8000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,

	/* Plane 1 */
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x4000,0x0000,0x0000,0x0000,0x0000,0x4000,0x0000,
	0x0000,0x0000,0x0000,0x4000,0x0000,0x0000,0x0000,0x0000,
	0x4000,0x0000,0x0000,0x0000,0x0000,0x4000,0x0000,0x0000,
	0x0000,0x0000,0x4000,0x0000,0x0000,0x0000,0x0000,0x4000,
	0x0000,0x0000,0x0000,0x0000,0x4000,0x7FFF,0xFFFF,0xFFFF,
	0xFFFF,0xC000
};

struct Image ClockI1 =
{
	0, 0,		/* Upper left corner */
	66, 10, 2,	/* Width, Height, Depth */
	ClockI1Data,	/* Image data */
	0x0003, 0x0000,	/* PlanePick, PlaneOnOff */
	NULL		/* Next image */
};

struct DiskObject Clock =
{
	WB_DISKMAGIC,		/* Magic Number */
	WB_DISKVERSION,		/* Version */
	{			/* Embedded Gadget Structure */
		NULL,			/* Next Gadget Pointer */
		0, 0, 66, 11,		/* Left,Top,Width,Height */
		GFLG_GADGIMAGE | GFLG_GADGHCOMP,	/* Flags */
		GACT_RELVERIFY,		/* Activation Flags */
		BOOLGADGET,		/* Gadget Type */
		(APTR)&ClockI1,		/* Render Image */
		NULL,			/* Select Image */
		NULL,			/* Gadget Text */
		NULL,			/* Mutual Exclude */
		NULL,			/* Special Info */
		0,			/* Gadget ID */
		(APTR) 0x0001,		/* User Data (Revision) */
	},
	WBTOOL,			/* Icon Type */
	NULL,			/* Default Tool */
	NULL,			/* Tool Type Array */
	NO_ICON_POSITION,	/* Current X */
	NO_ICON_POSITION,	/* Current Y */
	NULL,			/* Drawer Structure */
	NULL,			/* Tool Window */
	0			/* Stack Size */
};

/****************************************************************************/

struct TextFont * TopazFont;

struct TextAttr Topaz =
{
	"topaz.font",
	8,
	FS_NORMAL,
	FPF_ROMFONT|FPF_DESIGNED
};

/****************************************************************************/

VOID
SPrintf(STRPTR buffer, STRPTR formatString,...)
{
	va_list varArgs;

	va_start(varArgs,formatString);
	RawDoFmt(formatString,varArgs,(VOID (*)())"\x16\xC0\x4E\x75",buffer);
	va_end(varArgs);
}

/****************************************************************************/

LONG __saveds __asm __interrupt
ClockHookFunc(
	register __a0 struct Hook *		hook,
	register __a2 APTR			unused,
	register __a1 struct AppIconRenderMsg *	arm)
{
	struct timeval now;
	struct ClockData clock;
	struct RastPort rp = (*arm->arm_RastPort);
	UBYTE buffer[20];
	LONG left,top;

	kprintf(__FILE__ "/" __FUNC__ "/%ld: rp=%08lx  left=%ld top=%ld width=%ld height=%ld\n",
		__LINE__, arm->arm_RastPort, arm->arm_Left, arm->arm_Top, arm->arm_Width, arm->arm_Height);

	GetSysTime(&now);

	Amiga2Date(now.tv_secs,&clock);

	SPrintf(buffer,"%02ld:%02ld:%02ld",clock.hour,clock.min,clock.sec);

	SetFont(&rp,TopazFont);
	SetABPenDrMd(&rp,2,1,JAM2);

	left = arm->arm_Left;
	top = arm->arm_Top;

	RectFill(&rp,left,top,
		left + arm->arm_Width - 1,
		top + arm->arm_Height - 1);

	SetAPen(&rp,2);
	Move(&rp,left + 1,top + 1 + rp.TxBaseline);
	Text(&rp,buffer,8);

	return(FALSE);
}

struct Hook ClockHook =
{
	{NULL},
	(HOOKFUNC)ClockHookFunc
};

/****************************************************************************/

int
main(int argc,char ** argv)
{
	TimePort = CreateMsgPort();
	if(TimePort != NULL)
	{
		TimeRequest = (struct timerequest *)CreateIORequest(TimePort,sizeof(*TimeRequest));
		if(TimeRequest != NULL)
		{
			if(OpenDevice(TIMERNAME,UNIT_VBLANK,(struct IORequest *)TimeRequest,0) == OK)
			{
				TimerBase = TimeRequest->tr_node.io_Device;

				if(WorkbenchBase->lib_Version >= 44)
				{
					TopazFont = OpenFont(&Topaz);
					if(TopazFont != NULL)
					{
						struct MsgPort * appPort;

						appPort = CreateMsgPort();
						if(appPort != NULL)
						{
							struct AppIcon * appIcon;
							struct Message * msg;

							appIcon = AddAppIcon(0,0,"Clock",appPort,0,&Clock,
								WBAPPICONA_RenderHook,&ClockHook,
							TAG_DONE);
							if(appIcon != NULL)
							{
								ULONG signals;
								BOOL done = FALSE;

								TimeRequest->tr_node.io_Command	= TR_ADDREQUEST;
								TimeRequest->tr_time.tv_secs	= 1;
								TimeRequest->tr_time.tv_micro	= 0;
								SendIO((struct IORequest *)TimeRequest);

								do
								{
									signals = Wait(SIGBREAKF_CTRL_C | (1UL << TimePort->mp_SigBit) | (1UL << appPort->mp_SigBit));

									if(signals & (1UL << TimePort->mp_SigBit))
									{
										WaitIO((struct IORequest *)TimeRequest);

										WorkbenchControl(NULL,
											WBCTRLA_RedrawAppIcon,appIcon,
										TAG_DONE);

										TimeRequest->tr_node.io_Command	= TR_ADDREQUEST;
										TimeRequest->tr_time.tv_secs	= 1;
										TimeRequest->tr_time.tv_micro	= 0;
										SendIO((struct IORequest *)TimeRequest);
									}

									if(signals & (1UL << appPort->mp_SigBit))
									{
										while((msg = GetMsg(appPort)) != NULL)
											ReplyMsg(msg);
									}

									if(signals & SIGBREAKF_CTRL_C)
									{
										done = TRUE;
									}
								}
								while(NOT done);

								RemoveAppIcon(appIcon);
							}

							AbortIO((struct IORequest *)TimeRequest);
							WaitIO((struct IORequest *)TimeRequest);

							while((msg = GetMsg(appPort)) != NULL)
								ReplyMsg(msg);

							DeleteMsgPort(appPort);
						}

						CloseFont(TopazFont);
					}
				}

				CloseDevice((struct IORequest *)TimeRequest);
			}

			DeleteIORequest((struct IORequest *)TimeRequest);
		}

		DeleteMsgPort(TimePort);
	}

	return(0);
}
