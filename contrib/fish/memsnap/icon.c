/*
 *  Routines dealing with processing of Onekey's icon
 *  (be they for tooltypes or AppIcon purposes).
 *  
 *  MWS, Tuesday 13-Oct-92
 */
#include <exec/types.h>
#include <dos/dos.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/wb.h>
#include <proto/icon.h>
#include <string.h>

#include "icon.h"
static struct DiskObject *onekeyobj;

BOOL
GetOurIcon(struct WBStartup *WBenchMsg)
{
	if (WBenchMsg)
		onekeyobj = GetDiskObject(WBenchMsg->sm_ArgList->wa_Name);
	return onekeyobj ? TRUE : FALSE;
}

/* safe to call when open failed, and multiple times */
void
FreeOurIcon()
{
	if (onekeyobj) FreeDiskObject(onekeyobj);
	onekeyobj = NULL;
}

/* like ArgString() */
char *
TTString(char *name, char *def)
{
	char *what;
	if (onekeyobj)
		if (what = FindToolType(onekeyobj->do_ToolTypes, name))
			return what;
	return def;
}

/* like ArgInt() */
LONG
TTInt(char *name, LONG def)
{
	char *what;
	if (onekeyobj)
		if (what = FindToolType(onekeyobj->do_ToolTypes, name))
			StrToLong(what, &def);
	return def;
}

/* simple extension to ArgXXX routines */
BOOL
TTBool(char *name, BOOL def)
{
	char	*s;

	s = TTString(name, def ? "YES" : "NO");

	return	((strcmp(s, "YES") == 0) ||
		(strcmp(s, "TRUE") == 0)) ? TRUE : FALSE;
}
