#ifndef MYGADGET_H
#define MYGADGET_H

#ifndef REQLIST_H
#include "reqlist.h"
#endif

#ifndef MYIMAGE_H
#include "myimage.h"
#endif

struct MyGadget
{
	struct Gadget *gad;
	struct MyImage myim;
	struct Image *im;
	char	*text;
	WORD	width;
	WORD	height;
};


#endif

