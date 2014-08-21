

#define SYSTEM_PRIVATE
#define SYSTEM_REALLY_PRIVATE
#include <stdio.h>
#include <stdlib.h>

#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/locale.h>
#include <proto/icon.h>

#if 0
#include <exec/execbase.h>
#endif

#include <private/vapor/vapor.h>
#include <libraries/mui.h>
#include <workbench/startup.h>
#include <clib/alib_protos.h>
#include <clib/rexxsyslib_protos.h>

#include "util.h"
#include "version.h"
#include "mcc/classes.h"

#include "system/system.h"

#include "mcc/pageview_class.h"
#include "mcc/documentview_class.h"
#include "mcc/outlineview_class.h"
#include "mcc/documentlayout_class.h"
#include "mcc/toolbar_class.h"
#include "mcc/renderer_class.h"
#include "mcc/title_class.h"
#include "mcc/search_class.h"
#include "mcc/thumbnaillist_class.h"
#include "mcc/lay_class.h"
#include "mcc/annotation_class.h"
#include "application.h"
#include "logger.h"
#include "window.h"
#include "pagenumberclass.h"
#include "arrowstring_class.h"
#include "printer.h"
#include "settings.h"
#include "locale.h"

int __stack = 1000000;

#define	PROGNAME "VPDF"
#define ARG_FILE 0
#define ARG_NEWAPP 1
#define ARG_NUMARGS 3
#define ARG_TEMPLATE "FILE/M,NEWAPP/S,APPTOFRONT/S"


extern struct Library *CairoBase;
extern char * _ProgramName;  // from libnix startup code

struct classinitialization_function
{
	ULONG (*initfunc)(void);
	void (*endfunc)(void);
	int done;
};


static struct classinitialization_function classinitialization_functions[] = {
	{create_RendererClass, delete_RendererClass},
	{create_AnnotationClass, delete_AnnotationClass},
	{create_LayClass, delete_LayClass},
	//{create_ThumbnailClass, delete_ThumbnailClass},
	{create_SearchClass, delete_SearchClass},
	{create_TitleClass, delete_TitleClass},
	{create_ToolbarClass, delete_ToolbarClass},
	{create_ContinuousLayoutClass, delete_ContinuousLayoutClass},
	{create_ContinuousDynamicLayoutClass, delete_ContinuousDynamicLayoutClass},
	{create_SinglePageLayoutClass, delete_SinglePageLayoutClass},
	{create_PageViewClass, delete_PageViewClass},
	{create_OutlineViewClass, delete_OutlineViewClass},
	{create_DocumentViewClass, delete_DocumentViewClass},
	{create_ThumbnailListClass, delete_ThumbnailListClass},
	{create_VPDFSettingsClass, delete_VPDFSettingsClass},
	{create_VPDFWindowClass, delete_VPDFWindowClass},
	{create_VPDFTitleButtonClass, delete_VPDFTitleButtonClass},
	{create_ArrowStringClass, delete_ArrowStringClass},
	{create_VPDFNumberGeneratorClass, delete_VPDFNumberGeneratorClass},
	{create_VPDFPrinterClass, delete_VPDFPrinterClass},
	{create_VPDFClass, delete_VPDFClass},
	{create_LoggerClass, delete_LoggerClass},
	{NULL}
};

static void cleanupclasses(void)
{
	int i = sizeof(classinitialization_functions) / sizeof(*classinitialization_functions) - 1;

	while (i >= 0)
	{
		if (classinitialization_functions[ i ].done)
			classinitialization_functions[ i ].endfunc();
		i--;
	}
}

static Object *_app = NULL;

void vpdfErrorFunction(int pos, char *message)
{
	if (_app != NULL && message != NULL)
		DoMethod(_app, MUIM_Application_PushMethod, _app, 3, MUIM_VPDF_LogMessage, MUIV_VPDF_LogMessage_Error, strdup(message));
}

static void MessageAddFile(struct MsgPort *port, struct MsgPort *replyport, struct RexxMsg *rexxmsg, STRPTR file)
{
	char arexxstr[1024];
	char filepath[1024];

	BPTR lock = Lock(file, ACCESS_READ);

	strcpy(arexxstr, "OPEN FILE=");

	if(lock == NULL)
	{
		lock = Lock("", ACCESS_READ);
		NameFromLock(lock, filepath, sizeof(filepath));
		AddPart(filepath, file, sizeof(filepath));
	}
	else
		NameFromLock(lock, filepath, sizeof(filepath));

	if(lock != NULL)
	{
		char *ftemp = filepath;

		UnLock(lock);

		while(*ftemp != '\0')
		{
			if(*ftemp == ' ')
				*ftemp = '\\';
			ftemp++;
		}

		strlcat(arexxstr, filepath, sizeof(arexxstr));
		rexxmsg->rm_Args[0] = (char *)CreateArgstring(arexxstr, strlen(arexxstr));
		rexxmsg->rm_Action = RXCOMM;
		PutMsg(port, (struct Message*)rexxmsg);
		WaitPort(replyport);
		GetMsg(replyport);
		DeleteArgstring(rexxmsg->rm_Args[0]);
	}
}

int main(int argc, char *argv[])
{
	struct ExecBase *MySysBase = *(unsigned int*)4;
	
#if 0
	/* make us halt on first hit! */
	{
		struct Task *this = FindTask(NULL);
		this->tc_ETask->MaxHits = 1;
	}


	MySysBase->ex_AlertMsgPort = NULL;
#endif
	InitSystem(argc, argv);
	/* initialize classes */

	locale_init();

	{
		int i = 0;

		while (classinitialization_functions[ i ].initfunc != NULL)
		{
			if (!classinitialization_functions[ i ].initfunc())
			{
				char message[ 512 ];

				snprintf(message, sizeof(message), "Failed to initialize class (%d)", i);
				MUI_Request(NULL, NULL, 0, "Error...", "OK", message, TAG_END);
				cleanupclasses();
				return NULL;
			}
			classinitialization_functions[ i ].done = TRUE;
			i++;
		}
	}

	/* handle params */
	{
		struct RDArgs *myargs = NULL;
		LONG args[ARG_NUMARGS] = {0};
		Object *app, *window;
		ULONG sigs = 0;
		char buf[80];
		struct MsgPort *port = NULL;
		struct DiskObject *disk_object = NULL;
	
		app = NULL;

		if(argc && !(myargs = ReadArgs(ARG_TEMPLATE, args, NULL)))
			Fault(IoErr(), 0, buf, sizeof(buf));
		else
		{
			{
				if(!args[ARG_NEWAPP])
				{
					Forbid();
					port = FindPort("VPDF.1");
					if(port)
					{
						struct MsgPort *replyport;
						struct RexxMsg *rexxmsg;

						Permit();
						replyport = CreateMsgPort();
						rexxmsg	= CreateRexxMsg(replyport, NULL, "VPDF.1");
						if(rexxmsg != NULL)
						{
							char arexxstr[1024];

							if(args[ARG_FILE])
							{
								char **files = (char**)args[ARG_FILE];
								int i;

								for(i = 0; files[i]; i++)
									MessageAddFile(port, replyport, rexxmsg, files[i]);
							}
							else if(argc == 0)
							{
								struct WBStartup *wbs = (struct WBStartup *)argv;
								struct WBArg *wba;
								char filename[1024];
								int i = 1;

								while(i < wbs->sm_NumArgs)
								{
									wba = wbs->sm_ArgList + i;

									if(wba->wa_Lock)
									{
										if(NameFromLock(wba->wa_Lock, filename, sizeof(filename)))
										{
											if(AddPart(filename, (char*)wba->wa_Name, sizeof(filename)))
												MessageAddFile(port, replyport, rexxmsg, filename);
										}
									}

									i++;
								}
							}

							strcpy(arexxstr, "APPTOFRONT");
							rexxmsg->rm_Args[0] = (char *)CreateArgstring(arexxstr, strlen(arexxstr));
							rexxmsg->rm_Action = RXCOMM;

							Forbid();
							port = FindPort("VPDF.1");
							if(port)
							{

								PutMsg(port, (struct Message*)rexxmsg);
								WaitPort(replyport);
								GetMsg(replyport);
							}
							Permit();
							DeleteArgstring(rexxmsg->rm_Args[0]);
							DeleteRexxMsg(rexxmsg);
						}
						if(replyport)
							DeleteMsgPort(replyport);
					}
					else
						Permit();
				}

				if(!port)
				{
					/* build new application object */
					disk_object =GetDiskObject(_ProgramName);
					
	            	app	= VPDFObject,
						MUIA_Application_Title,			PROGNAME,
						MUIA_Application_Version,		"$VER: " PROGNAME " " VERSION "." REVISION " ("VERSIONDATE ")""© 2009-2012 by Michal Wozniak",
						MUIA_Application_Copyright,		"© 2009-2012 by Michal Wozniak",
						MUIA_Application_Author,		"Michal Wozniak",
						MUIA_Application_Description, LOCSTR( MSG_APPLICATION_DESCRIPTION ),
						MUIA_Application_Base,			"VPDF",
					    MUIA_Application_DiskObject,    disk_object,
					End;

					if(app != NULL)
					{
						int opened = 0;
						DoMethod(app, MUIM_Application_Load, MUIV_Application_Load_ENV);
						_app = app;

						if(argc)
						{
							if(args[ARG_FILE])
							{
								char **files = (char**)args[ARG_FILE];
								int i;

								for(i = 0; files[i] != NULL; i++)
								{
									DoMethod(app, MUIM_VPDF_OpenFile, 0, files[i], MUIV_VPDFWindow_OpenFile_NewTab);
									opened++;
								}
							}
						}
						else
						{
							struct WBStartup *wbs = (struct WBStartup *)argv;
							struct WBArg *wba;
							char filename[1024];
							int i = 1;

							while(i < wbs->sm_NumArgs)
							{
								wba = wbs->sm_ArgList + i;

								if(wba->wa_Lock)
								{
									if(NameFromLock(wba->wa_Lock, filename, sizeof(filename)))
									{
										if(AddPart(filename, (char*)wba->wa_Name, sizeof(filename)))
										{
											DoMethod(app, MUIM_VPDF_OpenFile, 0, filename, MUIV_VPDFWindow_OpenFile_NewTab);
											opened++;
										}
									}
								}

								i++;
							}
						}

						if (opened == 0)
							DoMethod(app, MUIM_VPDF_CreateWindow);

						while(DoMethod(app, MUIM_Application_NewInput, &sigs) != (ULONG)MUIV_Application_ReturnID_Quit)
						{
							if(sigs)
							{
								sigs = Wait(sigs | SIGBREAKF_CTRL_C);
								if(sigs & SIGBREAKF_CTRL_C) break;
							}
						}

						DoMethod(app, MUIM_Application_Save, MUIV_Application_Save_ENV);
						DoMethod(app, MUIM_Application_Save, MUIV_Application_Save_ENVARC);

						MUI_DisposeObject(app);
					}
					if (disk_object)
						FreeDiskObject(disk_object);	
				}
			}

			if(myargs)
				FreeArgs(myargs);
				
				
			
		}
	}

	cleanupclasses();
	locale_cleanup();
	return 0;
}
