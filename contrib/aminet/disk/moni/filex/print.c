#ifdef USE_PROTO_INCLUDES
#include <intuition/intuition.h>
#include <libraries/reqtools.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/reqtools.h>
#include <clib/alib_protos.h>

#include <stdio.h>
#endif

#include <devices/printer.h>

#include "filexstructs.h"
#include "filexstrings.h"

#include "allprotos.h"

union printerIO
{
	struct IOStdReq ios;
	struct IODRPReq iodrp;
	struct IOPrtCmdReq iopc;
};

void MyPrint(UBYTE *start, long laenge, long DisplayTyp, struct Window *Wnd )
{
	struct MsgPort *PrinterMP;
	union printerIO *PrintIO;
	
	if( !laenge )
	{
		DisplayLocaleText( MSG_INFO_PRINT_NOTHINGTOPRINT );
	}
	else
	if((PrinterMP=CreateMsgPort()))
	{
		if((PrintIO=(union printerIO *) CreateExtIO(PrinterMP,sizeof(union printerIO))))
		{
			if(0==OpenDevice("printer.device",0L,(struct IORequest *)PrintIO,0))
			{
				UWORD status;

				PrintIO->ios.io_Data=&status;
				PrintIO->ios.io_Command=PRD_QUERY;

				DoIO((struct IORequest *)PrintIO);

					/* Prüfen, ob Drucker bereit */
					/* Endweder Parallel und nicht offline oder seriell */

				if( ((PrintIO->ios.io_Actual==1)&&(!(status&0x300)))
					|| (PrintIO->ios.io_Actual!=1))
				{
					BOOL RequesterOpened = FALSE;
				
						/* Reqtoolsrequesterdaten */
				
					struct rtHandlerInfo *hinfo;
				
						/* Intuitionrequesterdaten */
				
					struct Window *ReqWnd;
					struct EasyStruct Easy;
		
						/* Requester öffnen */
				
					if( UseAsl )
					{
							/* Werte für den Intuitionrequester setzen */

						Easy.es_StructSize = sizeof(struct EasyStruct);
						Easy.es_Flags = 0;
						Easy.es_Title = GetStr( MSG_WINDOWTITLE_INFO );
						Easy.es_GadgetFormat	= GetStr( MSG_GADGET_STOPPRINTING );
						Easy . es_TextFormat = GetStr( MSG_INFO_PRINT_PRINTING );

						if(( ReqWnd = BuildEasyRequestArgs( Wnd, &Easy, 0, NULL )))
							RequesterOpened = TRUE;
					}
					else
					{
						if(CALL_HANDLER == rtEZRequestTags(GetStr(MSG_INFO_PRINT_PRINTING), GetStr(MSG_GADGET_STOPPRINTING), 0L, 0L, RTEZ_ReqTitle, GetStr( MSG_WINDOWTITLE_INFO ), (IPTR)RT_ReqHandler, (IPTR)&hinfo,  Wnd ? RT_Window : TAG_IGNORE, Wnd, RT_Underscore, '_', TAG_DONE))
							RequesterOpened = TRUE;
					}
				
					if( RequesterOpened )
					{
						register short zeichen = 16;
						ULONG sigs = 0;
						LONG off=0,k;
						UBYTE buffer[9+1+35+1+16+2];

						PrintIO->ios.io_Length=-1;
						PrintIO->ios.io_Data="\033#1";
						PrintIO->ios.io_Command=CMD_WRITE;
		
						DoIO((struct IORequest *)PrintIO);


						buffer[9]=buffer[18]=buffer[27]=buffer[36]=buffer[45]=' ';
						buffer[62]='\n';

						if(laenge-off<0)zeichen=off-laenge;

						sprintf(buffer,"%08lx: ",(unsigned long)off);

						for(k=0;k<zeichen;k++)
						{
							ByteToString(start[off+k],buffer+2*k+k/4+10);
							buffer[k+10+36]=displaytab[DisplayTyp][start[k+off]];
						}

						PrintIO->ios.io_Length=63;
						PrintIO->ios.io_Data=buffer;
						PrintIO->ios.io_Command=CMD_WRITE;
		
						SendIO((struct IORequest *)PrintIO);

						off+=16;

						while( RequesterOpened && ( off < laenge ))
						{
							if(!UseAsl)
							{
								if(!hinfo->DoNotWait)
									sigs = Wait (hinfo->WaitMask|(1L<< PrinterMP->mp_SigBit));
							}
							else
								sigs = Wait ( 1L<< PrinterMP->mp_SigBit );

							if(sigs&(1L << PrinterMP->mp_SigBit))
							{
								while(GetMsg(PrinterMP));

								zeichen = 16;
								if(laenge-off-16<0)zeichen=laenge-off;

								sprintf(buffer,"%08lx: ",(unsigned long)off);

								for(k=0;k<zeichen;k++)
								{
									ByteToString(start[off+k],buffer+2*k+k/4+10);
									buffer[k+10+36]=displaytab[DisplayTyp][start[k+off]];
								}

								if(zeichen!=16)
								{
									for(k=zeichen;k<16;k++)
									{
										*(buffer+2*k+k/4+10)=' ';
										*(buffer+2*k+k/4+10+1)=' ';
										*(buffer+k+10+36)=' ';
									}
								}	

								PrintIO->ios.io_Length=63;
								PrintIO->ios.io_Data=buffer;
								PrintIO->ios.io_Command=CMD_WRITE;
			
								SendIO((struct IORequest *)PrintIO);

								off+=16;
							}

							if( UseAsl )
							{
								if( -2 != SysReqHandler( ReqWnd, NULL, FALSE ))
									RequesterOpened = FALSE;
							}
							else						
							{
								if( !( CALL_HANDLER == rtReqHandler( hinfo, sigs, TAG_END )))
									RequesterOpened = FALSE;
							}
						}

						if( UseAsl )
						{
							if( ReqWnd )
								FreeSysRequest( ReqWnd );
						}

						if( RequesterOpened )
						{
							if( !UseAsl )
							{
								ULONG ret;

								do
								{
									ret = rtReqHandler( hinfo, 0L, RTRH_EndRequest, REQ_OK, TAG_END );
								} while( ret == CALL_HANDLER );
							}

							WaitIO((struct IORequest *)PrintIO);
						}
						else 
						{
							AbortIO((struct IORequest *)PrintIO);
							WaitIO((struct IORequest *)PrintIO);
						}
					}
				}
				else
					DisplayLocaleText(MSG_INFO_PRINT_PRINTERNOTREADY);

				CloseDevice((struct IORequest *)PrintIO);
			}
			DeleteExtIO((struct IORequest *)PrintIO);
		}
		DeleteMsgPort(PrinterMP);
	}
}
