#ifndef PATCH_H
#define PATCH_H

#ifdef __AROS__
#include <aros/cpu.h>
#define LVONUM(x) (x * (LONG)LIB_VECTSIZE / 6)
#else
#define LVONUM(x) x
#endif

#define LVO_FreeSysRequest  	    LVONUM(-372)
#define LVO_EasyRequestArgs 	    LVONUM(-588)
#define LVO_AutoRequest     	    LVONUM(-348)
#define LVO_BuildEasyRequestArgs    LVONUM(-594)
#define LVO_BuildSysRequest 	    LVONUM(-360)
#define LVO_SysReqHandler   	    LVONUM(-600)

#define LVO_AsyncRequest    	    LVONUM(-0x6e4) //dopus5.library
#define LVO_rtEZRequestA    	    LVONUM(-66)    //reqtools.library
#define NUM_PATCHES 6

struct PatchEntry
{
	BOOL dopatch;
	WORD lvo;
	APTR *oldfunc;
	APTR newfunc;
	BOOL patchinstalled;
};

struct EasyIdcmpWin
{
	struct EasyStruct easystruct;
	APTR *IDCMPFlags;
	struct Window  *window;
	APTR *arglist;
	ULONG closew;
};

extern APTR oldeasyrequestargs;
extern APTR oldautorequest;
extern APTR oldbuildeasyrequestargs;
extern APTR oldbuildsysrequest;
extern APTR oldfreesysrequest;
extern APTR oldsysreqhandler;
extern APTR oldreqtools;
//extern APTR olddopus;

void InstallPatches(void);
BOOL RemovePatches(void);

LONG New_EasyRequestArgs(struct Window *win,
								 struct EasyStruct *es,
								 ULONG *idcmpptr,
								 APTR arglist);

BOOL New_AutoRequest(struct Window *win,
							struct IntuiText *bodytext,
							struct IntuiText *postext,
							struct IntuiText *negtext,
							ULONG posflags,
							ULONG negflags,
							WORD width,
							WORD height);

struct Window *New_BuildEasyRequestArgs(struct Window *win,
													 struct EasyStruct *es,
													 ULONG idcmp,
													 APTR args);

struct Window *New_BuildSysRequest(struct Window *win,
											  struct IntuiText *bodytext,
											  struct IntuiText *postext,
											  struct IntuiText *negtext,
											  ULONG idcmpflags,
											  WORD width,
											  WORD height);
												 
//ULONG New_DOpus5Request(REG(d1,struct TagItem *,tags),
//					REG(a1,struct Window *,window),REG(a2,ULONG *,callback),
//					REG(a3,APTR,data));

LONG New_FreeSysRequest(struct Window *win);

LONG New_SysReqHandler(struct Window *win,
							  ULONG *idcmpptr,
							  BOOL waitinput);
								  

ULONG New_ReqTools(REG(a0,struct TagItem *,taglist),
						REG(a1,char *,bodyfmt),REG(a2,char *,gadfmt),
						REG(a3,ULONG *,reqinfo),REG(a4,ULONG *,argarray));

#endif

