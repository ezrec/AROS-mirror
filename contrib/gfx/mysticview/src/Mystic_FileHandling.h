/*********************************************************************
----------------------------------------------------------------------

	MysticView
	definitions for file operations

----------------------------------------------------------------------
*********************************************************************/

#ifndef MYSTIC_FILEZ_H
#define	MYSTIC_FILEZ_H 1

#include "Mystic_Global.h"

struct piccopier
{
	APTR subtask;			/* the subtask actually copying the picture */
	char *fullsourcename;	
	char *fulldestname;	
	int confirmlevel;
	struct mvwindow *window;
	struct mview *mv;
};

#include "Mystic_InitExit.h"

enum FILEACTIONS {FILEACTION_COPY, FILEACTION_MOVE, FILEACTION_DELETE};

extern BOOL PerformFileAction(struct mview *mv, char *fullsourcename, char *fulldestname, int action, int confirmlevel);

#endif
