/*----------------------------···
 ·
 ·	XTWindows.c
 ·
 ----------------------------··*/
// $Date$
// $Revision$

#include <datatypes/datatypesclass.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <intuition/gadgetclass.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/input.h>
#include <proto/scalos.h>

#include <scalos/scalos.h>
#include <string.h>
#include <stdio.h>
#include "AutoMsg.h"

#include <defs.h>
#include "XTWindows.h"

#ifdef __AROS__
#include "plugin-common.c"
#endif

DISPATCHER_PROTO(FreeMsgInter);


extern struct ExecBase	*SysBase;
#ifndef __AROS__
struct DosLibrary	*DOSBase = NULL;
struct IntuitionBase	*IntuitionBase = NULL;
#endif
struct ScalosBase	*ScalosBase = NULL;
T_INPUTDEVICE		InputBase = NULL;
struct Library		*IconobjectBase = NULL;
T_UTILITYBASE		UtilityBase = NULL;

#ifdef __amigaos4__
struct Library 		*NewlibBase = NULL;

struct DOSIFace		*IDOS = NULL;
struct IntuitionIFace	*IIntuition = NULL;
struct IconobjectIFace	*IIconobject = NULL;
struct ScalosIFace	*IScalos = NULL;
struct InputIFace	*IInput = NULL;
struct UtilityIFace	*IUtility = NULL;
struct Interface 	*INewlib = NULL;
#endif

static struct AutoReplyPort    replyPort;

//----------------------------------------------------------------------------·

DISPATCHER(FreeMsgInter)
{
	struct AutoReplyPort *arp = (struct AutoReplyPort *) msg;

	(void) cl;
	(void) obj;

	while (1)
		{
		struct ScalosMessage *sMsg;

		sMsg = (struct ScalosMessage *) GetMsg(&arp->msgPort);
		if (NULL == sMsg)
			{
			arp->numMsgs--;
			break;
			}
		SCA_FreeMessage(sMsg);
		}
	return 0;
}
DISPATCHER_END

//----------------------------------------------------------------------------·
//  LibInit / LibCleanup / GetClassInfo
//----------------------------------------------------------------------------·

BOOL initPlugin(struct PluginBase *base)
{
///FrexxStartFold1.2.0.
	d1(KPrintF(__FUNC__ "/%ld: lib_OpenCnt=%lu\n", __LINE__, base->lib_OpenCnt));

#ifdef __AROS__
	base->pl_PlugID = MAKE_ID('P','L','U','G');
#endif

	if( !(DOSBase = (struct DosLibrary *)OpenLibrary( "dos.library", 37 )) )
		{
		d1(KPrintF(__FUNC__ "/%ld: Failure!\n", __LINE__));
		return FALSE;
		}
#ifdef __amigaos4__
	if( !(IDOS = (struct DOSIFace *)GetInterface((struct Library *)DOSBase, "main", 1, NULL)))
		return FALSE;
#endif

	if( !(IntuitionBase = (struct IntuitionBase *)OpenLibrary( "intuition.library", 37 )) )
		{
		d1(KPrintF(__FUNC__ "/%ld: Failure!\n", __LINE__));
		return FALSE;
		}
#ifdef __amigaos4__
	if( !(IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL)))
		return FALSE;
#endif

	if( !(IconobjectBase = OpenLibrary( "iconobject.library", 39 )) )
		{
		d1(KPrintF(__FUNC__ "/%ld: Failure!\n", __LINE__));
		return FALSE;
		}
#ifdef __amigaos4__
	if( !(IIconobject = (struct IconobjectIFace *)GetInterface((struct Library *)IconobjectBase, "main", 1, NULL)))
		return FALSE;
#endif

	UtilityBase = (T_UTILITYBASE) OpenLibrary("utility.library", 39);
	if (NULL == UtilityBase)
		{
		d1(KPrintF(__FUNC__ "/%ld: Failure!\n", __LINE__));
		return FALSE;
		}
#ifdef __amigaos4__
	if( !(IUtility = (struct UtilityIFace *)GetInterface((struct Library *)UtilityBase, "main", 1, NULL)))
		return FALSE;
#endif
	if( !(ScalosBase = (struct ScalosBase *) OpenLibrary( "scalos.library", 39 )) )
		{
		d1(KPrintF(__FUNC__ "/%ld: Failure!\n", __LINE__));
		return FALSE;
		}
#ifdef __amigaos4__
	if( !(IScalos = (struct ScalosIFace *)GetInterface((struct Library *)ScalosBase, "main", 1, NULL)))
		return FALSE;
#endif

	Disable();
	InputBase = (T_INPUTDEVICE) FindName(&SysBase->DeviceList, "input.device");
	Enable();

	if( !InputBase )
		{
		d1(KPrintF(__FUNC__ "/%ld: Failure!\n", __LINE__));
		return FALSE;
		}
#ifdef __amigaos4__
	if( !(IInput = (struct InputIFace *)GetInterface((struct Library *)InputBase, "main", 1, NULL)))
		return FALSE;

	NewlibBase = OpenLibrary("newlib.library", 0);
	if (NULL == NewlibBase)
		return FALSE;
	INewlib = GetInterface(NewlibBase, "main", 1, NULL);
	if (NULL == INewlib)
		return FALSE;
#endif

	InitAutoReplyPort( &replyPort, ScalosBase );

	d1(KPrintF(__FUNC__ "/%ld: Success!\n", __LINE__));

	return TRUE;
}
///FrexxEndFold1.

BOOL closePlugin(struct PluginBase *base)
{
///FrexxStartFold1.2.0.

	d1(kprintf(__FUNC__ "/%ld: lib_OpenCnt=%lu\n", __LINE__, base->lib_OpenCnt));

	FreeAutoReplyPort( &replyPort );

#ifdef __amigaos4__
	if (INewlib)
		DropInterface(INewlib);
	if( IScalos )
		DropInterface( (struct Interface *)IScalos );
	if( IUtility )
		DropInterface( (struct Interface *)IUtility );
	if( IIconobject )
		DropInterface( (struct Interface *)IIconobject );
	if( IIntuition )
		DropInterface( (struct Interface *)IIntuition );
	if( IDOS )
		DropInterface( (struct Interface *)IDOS );

	if (NewlibBase)
		CloseLibrary(NewlibBase);
#endif
	if( ScalosBase )
		CloseLibrary( (struct Library *)ScalosBase );
	if (UtilityBase)
		CloseLibrary((struct Library *) UtilityBase);
	if( IconobjectBase )
		CloseLibrary( IconobjectBase );
	if( IntuitionBase )
		CloseLibrary( (struct Library *)IntuitionBase );
	if( DOSBase )
		CloseLibrary( (struct Library *)DOSBase );
	return TRUE;
}
///FrexxEndFold1.

//----------------------------------------------------------------------------·
//  Main entry point
//----------------------------------------------------------------------------·

M68KFUNC_P3(ULONG, Dispatcher,
	A0, Class *, cl,
	A2, Object *, obj,
	A1, Msg, msg)
{
///
	ULONG Result;
	struct XTWindowsInstanceData *inst;

	switch (msg->MethodID)
		{
	case OM_NEW:
		obj = (Object *) DoSuperMethodA( cl,obj,msg );
		if (obj)
			{
			inst = INST_DATA(cl,obj);
			inst->windowTask = ((struct ScaRootList *)obj)->rl_WindowTask;
			}
		Result = (ULONG) obj;
		break;

	case SCCM_Message:
		{
		struct msg_Message *methodMsg = (struct msg_Message *) msg;

		inst = INST_DATA(cl, obj);

		if (IDCMP_CLOSEWINDOW == methodMsg->msm_iMsg->Class)
			{
			if ( PeekQualifier() & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT) )
				{
				BPTR	lock, windowLock;
				char	*name;

				// --------------------··· Open parentdrawer ------------------------··
				inst->windowTask = ((struct ScaRootList *)obj)->rl_WindowTask;
				lock = inst->windowTask->mt_WindowStruct->ws_Lock;
				windowLock = ParentDir( lock );
				if( windowLock )
					{
					name = AllocVec( 1000, 0 );
					if( name )
						{
						if( NameFromLock( windowLock, name, 1000 ) )
							{
							SCA_OpenDrawerByNameTags(name, TAG_END);
/*
							struct TagItem	tags[2] = { SCA_Path, 0, TAG_END };

							tags[0].ti_Data = (ULONG)name;
							SCA_OpenIconWindow( tags );
*/
							}
						FreeVec( name );
						}
					UnLock( windowLock );
					}
				}
			}
		Result = DoSuperMethodA(cl, obj, msg);
		}
		break;

	case SCCM_IconWin_Open:
		{
		if( !DoSuperMethodA(cl, obj, msg) )
			Result =  FALSE;
		else
			{
			inst = INST_DATA(cl, obj);
			Result = TRUE;

			if ( PeekQualifier() & (IEQUALIFIER_LALT | IEQUALIFIER_RALT) )
				{
				// --------------------··· Close window ------------------------··
				struct SM_CloseWindow	*message;

				inst->windowTask = ((struct ScaRootList *)obj)->rl_WindowTask;

				if( inst->windowTask )
					{
					message = (struct SM_CloseWindow *)SCA_AllocMessage( MTYP_CloseWindow, 0 );
					if( message )
						{
						PutAutoMsg( inst->windowTask->mt_WindowStruct->ws_MessagePort,
							&message->ScalosMessage.sm_Message, &replyPort );
						}
					}
				}
			}
		}
		break;

	default:
		Result = DoSuperMethodA(cl, obj, msg);
		break;
		}

	return Result;
}
M68KFUNC_END
///FrexxEndFold2.


//----------------------------------------------------------------------------

#if defined(__AROS__)

#include "aros/symbolsets.h"

ADD2EXPUNGELIB(closePlugin, 0);
ADD2OPENLIB(initPlugin, 0);

#endif

//----------------------------------------------------------------------------
