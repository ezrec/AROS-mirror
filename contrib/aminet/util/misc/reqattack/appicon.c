#include <proto/exec.h>
#include <proto/wb.h>

//					reqnode->appicon = AddAppIconA(1,(ULONG)reqnode,
//												reqnode->title,
//												reqnode->win->UserPort,
//												NULL,reqnode->appicondiskobject,
//												NULL);

struct AppIcon *AddAppIconB(ULONG id, ULONG userdata, STRPTR text, struct MsgPort *msgport, BPTR lock, struct DiskObject *diskobj, struct TagItem *taglist)
{
	/* End */
	return(AddAppIconA(id, userdata, text, msgport, lock, diskobj, taglist));
}