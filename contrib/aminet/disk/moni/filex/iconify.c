#ifdef USE_PROTO_INCLUDES
#include <workbench/startup.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/wb.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#endif

#include <string.h>
#include "filexstructs.h"
#include "allprotos.h"
#include "filexstrings.h"

static struct DiskObject *dobj;		/* Iconifydaten */
static struct AppIcon *appicon;
static struct MsgPort *AppIconPort;

void DoAppIconMessage( void )
{
	struct AppMessage *appMsg;
	long k;

	while( AppIconPort && (appMsg = (struct AppMessage *)GetMsg(AppIconPort)))
	{
		if( appMsg->am_Type == 8 )		/* AppIcon aktiviert */
		{
			UBYTE buffer[256] = "";

			if( appMsg->am_NumArgs == 0 )
			{
				RemoveIconify();
			}
			else
			{
				struct WBArg *argptr;

				argptr = appMsg->am_ArgList;

				for( k = 0; ( k < appMsg->am_NumArgs ) && ( mainflags & MF_ICONIFIED ); k++, argptr++)
				{
					if( strlen( argptr->wa_Name ))
					{
						NameFromLock( argptr->wa_Lock, buffer, 256 );

						AddPart( buffer, argptr->wa_Name, 256 );

						if(strlen(buffer))
						{
							if( QuitView( 5, AktuDD ))
								MyOpen(buffer,AktuDD);
						}

						RemoveIconify();
					}
				}
			}
		}

		ReplyMsg((struct Message *)appMsg);
	}
}

static long premf;
		
BOOL MakeIconify( void )
{
	if( mainflags & MF_ICONIFIED )
		return( TRUE );

	premf = mainflags;

	if( mainflags & MF_CLIPCONV )
		CloseClipConv();

	if( mainflags & MF_CALC )
		CloseCalc();

	if( CloseDisplay())
	{
			/* AppMessagePort erstellen */

		if((AppIconPort=CreateMsgPort()))
		{
			MyAddSignal( 1L << AppIconPort->mp_SigBit, &DoAppIconMessage);

			if( !( dobj = GetDiskObjectNew( programname )))
			{
				BPTR NewLock;
	
				if((NewLock = Lock("PROGDIR:",ACCESS_READ)))
				{
					BPTR OldLock;
	
					OldLock = CurrentDir(NewLock);
	
					dobj = GetDiskObjectNew( FilePart( programname ));
	
					CurrentDir(OldLock);
	
					UnLock(NewLock);
				}
			}
		
			if( dobj )
			{
				dobj->do_Type = 0;
		
				if(( appicon = AddAppIconA( 0, 0, GetStr(MSG_APPICONNAME), AppIconPort, BNULL, dobj, NULL )))
				{
					mainflags |= MF_ICONIFIED;
	
					return( TRUE );
				}
		
				FreeDiskObject( dobj );
			}

				/* AppIconPort freigeben */

			MyRemoveSignal( 1L << AppIconPort->mp_SigBit );
			DeleteMsgPort( AppIconPort );
			AppIconPort = NULL;
		}

		OpenDisplay();
	}

	if( premf & MF_CLIPCONV )
		OpenClipConv();

	if( premf & MF_CALC )
		OpenCalc();

	return( FALSE );
}

void RemoveIconify( void )
{
	if(! ( mainflags & MF_ICONIFIED ))
		return;

	RemoveAppIcon(appicon);

		/* AppIconPort freigeben */

	MyRemoveSignal( 1L << AppIconPort->mp_SigBit );
	DeleteMsgPort( AppIconPort );
	AppIconPort = NULL;

	FreeDiskObject(dobj);

	OpenDisplay();

	if( premf & MF_CLIPCONV )
		OpenClipConv();

	if( premf & MF_CALC )
		OpenCalc();

	ActivateWindow( AktuDI->Wnd );
	WindowToFront( AktuDI->Wnd );

	mainflags &= ~MF_ICONIFIED;
}
