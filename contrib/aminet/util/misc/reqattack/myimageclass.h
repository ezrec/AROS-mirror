#ifndef MYIMAGECLASS_H
#define MYIMAGECLASS_H

#ifndef MYIMAGE_H
#include "myimage.h"
#endif

#ifndef REQLIST_H
#include "reqlist.h"
#endif

#define MYIA_Dummy	(IA_Dummy + 1000)
#define MYIA_ReqNode (MYIA_Dummy + 1)
#define MYIA_Gadget	(MYIA_Dummy + 2)

#define MYIM_GETSIZE (IM_DRAW + 100)
 
struct MyImageData
{
	struct DrawInfo *di;
	struct MyImage *im;
	struct ReqNode *reqnode;
	struct Gadget *gad;
	char *text;

	WORD textlen;	
};

struct myimpGetSize
{
	ULONG MethodID;
	WORD *width;
	WORD *height;
};

extern struct IClass *MyImageClass;

extern BOOL InitMyImageClass(void);
extern void CleanupMyImageClass(void);

#endif

