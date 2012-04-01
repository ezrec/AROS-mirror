/* Wazp3D - Alain THELLIER - Paris - FRANCE 							*/
/* Adaptation to AROS from Matthias Rustler							*/
/* Code clean-up and library enhancements from Gunther Nikl					*/
/* LICENSE: GNU General Public License (GNU GPL) for this file				*/

/* This file contain the Wazp3d3D Prefs call							*/

#ifdef __amigaos4__
#define __USE_INLINE__
#define __USE_BASETYPE__
#define __USE_OLD_TIMEVAL__
#endif

#include <string.h>
#include <stdarg.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include <proto/Warp3D.h>

struct Library *Warp3DBase;
W3D_Driver **drivers;
#define W3D_Q_SETTINGS 9999

#ifdef __amigaos4__
struct Warp3DIFace*			IWarp3D		=NULL;
#endif

int main(int argc, char **argv)
{
	Warp3DBase = OpenLibrary("Warp3D.library", 4L);
	if (Warp3DBase==NULL)	exit(0);

#ifdef __amigaos4__
	IWarp3D = (struct Warp3DIFace*)GetInterface((struct Library *)Warp3DBase, "main", 1, NULL);
	if (IWarp3D==NULL)	return(FALSE);
#endif

	drivers = W3D_GetDrivers();
	if (*drivers == NULL) 	exit(0);

/* The W3D_Q_SETTINGS call CANT works with a real Warp3D but only with Wazp3D */
	if(drivers[0]->name[0]=='W')
	if(drivers[0]->name[1]=='a')
	if(drivers[0]->name[2]=='z')
	if(drivers[0]->name[3]=='p')
	if(drivers[0]->name[4]=='3')
	if(drivers[0]->name[5]=='D')
		W3D_QueryDriver(drivers[0],W3D_Q_SETTINGS,0);	

	CloseLibrary(Warp3DBase);
	exit(0);
}


