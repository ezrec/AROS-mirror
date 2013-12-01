/*
 * $Id$
 *
 * :ts=8
 */

//#include "/workbench.h"
#include <workbench/startup.h>

#include <dos/dos.h>

#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/wb_protos.h>

#include <pragmas/exec_sysbase_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/wb_pragmas.h>

/****************************************************************************/

extern struct Library *SysBase;
extern struct Library *DOSBase;

/****************************************************************************/

struct Library * WorkbenchBase;

/****************************************************************************/

int
main(int argc,char ** argv)
{
	struct MsgPort * mp = NULL;
	struct AppMenu * menu = NULL;
	struct AppMenuItem * item1 = NULL;
	struct AppMenuItem * item2 = NULL;
	struct AppMenuItem * item3 = NULL;
	struct AppMenuItem * sub = NULL;
	struct AppMessage * am;
	ULONG key;
	ULONG key2;
	BOOL done;

	WorkbenchBase = OpenLibrary("workbench.library",45);
	if(WorkbenchBase == NULL)
	{
		Printf("Couldn't open workbench.library V45.\n");
		goto out;
	}

	mp = CreateMsgPort();
	if(mp == NULL)
	{
		Printf("Couldn't create MsgPort.\n");
		goto out;
	}

	key = 0;

	// AddAppMenuItemA()
	menu = (struct AppMenu *)AddAppMenuItem(0,
		(ULONG)"new top menu",
		"New menu title",
		mp,
		WBAPPMENUA_GetTitleKey,&key,
		TAG_DONE);
	if(menu == NULL)
	{
		Printf("Couldn't create menu.\n");
		goto out;
	}

	Printf("Key=%08lx\n", key);

	if(key == 0)
	{
		Printf("Can't get a key for the menu\n");
		goto out;
	}

	item1 = AddAppMenuItem(1,
		(ULONG)"menu item 1",
		"This is the first menu item",
		mp,
		WBAPPMENUA_UseKey,key,
		TAG_DONE);
	if(item1 == NULL)
	{
		Printf("Couldn't create first menu item.\n");
		goto out;
	}

	item2 = AddAppMenuItem(2,
		(ULONG)"menu item 2",
		"This is the second menu item",
		mp,
		WBAPPMENUA_UseKey,key,
		TAG_DONE);
	if(item2 == NULL)
	{
		Printf("Couldn't create second menu item.\n");
		goto out;
	}

	key2 = 0;
	item3 = AddAppMenuItem(3,
		(ULONG)"menu item 3",
		"This is the third menu item, and it has a subitem",
		mp,
		WBAPPMENUA_UseKey,key,
		WBAPPMENUA_GetKey,&key2,
		TAG_DONE);
	if(item2 == NULL)
	{
		Printf("Couldn't create third level menu item.\n");
		goto out;
	}

	Printf("Key2=%08lx\n", key2);

	if(key2 == 0)
	{
		Printf("Can't get a key for the submenu\n");
		goto out;
	}

	sub = AddAppMenuItem(4,
		(ULONG)"sub menu item",
		"This is the sub menu item",
		mp,
		WBAPPMENUA_UseKey,key2,
		TAG_DONE);
	if(sub == NULL)
	{
		Printf("Couldn't create sub menu item.\n");
		goto out;
	}

	done = FALSE;
	do
	{
		WaitPort(mp);

		while((am = (struct AppMessage *)GetMsg(mp)) != NULL)
		{
			if(am->am_UserData != (ULONG)NULL)
				Printf("%s\n",am->am_UserData);

			if(am->am_ID == 1)
				done = TRUE;
		}
	}
	while(NOT done);

	Printf("removing the two; press ^C to continue... ");
	Flush(Output());
	Wait(SIGBREAKF_CTRL_C);

	RemoveAppMenuItem(item1);
	item1 = NULL;

	RemoveAppMenuItem(item2);
	item2 = NULL;

	Printf("removing the submenu; press ^C to continue... ");
	Flush(Output());
	Wait(SIGBREAKF_CTRL_C);

	RemoveAppMenuItem(item3);
	item3 = NULL;

	RemoveAppMenuItem(sub);
	sub = NULL;

 out:

	if(item1 != NULL)
		RemoveAppMenuItem(item1);

	if(item2 != NULL)
		RemoveAppMenuItem(item2);

	if(item3 != NULL)
		RemoveAppMenuItem(item3);

	if(sub != NULL)
		RemoveAppMenuItem(sub);

	if(menu != NULL)
		RemoveAppMenuItem((struct AppMenuItem *)menu);

	if(mp != NULL)
	{
		while((am = (struct AppMessage *)GetMsg(mp)) != NULL)
			ReplyMsg((struct Message *)am);

		DeleteMsgPort(mp);
	}

	if(WorkbenchBase != NULL)
	{
		CloseLibrary(WorkbenchBase);
		WorkbenchBase = NULL;
	}

	return(0);
}
