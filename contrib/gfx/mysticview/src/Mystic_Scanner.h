/*********************************************************************
----------------------------------------------------------------------

	MysticView
	definitions for scanner

----------------------------------------------------------------------
*********************************************************************/

#ifndef MYSTIC_SCANNER_H
#define	MYSTIC_SCANNER_H 1

#include "Mystic_Handler.h"


extern BOOL Scan(struct FileList *filelist, struct ScanListNode *snode, ULONG abortsignals, ULONG *scanindex, struct PicHandler *handler);


#endif
