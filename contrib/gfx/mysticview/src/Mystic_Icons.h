/*********************************************************************
----------------------------------------------------------------------

	MysticView
	definitions for icon operations

----------------------------------------------------------------------
*********************************************************************/

#ifndef MYSTIC_ICONZ_H
#define	MYSTIC_ICONZ_H 1

#include "Mystic_Global.h"

#ifdef NO_NEWICONS
struct ChunkyImage
{
    UBYTE dummy;
};
struct NewDiskObject
{
    UBYTE dummy;
};
#else
#include <libraries/newicon.h>
#endif

struct IconThumbnail
{
	struct NewDiskObject *dob;
	int width, height;
	struct ChunkyImage *oldimg1, *oldimg2;
	struct ChunkyImage img1;
	char *array;
	char palette[256*3];
};

extern struct IconThumbnail *CreateIconThumbnail(PICTURE *picture, int width, int height, int maxcolors, struct NewDiskObject *diskobject);
extern void DeleteIconThumbnail(struct IconThumbnail *icon);
extern BOOL PutNewIcon(PICTURE *picture, int width, int height, int numcolors, char *filename, struct NewDiskObject *defaultdob, char *deftool);

#endif
