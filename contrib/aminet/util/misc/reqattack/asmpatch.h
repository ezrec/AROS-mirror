/*#ifndef ASMPATCH_H*/
#define ASMPATCH_H

extern WORD patchentrycounter;

#ifdef __AROS__

#include <aros/libcall.h>

extern
AROS_LD4(LONG, EasyRequestArgs,
    AROS_LDA(struct Window *, win, A0),
    AROS_LDA(struct EasyStruct *, es, A1),
    AROS_LDA(ULONG *, idcmpptr, A2),
    AROS_LDA(APTR, arglist, A3),
    struct IntuitionBase *, IntuitionBase, 98, Asm);

extern
AROS_LD8(BOOL, AutoRequest,
    AROS_LDA(struct Window *, win, A0),
    AROS_LDA(struct IntuiText *, bodytext, A1),
    AROS_LDA(struct IntuiText *, postext, A2),
    AROS_LDA(struct IntuiText *, negtext, A3),
    AROS_LDA(ULONG, posflags, D0),
    AROS_LDA(ULONG, negflags, D1),
    AROS_LDA(WORD, width, D2),
    AROS_LDA(WORD, height, D3),
    struct IntuitionBase *, IntuitionBase, 58, Asm);

extern
AROS_LD4(struct Window *, BuildEasyRequestArgs,
    AROS_LDA(struct Window *, win, A0),
    AROS_LDA(struct EasyStruct *, es, A1),
    AROS_LDA(ULONG, idcmp, D0),
    AROS_LDA(APTR, args, A3),
    struct IntuitionBase *, IntuitionBase, 99, Asm);

extern
AROS_LD7(struct Window *, BuildSysRequest,
    AROS_LDA(struct Window *, win, A0),
    AROS_LDA(struct IntuiText *, bodytext, A1),
    AROS_LDA(struct IntuiText *, postext, A2),
    AROS_LDA(struct IntuiText *, negtext, A3),
    AROS_LDA(ULONG, idcmpflags, D0),
    AROS_LDA(WORD, width, D1),
    AROS_LDA(WORD, height, D2),
    struct IntuitionBase *, IntuitionBase, 60 , Asm);

extern
AROS_LD3(LONG, SysReqHandler,
    AROS_LDA(struct Window *, win, A0),
    AROS_LDA(ULONG *, idcmpptr, A1),
    AROS_LDA(BOOL, waitinput, D0),
    struct IntuitionBase *, IntuitionBase, 100, Asm);

extern
AROS_LD1(void, FreeSysRequest,
    AROS_LDA(struct Window *, win, A0),
    struct IntuitionBase *, IntuitionBase, 62, Asm);

extern
AROS_LD5(ULONG, ReqTools,
    AROS_LDA(char *, bodyfmt, A1),
    AROS_LDA(char *, gadfmt, A2),
    AROS_LDA(ULONG *, reqinfo, A3),
    AROS_LDA(ULONG *, argarray, A4),
    AROS_LDA(struct TagItem *, taglist, A0),
    struct Library *, ReqToolsBase, 11, Asm);

#else
LONG Asm_EasyRequestArgs(REG(a0,struct Window,*win),
								 REG(a1,struct EasyStruct,*es),
								 REG(a2,ULONG *,idcmpptr),
								 REG(a3,APTR,arglist));

BOOL Asm_AutoRequest(REG(a0,struct Window *,win),
							REG(a1,struct IntuiText *,bodytext),
							REG(a2,struct IntuiText *,postext),
							REG(a3,struct IntuiText *,negtext),
							REG(d0,ULONG,posflags),
							REG(d1,ULONG,negflags),
							REG(d2,WORD,width),
							REG(d3,WORD,height));

struct Window *Asm_BuildEasyRequestArgs(REG(a0,struct Window *,_win),
													 REG(a1,struct EasyStruct *,_es),
													 REG(d0,ULONG,_idcmp),
													 REG(a3,APTR,_args));


struct Window *Asm_BuildSysRequest(REG(a0,struct Window *,win),
											  REG(a1,struct IntuiText *,bodytext),
											  REG(a2,struct IntuiText *,postext),
											  REG(a3,struct IntuiText *,negtext),
											  REG(d0,ULONG,idcmpflags),
											  REG(d1,WORD,width),
											  REG(d2,WORD,height));
												 
void Asm_FreeSysRequest(REG(a0,struct Window *,win));

LONG Asm_SysReqHandler(REG(a0,struct Window *,win),
							  REG(a1,ULONG *,idcmpptr),
							  REG(d0,BOOL,waitinput));
								  
ULONG Asm_ReqTools(REG(a0,struct TagItem *,taglist),
						REG(a1,char *,bodyfmt),
						REG(a2,char *,gadfmt),
						REG(a3,ULONG *,reqinfo),
						REG(a4,ULONG *,argarray));

//ULONG Asm_DOpus(REG(d1,struct TagItem *,tags),
//					REG(a1,struct Window *,window),REG(a2,ULONG *,callback),
//					REG(a3,APTR,data));

#endif

/***********************************************************/

LONG Old_EasyRequestArgs(struct Window *win,
								 struct EasyStruct *es,
								 ULONG *idcmpptr,
								 APTR arglist);

BOOL Old_AutoRequest(struct Window *win,
							struct IntuiText *bodytext,
							struct IntuiText *postext,
							struct IntuiText *negtext,
							ULONG posflags,
							ULONG negflags,
							WORD width,
							WORD height);

struct Window *Old_BuildEasyRequestArgs(struct Window *win,
													 struct EasyStruct *es,
													 ULONG idcmp,
													 APTR args);

struct Window *Old_BuildSysRequest(struct Window *win,
											  struct IntuiText *bodytext,
											  struct IntuiText *postext,
											  struct IntuiText *negtext,
											  ULONG idcmpflags,
											  WORD width,
											  WORD height);
												 
void Old_FreeSysRequest(struct Window *win);

LONG Old_SysReqHandler(struct Window *win,
							  ULONG *idcmpptr,
							  BOOL waitinput);

ULONG Old_ReqTools(REG(a0,struct TagItem *,taglist),
						REG(a1,char *,bodyfmt),
						REG(a2,char *,gadfmt),
						REG(a3,ULONG *,reqinfo),
						REG(a4,ULONG *,argarray));

//LONG Old_DOpus(REG(d1,struct TagItem *,tags),
//					REG(a1,struct Window *,window),REG(a2,ULONG *,callback),
//					REG(a3,APTR,data));

/*#endif*/

