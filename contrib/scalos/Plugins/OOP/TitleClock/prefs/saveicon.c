/*
 *  saveicon.h  Public protos for functions to deal with saving icons
 *
 * $Date$
 * $Revision$
 *
 *  0.1
 *  20010804    DM  + Created. Added one function to save an icon (currently only
 *                  saves a default projet icon) for a file, if the file exists.
 *
 */

#include <exec/types.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>

#include <workbench/workbench.h>


BOOL SaveIcon(STRPTR filename)
{
	BOOL                saved = FALSE;
	BPTR                flock;
	struct DiskObject   *ico;   /* Icon to save */

	if(filename)
	{
		/* Make sure file exists */
		flock = Lock(filename, ACCESS_READ);
		UnLock(flock);

		if(flock)
		{
			if(NULL != (IconBase = OpenLibrary("icon.library", 36)) )
			{
				ico = GetDefDiskObject(WBPROJECT);
				if(ico)
				{
					saved = PutDiskObject(filename, ico);
					FreeDiskObject(ico);
				}
				CloseLibrary(IconBase);
			}
		}
	}
	return(saved);
}


