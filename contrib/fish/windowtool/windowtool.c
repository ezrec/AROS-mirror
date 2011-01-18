#define chip 
#define USHORT unsigned short
#include <intuition/iobsolete.h>

/*********************************************************/
/* Programmname		  : WindowTool      						*/
/* Autor               : Klaas Hermanns						*/
/* Aktuelle Version    : 1.00          						*/
/* Erstellungsdatum    : 25-05-92								*/
/* Compileranweisungen : lmk lmkfile    						*/
/*********************************************************/
#define VERSIONSTRING "1.00"
#define DATESTRING __DATE__" "__TIME__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <exec/exec.h>
#include <exec/types.h>
#include <graphics/displayinfo.h>
#include <graphics/text.h>
#include <graphics/clip.h>
#include <dos/datetime.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <intuition/intuitionbase.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <intuition/gadgetclass.h>
#include <libraries/commodities.h>
#include <libraries/gadtools.h>
#include <libraries/asl.h>
#include <libraries/reqtools.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/asl.h>
#include <proto/alib.h>
#include <proto/commodities.h>
#include <proto/icon.h>
#include <proto/reqtools.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <aros/debug.h>

#define DEFAULTPRIORITY 21

#define GD_Gadget0                             0
#define GD_Gadget1                             1
#define GD_Gadget2                             2
#define GD_Gadget3                             3
#define GD_Gadget4                             4
#define GD_Gadget5                             5
#define GD_Gadget6                             6
#define GD_Gadget7                             7
#define GD_Gadget8                             8
#define GD_Gadget9                             9
#define GD_Gadget10                             10
#define GD_Gadget11                             11
#define GD_GadgetSave                            50		/* Save */
#define GD_GadgetHide                            51		/* Hide */
#define GD_GadgetQuit                            52		/* Quit */
#define GD_GadgetCancel                          53		/* Cancel */

extern void activetofront(void);
extern void closewindow(void);
extern void bigwindow(void);
extern void smallwindow(void);
extern void zipwindow(void);
extern void activatefront(void);
extern void worktofront(void);

extern void checkstartup(int,char **,struct WBStartup *);

extern int windowopen;
extern struct Window        *Wnd        ;
extern struct Screen        *Scr        ;
extern APTR                  VisualInfo ;
extern struct Gadget        *GList      ;
extern struct Gadget        *Gadgets[14];
extern struct Menu          *Menus      ;
extern long InitStuff(void);
extern void CleanStuff(void);
extern struct IntuiText IText[];
extern char windowtitle[];

#define KEYNUMMER 12

struct Library *CxBase;
extern struct ExecBase *SysBase;
struct IntuitionBase *IntuitionBase;
struct Library *GadToolsBase;
struct Library *AslBase;
struct ReqToolsBase *ReqToolsBase;
struct GfxBase *GfxBase;
struct Library *IconBase;

extern struct WBStartup *WBenchMsg;

void ende(char *);
void openlibs(void);
void openrest(void);
void setkeys(void);
void switchstate(void);
void about(void);
void cycle(void);

BOOL state=0;						/* Status 0=Normal,~0=Dauerabfrage */
#define POPUPFLAG 1
#define NEWSETTINGSFLAG 2
int flags=0;
										/* Werte f¸r Commoditie */
struct MsgPort *mp;
struct Message *msg;
CxObj *broker,*filter[KEYNUMMER];
struct NewBroker nb=
{
	NB_VERSION,
	"WindowTool(e.BB)",
	"WindowTool V1.0 by Klaas Hemanns .",
	"Just a nice programm ...",
	NBU_NOTIFY|NBU_UNIQUE,COF_SHOW_HIDE,DEFAULTPRIORITY,0,0
};

										/* Werte f¸r Fensterabfrage */
struct IntuiMessage *mes;			/* Erhaltene Message */
long class;								/* Message Class */
short code;								/* Message Code */
struct Gadget *gad;

ULONG waitsignals;				/* Signale, auf die gewartet werden */

char oldkeys[KEYNUMMER][256];	/* Alte Tastenkombinationen (werden beim */
										/* ÷ffnen des Windows neu gesetzt */

char keys[KEYNUMMER][256]=		/* aktuelle Tastenkombinationen */
{
"rshift ralt j",								/* StopKey */
"shift alt DEL",								/* NewShell */
"control ralt lalt q",						/* Quit */
"lcommand b",									/* Cycle */
"lcommand v",									/* Work */
"lcommand g",									/* Closewindow */

"lcommand x",									/* ActivateFrontWindow */
"lcommand a",									/* Big Active Window */
"lcommand s",									/* Small Active Window */
"lcommand d",									/* Zip Active Window */
"lcommand h",									/* Popup */
"lcommand f"									/* ActiveToFront */
};

enum {	BK_ID,NS_ID,Q_ID,B_ID,V_ID,CL_ID,AF_ID,BW_ID,SW_ID,ZW_ID,PU_ID,ATF_ID};

char prgname[256];					/* Programm Pfad und Name */

char filename[34]="windowtool.config";	/* Default Settings */
char dirname[256]="s:";
char fullname[300];				/* Nur Zwischenspeicher */

struct TagItem reqtags[]=
{
{RT_ReqPos,REQPOS_CENTERSCR},{TAG_END,0}
};

BOOL filereq(int typ)
{
	struct rtFileRequester *filereq;
	char *match="#?.config";
	BOOL erg=FALSE;

	if ((filereq = rtAllocRequestA (RT_FILEREQ, NULL)))
	{
		rtChangeReqAttr(filereq,RTFI_Dir,dirname,RTFI_MatchPat,match,TAG_END);

		if(typ==0)
		{
		if (rtFileRequest (filereq, filename, "Save configuration",RTFI_Flags,FREQF_SAVE,TAG_END))
					erg=TRUE;
		}
		else
		{
		if (rtFileRequest (filereq, filename, "Load configuration",TAG_END))
					erg=TRUE;
		}
		strcpy(dirname,filereq->Dir);
		rtFreeRequest (filereq);
	}
	else
		rtEZRequest ("Error:\nNo memory for filerequester !", "Continue", NULL, NULL);
	return(erg);
}

void save()
{
	BPTR fh;
	strcpy(fullname,dirname);
	AddPart((UBYTE *)fullname,(UBYTE *)filename,300);
	if((fh=Open((UBYTE *)fullname,MODE_NEWFILE)))
	{
		int k;
		for(k=0;k<KEYNUMMER;k++)
		{
			Write(fh,"\"",1);
			Write(fh,keys[k],strlen(keys[k]));
			Write(fh,"\"\n",2);
		}
		Close(fh);
	}
	else rtEZRequest("Error:\nUnable to open\nconfigurationfile\n%s !","Continue",NULL, reqtags,fullname);
}
void saveas()
{
	if(filereq(0)==TRUE)	save();
}

extern char *tt[];		/* NAMEN DER TOOLTYPES-KEYS */

USHORT chip imagedata[]=
{
	0x0000,0x0000,0x0000,0x0400,0x0000,0x0000,0x0000,0x0C00,
	0x0000,0x0000,0x0000,0x0C00,0x0000,0x0000,0x0000,0x0C00,
	0x0000,0x0000,0x0010,0x0C00,0x0000,0x0000,0x0010,0x0C00,
	0x0000,0x0000,0x0010,0x0C00,0x0000,0x1F04,0x0010,0x0C00,
	0x0000,0x398E,0x0010,0x0C00,0x0000,0x018E,0x0010,0x0C00,
	0x0000,0x070E,0x0010,0x0C00,0x0000,0x0C04,0x0010,0x0C00,
	0x0000,0x0E04,0x0010,0x0C00,0x0000,0x0000,0x0010,0x0C00,
	0x0000,0x0E0E,0x0010,0x0C00,0x0000,0x0000,0x0010,0x0C00,
	0x0000,0x0000,0x0010,0x0C00,0x0000,0x0000,0x0010,0x0C00,
	0x0000,0x0000,0x0010,0x0C00,0x001F,0xFFFF,0xFFE0,0x0C00,
	0x0000,0x0000,0x0000,0x0C00,0x7FFF,0xFFFF,0xFFFF,0xFC00,
	0x0000,0x0000,0x0000,0x0000,0xFFFF,0xFFFF,0xFFFF,0xF800,
	0xD555,0x5555,0x5555,0x5000,0xD555,0x5555,0x5555,0x5000,
	0xD53F,0xFFFF,0xFFE5,0x5000,0xD57F,0xFFFF,0xFFC5,0x5000,
	0xD578,0x0000,0x0085,0x5000,0xD571,0x5555,0x5405,0x5000,
	0xD575,0x4051,0x5505,0x5000,0xD575,0x4451,0x5505,0x5000,
	0xD575,0x5451,0x5505,0x5000,0xD575,0x5051,0x5505,0x5000,
	0xD575,0x5151,0x5505,0x5000,0xD575,0x5151,0x5505,0x5000,
	0xD575,0x5555,0x5505,0x5000,0xD575,0x5151,0x5505,0x5000,
	0xD571,0x5555,0x5405,0x5000,0xD578,0x0000,0x0085,0x5000,
	0xD570,0x0000,0x0045,0x5000,0xD560,0x0000,0x0025,0x5000,
	0xD540,0x0000,0x0015,0x5000,0xD555,0x5555,0x5555,0x5000,
	0x8000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};
struct Image image =
{
	0,0,54,23,2,imagedata,3,0,0
};
void saveasinfo()
{
	char tooltypes[KEYNUMMER][256];
	long tooltypesadr[KEYNUMMER+1];
	int k;
	struct DiskObject *diskobj;
	struct DiskObject mydiskobj =
	{
		WB_DISKMAGIC,WB_DISKVERSION,
		{
			0,0,0,53,23,GADGIMAGE,RELVERIFY,BOOLGADGET,
			(APTR)&image,0,
			0,0,0,100,0
		},
		WBTOOL,0,0,NO_ICON_POSITION,NO_ICON_POSITION,0,0,4096
	};
	
	mydiskobj.do_ToolTypes=(char **)&tooltypesadr[0];
	
	for(k=0;k<KEYNUMMER;k++)
	{
		tooltypesadr[k]=(LONG)tooltypes[k];
		sprintf(tooltypes[k],"%s=%s",tt[k],keys[k]);
	}
	tooltypesadr[k]=0;
	
	if((diskobj=GetDiskObject(prgname)))
	{
		mydiskobj.do_Gadget.LeftEdge=diskobj->do_Gadget.LeftEdge;
		mydiskobj.do_Gadget.TopEdge=diskobj->do_Gadget.TopEdge;
		mydiskobj.do_Gadget.Width=diskobj->do_Gadget.Width;
		mydiskobj.do_Gadget.Height=diskobj->do_Gadget.Height;
		mydiskobj.do_Gadget.Flags=diskobj->do_Gadget.Flags;
		mydiskobj.do_Gadget.Activation=diskobj->do_Gadget.Activation;
		mydiskobj.do_Gadget.GadgetRender=diskobj->do_Gadget.GadgetRender;
		mydiskobj.do_Gadget.SelectRender=diskobj->do_Gadget.SelectRender;
		mydiskobj.do_Gadget.GadgetID=diskobj->do_Gadget.GadgetID;
		mydiskobj.do_Gadget.UserData=diskobj->do_Gadget.UserData;
		mydiskobj.do_CurrentX=diskobj->do_CurrentX;
		mydiskobj.do_CurrentY=diskobj->do_CurrentY;
		mydiskobj.do_StackSize=diskobj->do_StackSize;
	}
	
	PutDiskObject(prgname,&mydiskobj);
	if(diskobj)FreeDiskObject(diskobj);
}

void open(void)
{
	BPTR fh;
	strcpy(fullname,dirname);
	AddPart((UBYTE *)fullname,(UBYTE *)filename,300);
	if((fh=Open((UBYTE *)fullname,MODE_OLDFILE)))
	{
		char *mem;
		long memlen;
		Seek(fh,0,OFFSET_END);
		memlen=Seek(fh,0,OFFSET_BEGINNING);
		if(memlen==0)	rtEZRequest("Error:\nconfigurationfile\n%s\nis empty!","Continue",NULL, reqtags,fullname);
		else
		if((mem=AllocMem(memlen,0)))
		{
			int aktu,a,e,k;
			
			Read(fh,mem,memlen);
#warning CHECKME whats this???? To checks separated by commas?
/*			for(k=0,aktu=0;aktu<memlen,k<KEYNUMMER;aktu++,k++)*/
			for(k=0,aktu=0;aktu<memlen && k<KEYNUMMER;aktu++,k++)
			{
				for(;mem[aktu]!='\"';aktu++);
				if(aktu>memlen){	rtEZRequest("Error:\nError in\nconfigurationfile\n%s !","Continue",NULL, reqtags,fullname);break;}
				aktu++;
				a=aktu;
					for(;mem[aktu]!='\"';aktu++);
				if(aktu>memlen){	rtEZRequest("Error:\nError in\nconfigurationfile\n%s !","Continue",NULL, reqtags,fullname);break;}
				e=aktu;
				mem[e]=0;
				strcpy(keys[k],&mem[a]);
			}
			if(k==KEYNUMMER)setkeys();
			FreeMem(mem,memlen);
		}
		else rtEZRequest("Error:\nNo memory to load\nconfigurationfile\n%s !","Continue",NULL, reqtags ,fullname);
		Close(fh);
	}
	else rtEZRequest("Error:\nError while opening the configurationfile\n%s !","Continue",NULL, reqtags,fullname);
}
void openas()
{
	if(filereq(1)==TRUE)
				open();
}

void openinfo()
{
	struct DiskObject *infoobj;
	int k;
	char *key;

	if((infoobj=GetDiskObject(prgname)))
	{
		if((key=FindToolType(infoobj->do_ToolTypes,"SETTINGS")))
			{
				strcpy(filename,FilePart(key));
				*((char *)PathPart(key))=0;
				strcpy(dirname,key);
				open();
			}
		for(k=0;k<KEYNUMMER;k++)
		if((key=FindToolType(infoobj->do_ToolTypes,tt[k])))
						strcpy(keys[k],key);
		setkeys();
		FreeDiskObject(infoobj);
	}
	else rtEZRequest("Error:\nUnable to open %s.info !","Continue",NULL,reqtags,prgname);
}
		/* ÷ffnet Window neu */
void popup(void)
{
	int k;
	if(windowopen==FALSE) for(k=0;k<KEYNUMMER;k++)			/* KEYS SICHERN */
										strcpy(oldkeys[k],keys[k]);

	if(InitStuff()){
				rtEZRequest(	"Error:\nUnable to open HotkeyPrefsWindow !",
						"Continue",NULL, reqtags,NULL);
						}
	else
	{
	 	waitsignals= (1L<<Wnd->UserPort->mp_SigBit) | 
				(1<<mp->mp_SigBit) | SIGBREAKF_CTRL_C ;
		ScreenToFront(Scr);
	}
}
		/* Schlieﬂt Window */
void popdown(void)
{
	CleanStuff();
	waitsignals=(1<<mp->mp_SigBit) | SIGBREAKF_CTRL_C ;
}

void setkeys(void)
{
	int k;
	for(k=0;k<KEYNUMMER;k++)
	{
		if(windowopen==TRUE)GT_SetGadgetAttrs(Gadgets[k],Wnd,0,GTST_String,keys[k],TAG_DONE);
		DeleteCxObjAll(filter[k]);
		filter[k]=HotKey(keys[k],mp,k);
		AttachCxObj(broker,filter[k]);
	}
	if(windowopen==TRUE)
	{
		strcpy(&windowtitle[26],keys[10]);
		SetWindowTitles(Wnd,windowtitle,(char *)-1);
	}
}

void quit(void)
{
	if(rtEZRequestA("Quit WindowTool ?\n","Yes|No",
								NULL, NULL,NULL) )ende(0);;
}
			/* About Requester */
void about(void)
{
	rtEZRequest(	"WindowTool V"VERSIONSTRING" ("DATESTRING")\n"
						"This program is Giftware\n"
						"Copyright 1992 by Klaas Hermanns\n"
						"(Weberstraﬂe 42/4190 Kleve/Germany)",
	"Continue",NULL, reqtags,NULL);
}


int main(argc,argv)
int argc;
char *argv[];
{
	ULONG signals;
	LONG id,k;

	openlibs();

	checkstartup(argc,argv,WBenchMsg);
	
	openrest();

	if(flags&POPUPFLAG)
	{
		popup();
		waitsignals= (1L<<Wnd->UserPort->mp_SigBit) | 
				(1<<mp->mp_SigBit) | SIGBREAKF_CTRL_C ;
	}
	else waitsignals = (1<<mp->mp_SigBit) | SIGBREAKF_CTRL_C ;

	if(flags&NEWSETTINGSFLAG)open();
	
	for(;;)
	{
		if(state==0)signals=Wait(waitsignals);
		else	while(0==(signals=CheckSignal( waitsignals )));

		if(signals & SIGBREAKF_CTRL_C )	quit();
		if(Wnd && (signals & 1L<<Wnd->UserPort->mp_SigBit))
	while(1)
		{
			if(windowopen==FALSE)break;
			if(!(mes=(struct IntuiMessage *)GT_GetIMsg(Wnd->UserPort)))break;
			
			class=mes->Class;
			code=mes->Code;
			gad=(struct Gadget *)mes->IAddress;
			GT_ReplyIMsg(mes);
			switch(class)
			{
				case IDCMP_REFRESHWINDOW:GT_BeginRefresh(Wnd);
												GT_RefreshWindow(Wnd,0);
									PrintIText( Wnd->RPort, IText, 0l, 0l );
												GT_EndRefresh(Wnd,TRUE);
												break;
				case IDCMP_CHANGEWINDOW:
												break;
				case IDCMP_CLOSEWINDOW:
										popdown();
										break;
				case IDCMP_GADGETUP:
											switch(gad->GadgetID)
												{
													case	GD_GadgetSave:save();break;	/*SAVE*/
													case	GD_GadgetHide:popdown();break;
													case	GD_GadgetQuit:quit();break;
													case	GD_GadgetCancel:
															for(k=0;k<KEYNUMMER;k++)/* KEYS ZUR‹CK */
															{
																strcpy(keys[k],oldkeys[k]);
																DeleteCxObjAll(filter[k]);
																filter[k]=HotKey(keys[k],mp,k);
																AttachCxObj(broker,filter[k]);
															}
															popdown();
															break;
													default:if(gad->GadgetID<KEYNUMMER)
																{
																	k=gad->GadgetID;
																	DeleteCxObjAll(filter[k]);
																	strcpy(keys[k],((struct StringInfo *)gad->SpecialInfo)->Buffer);
																	filter[k]=HotKey(keys[k],mp,k);
																	AttachCxObj(broker,filter[k]);
																	ActivateGadget(Gadgets[k==11 ? 0 : k+1],Wnd,0);
																}
																if(gad->GadgetID==GD_Gadget10)
																	{
																		strcpy(&windowtitle[26],keys[10]);
																		SetWindowTitles(Wnd,windowtitle,(char *)-1);
																	}
															break;
												}
										break;
				case IDCMP_MENUPICK:	
								switch(MENUNUM(code))
									{
									case 0:switch(ITEMNUM(code))
										{
										case 0:for(k=0;k<KEYNUMMER;k++)
													GT_SetGadgetAttrs(Gadgets[k],Wnd,0,GTST_String,"",TAG_DONE);
												break;
										case 1:openas();break;			/* OPEN */
										case 2:openinfo();break;	/* OPEN INFO */
										case 4:save();break;			/* SAVE */
										case 5:saveas();break;		/* SAVE AS */
										case 6:saveasinfo();break;	/* SAVE AS INFO */
										case 8:about();				/* About */
												break;
										case 10:popdown();	 		/* HIDE */
												break;
										case 11:
											for(k=0;k<KEYNUMMER;k++)/* KEYS ZUR‹CK */
												{
													strcpy(keys[k],oldkeys[k]);
													DeleteCxObjAll(filter[k]);
													filter[k]=HotKey(keys[k],mp,k);
													AttachCxObj(broker,filter[k]);
												}
												popdown();
												break;
										case 13:quit();					/* QUIT */
												break;
										default:break;
										}
									default:break;
									}
								break;
				case IDCMP_VANILLAKEY:
						switch(code)
						{	
								case 's':
								case 'S':save();
										break;
								case 'h':
								case 'H':popdown();
										break;
								case 'c':
								case 'C':for(k=0;k<KEYNUMMER;k++)/* KEYS ZUR‹CK */
										{
											strcpy(keys[k],oldkeys[k]);
											DeleteCxObjAll(filter[k]);
											filter[k]=HotKey(keys[k],mp,k);
											AttachCxObj(broker,filter[k]);
										}
										popdown();
										break;
								case 'q':
								case 'Q':quit();
										break;
								case 'o':
								case 'O':ActivateGadget(Gadgets[0],Wnd,0);break;
								case 'n':
								case 'N':ActivateGadget(Gadgets[1],Wnd,0);break;
								case 'u':
								case 'U':ActivateGadget(Gadgets[2],Wnd,0);break;
								case 'y':
								case 'Y':ActivateGadget(Gadgets[3],Wnd,0);break;
								case 'r':
								case 'R':ActivateGadget(Gadgets[4],Wnd,0);break;
								case 'l':
								case 'L':ActivateGadget(Gadgets[5],Wnd,0);break;
								case 'a':
								case 'A':ActivateGadget(Gadgets[6],Wnd,0);break;
								case 'b':
								case 'B':ActivateGadget(Gadgets[7],Wnd,0);break;
								case 'm':
								case 'M':ActivateGadget(Gadgets[8],Wnd,0);break;
								case 'z':
								case 'Z':ActivateGadget(Gadgets[9],Wnd,0);break;
								case 'w':
								case 'W':ActivateGadget(Gadgets[10],Wnd,0);break;
								case 't':
								case 'T':ActivateGadget(Gadgets[11],Wnd,0);break;
						}
				default:break;
			}
		}

		if(signals & (1<<mp->mp_SigBit))
		{
			while((msg=(struct Message *)GetMsg(mp)))
			{
				id=CxMsgID((CxMsg *)msg);

				switch( CxMsgType((CxMsg*)msg) )
				{
					case CXM_IEVENT:
							ReplyMsg(msg);
							switch(id)
							{
								case V_ID:worktofront();
										break;
								case AF_ID:activatefront();
										break;
								case B_ID:cycle();
										break;
								case BK_ID:switchstate();
										break;
								case Q_ID:quit();
										break;
								case NS_ID:system("newshell");
										break;
								case BW_ID:bigwindow();
										break;
								case SW_ID:smallwindow();
										break;
								case ZW_ID:zipwindow();
										break;
								case PU_ID:popup();
										break;
								case ATF_ID:activetofront();
										break;
								case CL_ID:closewindow();
										break;
								default:break;
							}
						break;
					case CXM_COMMAND:
							ReplyMsg(msg);
							switch(id)
							{
								case CXCMD_DISABLE:ActivateCxObj(broker,0);break;
								case CXCMD_ENABLE:ActivateCxObj(broker,-1);break;
								case CXCMD_KILL:quit();break;
								case CXCMD_UNIQUE:
								case CXCMD_APPEAR:popup();break;
								case CXCMD_DISAPPEAR:popdown();break;
								default:break;
							}
							break;
					default:
							ReplyMsg(msg);
							break;
				}
			}
		}
	}
	return 0;
}

void openlibs(void)
{
	if(!(AslBase=(struct Library *)OpenLibrary("asl.library",37)))
					ende("asl.library not found !");
	if(!(GadToolsBase=(struct Library *)OpenLibrary("gadtools.library",37)))
					ende("gadtools.library not found !");
	if(!(GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",37)))
					ende("graphics.library not found !");
	if(!(IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",37)))
					ende("Intuition.library not found !");
	if(!(	ReqToolsBase=(struct ReqToolsBase *)OpenLibrary(REQTOOLSNAME,REQTOOLSVERSION)))
			ende("reqtools.library not found !");
	if(!(CxBase=(struct Library *)OpenLibrary("commodities.library",37)))
			ende("commodities.library not found !");
	if(!(IconBase=OpenLibrary("icon.library",37)))
					ende("icon.library not found !");
}

void openrest(void)
{
	int k;
	if(!(mp=(struct MsgPort *)CreateMsgPort()))ende("Unable to create MsgPort !");
	nb.nb_Port=mp;

	for(k=0;k<KEYNUMMER;k++)
	{
			filter[k]=HotKey(keys[k],mp,k);
	}
	
	
	if(!(broker=(CxObj *)CxBroker(&nb,0)))ende(0);/* WT l‰uft schon */

	for(k=0;k<KEYNUMMER;k++)
	AttachCxObj(broker,filter[k]);

	ActivateCxObj(broker,-1);
}

void ende(char *text)
{
	if(text)rtEZRequest(text,"Continue",NULL, reqtags,NULL);

	CleanStuff();

	if(broker)DeleteCxObjAll(broker);
	if(mp)DeleteMsgPort(mp);

	if(CxBase)CloseLibrary(CxBase);
	if(IntuitionBase)
	{
		DisplayBeep(0);
		CloseLibrary((struct Library *)IntuitionBase);
	}
	if(ReqToolsBase)CloseLibrary ((struct Library *)ReqToolsBase);
	if(GfxBase)CloseLibrary ((struct Library *)GfxBase);
	if(GadToolsBase)CloseLibrary(GadToolsBase);
	if(AslBase)CloseLibrary(AslBase);
	if(IconBase)CloseLibrary(IconBase);

	exit(0);
}

LONG oldpri=4;
void switchstate(void)
{
	state=~state;
	oldpri=SetTaskPri(FindTask(0),oldpri);
}
