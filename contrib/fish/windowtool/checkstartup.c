#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <proto/icon.h>
#include <proto/reqtools.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <libraries/commodities.h>
#include <libraries/reqtools.h>
#include <dos/rdargs.h>
//#include <pragmas/reqtools.h>
#include <string.h>
#include <stdio.h>

extern struct TagItem reqtags[];

char text[300];
BPTR fh;
#define KEYNUMMER 12

enum {	BK_ID,NS_ID,Q_ID,B_ID,V_ID,CL_ID,AF_ID,BW_ID,SW_ID,ZW_ID,PU_ID,ATF_ID};

extern char keys[KEYNUMMER][256];
extern struct NewBroker nb;
#define POPUPFLAG 1
#define NEWSETTINGSFLAG 2
extern int flags;

extern char filename[];
extern char dirname[];
extern char prgname[];

char *tt[KEYNUMMER] =
{
"CX_STOPKEY",
"CX_NEWSHELLKEY",
"CX_QUITKEY",
"CX_CYCLEKEY",
"CX_WORKBENCHKEY",
"CX_CLOSEWINDOW",

"CX_FRONTKEY",
"CX_MAKEBIGKEY",
"CX_MAKESMALLKEY",
"CX_ZIPKEY",
"CX_POPKEY",
"CX_ACTIVETOFRONT"
};

char *template=
"CX_PRIORITY/N,CX_POPUP/S,SETTINGS/K,"
		/* Alle Keys */
"CX_STOPKEY/K,CX_NEWSHELLKEY/K,CX_QUITKEY/K,CX_CYCLEKEY/K,CX_WORKBENCHKEY/K,CX_CLOSEWINDOW/K,CX_FRONTKEY/K,CX_MAKEBIGKEY/K,CX_MAKESMALLKEY/K,CX_ZIPKEY/K,CX_POPKEY/K,CX_ACTIVETOFRONT/K";

struct cliresults 
{
	long pri;
	long popup;
	char *settings;
	char *keys[KEYNUMMER];
};
struct cliresults clir={0,0,0,{0,0,0,0,0,0,0,0,0,0}};

void ende(char *);


void checkstartup(int argc,char *argv[],struct WBStartup *msg)
{
	int k;
	char *key;

	if(argc)
	{
		struct RDArgs *cliargs;

		GetCurrentDirName(prgname,300);
		AddPart(prgname,argv[0],300);
		
		if(!(cliargs=ReadArgs(template,(LONG *)&clir,0)))
			{
			fh=Output();
			sprintf(text,"WindowTool V1.0 : Error in Commandline !\nUSAGE: %s\n",template);
			Write(fh,text,strlen(text));
			ende(0);
			}

		if(clir.pri!=0)nb.nb_Pri=(BYTE)clir.pri;
		if(clir.settings)
			{
				flags=flags|NEWSETTINGSFLAG;
				strcpy(filename,FilePart(clir.settings));
				*((char *)PathPart(clir.settings))=0;
				strcpy(dirname,clir.settings);
			}					
		if(clir.popup!=0)flags=flags|POPUPFLAG;

		for(k=0;k<KEYNUMMER;k++)
			if(clir.keys[k])strcpy(keys[k],clir.keys[k]);

		if(cliargs)FreeArgs(cliargs);
	}
	else
	{
		GetCurrentDirName(prgname,300);
		AddPart(prgname,msg->sm_ArgList->wa_Name,300);

		if(msg->sm_NumArgs!=0)				/* Falls Tooltypes da */
		{
			BPTR olddir;
			struct DiskObject *infoobj;
			
			olddir=CurrentDir(msg->sm_ArgList->wa_Lock);
			
			if((infoobj=GetDiskObject(msg->sm_ArgList->wa_Name)))
			{
				if((key=FindToolType(infoobj->do_ToolTypes,"SETTINGS")))
					{
						flags=flags|NEWSETTINGSFLAG;
						strcpy(filename,FilePart(key));
						*((char *)PathPart(key))=0;
						strcpy(dirname,key);
					}
				for(k=0;k<KEYNUMMER;k++)
				if((key=FindToolType(infoobj->do_ToolTypes,tt[k])))
								strcpy(keys[k],key);
				if((key=FindToolType(infoobj->do_ToolTypes,"CX_PRIORITY")))
						{
							int dummy;
							sscanf(key,"%ld",&dummy);
							nb.nb_Pri=(BYTE)dummy;
						}
				if((key=FindToolType(infoobj->do_ToolTypes,"CX_POPUP")))
					if(stricmp(key,"YES")==0)flags=flags|POPUPFLAG;
				FreeDiskObject(infoobj);
			}
			CurrentDir(olddir);
		}
	}
}
