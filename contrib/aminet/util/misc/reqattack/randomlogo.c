#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>

#include <exec/memory.h>
#include <exec/semaphores.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <dos/exall.h>

#include <clib/alib_protos.h>
#include <string.h>
#include <stdio.h>

#include "randomlogo.h"
#include "reqlist.h"
#include "misc.h"
#include "global.h"

#include "asmmisc.h"

#define CONNAME "CON:0/40/200/50/RAPrefsMUI launch.../AUTO/CLOSE/WAIT"
#define EADATASIZE 4096

ULONG randomlogomask;

static struct MsgPort randomlogoport;

void InitRandomLogo(void)
{
	randomlogoport.mp_Node.ln_Type = NT_MSGPORT;
	randomlogoport.mp_Flags = PA_SIGNAL;
	randomlogoport.mp_SigBit = AllocSignal(-1);
	randomlogoport.mp_SigTask = FindTask(0);
	NewList(&randomlogoport.mp_MsgList);

	randomlogomask = 1L << randomlogoport.mp_SigBit;
}

void CleanupRandomLogo(void)
{
	FreeSignal(randomlogoport.mp_SigBit);
}

ULONG SendRandomLogo(char *logo,BYTE code,ULONG *data)
{
	struct RandomLogoMsg msg;
	struct MsgPort replyport;

	replyport.mp_Node.ln_Type = NT_MSGPORT;
	replyport.mp_Flags = PA_SIGNAL;
	replyport.mp_SigBit = SIGB_SINGLE;
	replyport.mp_SigTask = FindTask(0);
	NewList(&replyport.mp_MsgList);

	SetSignal(0,SIGF_SINGLE);

	msg.msg.mn_ReplyPort = &replyport;
	msg.msg.mn_Length = sizeof(msg);

	msg.code = code;
	msg.filename = logo;
	msg.data = data;

	PutMsg(&randomlogoport,&msg.msg);
	WaitPort(&replyport);

	return (ULONG)msg.result;
}

void HandleRandomLogo(void)
{
	struct RandomLogoMsg *msg;

	char 		path[256],tokenized[520];
	struct ExAllControl *eac;
	struct ExAllData *ead;

    	BPTR lock, rafile; /* stegerg: BUGFIX, were of type ULONG?? */
	ULONG *eadata;
	LONG iswild,randomno,more;

	char *raprefsdata,*tex,*tex2;
	struct ReqNode *reqnode;

	ULONG argt[] = {(ULONG)raprefspath};

	while ((msg = (struct RandomLogoMsg*)GetMsg(&randomlogoport)))
	{
				msg->result=FALSE;
				switch (msg->code)
				{
					case RA_RANDOMIZE:

						//1st we check if there's some token in the string...
						iswild = ParsePatternNoCase(msg->filename,tokenized,520);

						if (iswild == 1)
						{
							strncpy(path,msg->filename,(UBYTE *)PathPart(msg->filename)-(UBYTE *)msg->filename); /* BUGFIX stegerg: casts were (UBYTE)!?? */
							path[(UBYTE *)PathPart(msg->filename)-(UBYTE *)msg->filename]=0; //strncpy doesn't treminate strings with 0x0 (!?)

							if (strlen(FilePart(msg->filename))>0) //in dir search...
							{
								if (ParsePatternNoCase(FilePart(msg->filename),tokenized,520) != 1) break;
								if ((lock = Lock(path,ACCESS_READ)))
								{
									if ((eac = AllocDosObject(DOS_EXALLCONTROL,0)))
									{
										if ((eadata = MiscAllocVec(EADATASIZE)))
										{
											ead = (struct ExAllData *)eadata;
											eac->eac_LastKey = 0;
											eac->eac_MatchString = tokenized;
											more = ExAll(lock,(struct ExAllData *)eadata,EADATASIZE,ED_NAME,eac);
											if (eac->eac_Entries)
											{
												randomno = RangeRand(eac->eac_Entries);
												while(ead)
												{
													if (randomno == 0)
													{
														strcpy(msg->filename,path);
														AddPart(msg->filename,ead->ed_Name,256);
														msg->result = (ULONG *)TRUE;
														break;
													}
													randomno--;
													ead = ead->ed_Next;
												}
											}
											if (more) ExAllEnd(lock,(struct ExAllData *)eadata,EADATASIZE,ED_NAME,eac);
											MiscFreeVec(eadata);
										}
										FreeDosObject(DOS_EXALLCONTROL,eac);
									}
									UnLock(lock);
								}
							}
						}
						break;

					case RA_APPICON:
						msg->result = 0;
						if (msg->filename[0])
						{
							msg->result = (ULONG *)GetDiskObject(msg->filename);
						}
						if (!(msg->result)) msg->result = (ULONG *)GetDiskObjectNew("ENV:ReqAttack");
						break;

					case RA_RUNRAPREFS:
								reqnode = (struct ReqNode *)msg->filename;
								raprefsdata = MiscAllocVec(2000);
								if (raprefsdata)
								{
								if (!(progport.prefsappactive))
								{
									rafile = Open("T:RAPrefsRequest",MODE_NEWFILE);
									if (rafile)
									{
										tex=raprefsdata;
										tex2=reqnode->title;
	//preprocess data - all 'NEXT LINE' and ',' to '#?'
									if (tex2)
									{
										while(tex2[0])
										{
											if ((ULONG)tex > (ULONG)raprefsdata+1990)
											{
												tex[0]=0x23;tex[1]=0x3F;tex[2]=0;tex+=2;break;
											}

											tex[0]=tex2[0];
											if (tex2[0]==0xA)
											{
												if (tex2[1]==0)
												{
													tex[0]=0x23;tex[1]=0x3F;tex[2]=0;tex+=2;break;
												}else{
													tex[0]=0x23;tex++;tex[0]=0x3F;
												}
											};
											if (tex2[0]==0x2C)
											{
												tex[0]=0x23;tex++;tex[0]=0x3F;
											};
											tex++;tex2++;
										}
									}
										tex[0]=0xA;tex++;
										tex2=reqnode->bodytext;
									if (tex2)
									{
										while(tex2[0])
										{
											if ((ULONG)tex > (ULONG)raprefsdata+1990)
											{
												tex[0]=0x23;tex[1]=0x3F;tex[2]=0;tex+=2;break;
											}

											tex[0]=tex2[0];
											if (tex2[0]==0xA){if (tex2[1]==0){tex[0]=0x23;tex[1]=0x3F;tex[2]=0;tex+=2;break;}else{tex[0]=0x23;tex++;tex[0]=0x3F;}};
											if (tex2[0]==0x2C){tex[0]=0x23;tex++;tex[0]=0x3F;};
											tex++;tex2++;
										}
									}

										tex[0]=0xA;tex++;tex[0]=0;
										FPuts(rafile,raprefsdata);
										tex2 = (char *) msg->data;
										if (tex2) FPuts(rafile,tex2);
										Close(rafile);
									    #ifdef NO_ASM
									    {
									    	STRPTR strptr = raprefsdata;
										
										RawDoFmt("%s RA",argt,
											 (APTR)Copy_PutChFunc,&strptr);
									    }
									    #else
										RawDoFmt("%s RA",argt,
											 (APTR)Copy_PutChFunc,raprefsdata);
									    #endif    
									
										//sprintf(raprefsdata,"%s RA",raprefspath);
										rafile = Open(CONNAME,MODE_OLDFILE);
										if (rafile)
										{
											if ((SystemTags(raprefsdata,SYS_Input,(ULONG)rafile,SYS_Output,NULL,SYS_UserShell,TRUE,SYS_Asynch,TRUE,TAG_DONE))==0)
											{
												progport.prefsappactive = TRUE;
											}
										}
									}
										MiscFreeVec(raprefsdata);
								} //prefsappactive
								} //raprefsdata
						break;

					case RA_KEYPREFS: //run RAPrefsMUI without edit data :)
						if (!(progport.prefsappactive))
						{
						rafile = Open(CONNAME,MODE_OLDFILE);
						if (rafile)
						{
							if ((SystemTags(raprefspath,SYS_Input,(ULONG)rafile,SYS_Output,NULL,SYS_UserShell,TRUE,SYS_Asynch,TRUE,TAG_DONE))==0)
							{
								progport.prefsappactive = TRUE;
							}
						}
						}
						break;
				}
		ReplyMsg(&msg->msg);
	}//while getmsg
}

