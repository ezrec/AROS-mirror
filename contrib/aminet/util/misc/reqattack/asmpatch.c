/* C Version of asmpatch.asm */

#include <intuition/intuition.h>
#include <aros/libcall.h>

#include <proto/intuition.h>

#include "global.h"
#include "asmpatch.h"
#include "patch.h"

WORD patchentrycounter;

#warning "AROS port: No stack swapping implemented!"

AROS_LH4(LONG, EasyRequestArgs,
    AROS_LHA(struct Window *, win, A0),
    AROS_LHA(struct EasyStruct *, es, A1),
    AROS_LHA(ULONG *, idcmpptr, A2),
    AROS_LHA(APTR, arglist, A3),
    struct IntuitionBase *, IntuitionBase, 98, Asm)
{
    AROS_LIBFUNC_INIT
    
    LONG retval;

    patchentrycounter++;    
    retval = New_EasyRequestArgs(win, es, idcmpptr, arglist);
    patchentrycounter--;
    
    return retval;
    
    AROS_LIBFUNC_EXIT
}

LONG New_EasyRequestArgs(struct Window *win,
			 struct EasyStruct *es,
			 ULONG *idcmpptr,
			 APTR arglist)
{
    struct Window *req;
    LONG rc;

    req = BuildEasyRequestArgs(win,
			       es,
			       idcmpptr ? *idcmpptr : 0,
				arglist);

    if (!req)
    {
	rc = 0;
    }
    else if (req == (struct Window *)1)
    {
	rc = 1;
    }
    else
    {
	while ((rc = SysReqHandler(req,idcmpptr,TRUE)) == -2)
	{
	}
	FreeSysRequest(req);
    }

    return rc;
}

LONG Old_EasyRequestArgs(struct Window *win,
			 struct EasyStruct *es,
			 ULONG *idcmpptr,
			 APTR arglist)
{
    return AROS_CALL4(LONG, oldeasyrequestargs,
    	    	AROS_LCA(struct Window *, win, A0),
		AROS_LCA(struct EasyStruct *, es, A1),
		AROS_LCA(ULONG *, idcmpptr, A2),
		AROS_LCA(APTR, arglist, A3),
		struct IntuitionBase *, IntuitionBase);
}


AROS_LH8(BOOL, AutoRequest,
    AROS_LHA(struct Window *, win, A0),
    AROS_LHA(struct IntuiText *, bodytext, A1),
    AROS_LHA(struct IntuiText *, postext, A2),
    AROS_LHA(struct IntuiText *, negtext, A3),
    AROS_LHA(ULONG, posflags, D0),
    AROS_LHA(ULONG, negflags, D1),
    AROS_LHA(WORD, width, D2),
    AROS_LHA(WORD, height, D3),
    struct IntuitionBase *, IntuitionBase, 58, Asm)
{
    AROS_LIBFUNC_INIT
    
    BOOL retval;
    
    patchentrycounter++;    
    retval = New_AutoRequest(win, bodytext, postext, negtext, posflags, negflags, width, height);
    patchentrycounter--;
    
    return retval;
    
    AROS_LIBFUNC_EXIT
}

BOOL New_AutoRequest(struct Window *win,
                     struct IntuiText *bodytext,
                     struct IntuiText *postext,
                     struct IntuiText *negtext,
                     ULONG posflags,
                     ULONG negflags,
                     WORD width,
                     WORD height)
{
    struct Window *req;
    ULONG idcmp;
    BOOL rc;

    req = BuildSysRequest(win,
                          bodytext,
                          postext,
                          negtext,
                          posflags|negflags,
                          width,
                          height);

    if (!req)
    {
        rc = FALSE;
    }
    else if (req == (struct Window *)1)
    {
        rc = TRUE;
    }
    else
    {
        while ((rc = SysReqHandler(req,&idcmp,TRUE)) == -2)
        {
        }

        if (rc == -1)
        {
            rc = (idcmp & posflags) ? TRUE : FALSE;
        }
        FreeSysRequest(req);
    }

    return rc;
}

BOOL Old_AutoRequest(struct Window *win,
		     struct IntuiText *bodytext,
		     struct IntuiText *postext,
		     struct IntuiText *negtext,
		     ULONG posflags,
		     ULONG negflags,
		     WORD width,
		     WORD height)
{
    return AROS_CALL8(BOOL, oldautorequest,
    	    	AROS_LCA(struct Window *, win, A0),
		AROS_LCA(struct IntuiText *, bodytext, A1),
		AROS_LCA(struct IntuiText *, postext, A2),
		AROS_LCA(struct IntuiText *, negtext, A3),
		AROS_LCA(ULONG, posflags, D0),
		AROS_LCA(ULONG, negflags, D1),
		AROS_LCA(WORD, width, D2),
		AROS_LCA(WORD, height, D3),
		struct IntuitionBase *, IntuitionBase);
	    
}

    
AROS_LH4(struct Window *, BuildEasyRequestArgs,
    AROS_LHA(struct Window *, win, A0),
    AROS_LHA(struct EasyStruct *, es, A1),
    AROS_LHA(ULONG, idcmp, D0),
    AROS_LHA(APTR, args, A3),
    struct IntuitionBase *, IntuitionBase, 99, Asm)
{
    AROS_LIBFUNC_INIT
    
    struct Window *retval;

    patchentrycounter++;    
    retval = New_BuildEasyRequestArgs(win, es, idcmp, args);
    if (!retval)
    {
    	retval = Old_BuildEasyRequestArgs(win, es, idcmp, args);
	patchentrycounter--;
    }
        
    return retval;
    
    AROS_LIBFUNC_EXIT
}

struct Window *Old_BuildEasyRequestArgs(struct Window *win,
					struct EasyStruct *es,
					ULONG idcmp,
					APTR args)
{
    return AROS_CALL4(struct Window *, oldbuildeasyrequestargs,
    	    	AROS_LCA(struct Window *, win, A0),
		AROS_LCA(struct EasyStruct *, es, A1),
		AROS_LCA(ULONG, idcmp, D0),
		AROS_LCA(APTR, args, A3),
		struct IntuitionBase *, IntuitionBase);
}


AROS_LH7(struct Window *, BuildSysRequest,
    AROS_LHA(struct Window *, win, A0),
    AROS_LHA(struct IntuiText *, bodytext, A1),
    AROS_LHA(struct IntuiText *, postext, A2),
    AROS_LHA(struct IntuiText *, negtext, A3),
    AROS_LHA(ULONG, idcmpflags, D0),
    AROS_LHA(WORD, width, D1),
    AROS_LHA(WORD, height, D2),
    struct IntuitionBase *, IntuitionBase, 60 , Asm)
{
    AROS_LIBFUNC_INIT
    
    struct Window *retval;
    
    patchentrycounter++;    
    retval = New_BuildSysRequest(win, bodytext, postext, negtext, idcmpflags, width, height);
    if (!retval)
    {
    	retval = Old_BuildSysRequest(win, bodytext, postext, negtext, idcmpflags, width, height);
    	patchentrycounter--;
    }
        
    return retval;
    
    AROS_LIBFUNC_EXIT
}

struct Window *Old_BuildSysRequest(struct Window *win,
				   struct IntuiText *bodytext,
				   struct IntuiText *postext,
				   struct IntuiText *negtext,
				   ULONG idcmpflags,
				   WORD width,
				   WORD height)
{
    return AROS_CALL7(struct Window *, oldbuildsysrequest,
    	    	AROS_LCA(struct Window *, win, A0),
		AROS_LCA(struct IntuiText *, bodytext, A1),
		AROS_LCA(struct IntuiText *, postext, A2),
		AROS_LCA(struct IntuiText *, negtext, A3),
		AROS_LCA(ULONG, idcmpflags, D0),
		AROS_LCA(WORD, width, D1),
		AROS_LCA(WORD, height, D2),
		struct IntuitionBase *, IntuitionBase);
		
}

AROS_LH3(LONG, SysReqHandler,
    AROS_LHA(struct Window *, win, A0),
    AROS_LHA(ULONG *, idcmpptr, A1),
    AROS_LHA(BOOL, waitinput, D0),
    struct IntuitionBase *, IntuitionBase, 100, Asm)
{
    AROS_LIBFUNC_INIT

    LONG retval;
    
    patchentrycounter++;
    retval = New_SysReqHandler(win, idcmpptr, waitinput);
    patchentrycounter--;
    
    return retval;
    
    AROS_LIBFUNC_EXIT   
}

LONG Old_SysReqHandler(struct Window *win,
		       ULONG *idcmpptr,
		       BOOL waitinput)
{
    return AROS_CALL3(LONG, oldsysreqhandler,
    	    	AROS_LCA(struct Window *, win, A0),
		AROS_LCA(ULONG *, idcmpptr, A1),
		AROS_LCA(BOOL, waitinput, D0),
		struct IntuitionBase *, IntuitionBase);
}


AROS_LH1(void, FreeSysRequest,
    AROS_LHA(struct Window *, win, A0),
    struct IntuitionBase *, IntuitionBase, 62, Asm)
{
    AROS_LIBFUNC_INIT

    patchentrycounter -= New_FreeSysRequest(win);

    AROS_LIBFUNC_EXIT
}

void Old_FreeSysRequest(struct Window *win)
{
    return AROS_CALL1(void, oldfreesysrequest,
    	    	AROS_LCA(struct Window *, win, A0),
		struct IntuitionBase *, IntuitionBase);
}

AROS_LH5(ULONG, ReqTools,
    AROS_LHA(char *, bodyfmt, A1),
    AROS_LHA(char *, gadfmt, A2),
    AROS_LHA(ULONG *, reqinfo, A3),
    AROS_LHA(ULONG *, argarray, A4),
    AROS_LHA(struct TagItem *, taglist, A0),
    struct Library *, ReqToolsBase, 11, Asm)
{
    AROS_LIBFUNC_INIT
    
    ULONG retval;
    
    patchentrycounter++;
    
    retval = New_ReqTools(taglist, bodyfmt, gadfmt, reqinfo, argarray);
    if ((LONG)retval == -3)
    {
    	retval = AROS_CALL5(ULONG, oldreqtools,
	    	    AROS_LCA(char *, bodyfmt, A1),
		    AROS_LCA(char *, gadfmt, A2),
		    AROS_LCA(ULONG *, reqinfo, A3),
		    AROS_LCA(ULONG *, argarray, A4),
		    AROS_LCA(struct TagItem *, taglist, A0),
		    struct Library *, ReqToolsBase);
    }
    
    patchentrycounter--;
    
    return retval;
    
    AROS_LIBFUNC_EXIT
}
