#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>

#include <exec/memory.h>
#include <exec/semaphores.h>
#include <exec/libraries.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <dos/exall.h>
#include <intuition/intuition.h>

#include <clib/alib_protos.h>
#include <string.h>
#include <stdio.h>

#include "global.h"
#include "application.h"
#include "asmraapplication.h"
#include "reqlist.h"
#include "misc.h"
#include "myrequester.h"
#include "reqattack.h"
#include "cx.h"
#include "asmmisc.h"
#include "asmpatch.h"

#include "reqlist_protos.h"

ULONG functiontable[7];
struct Library *myLibrary;
struct SignalSemaphore mylibSemaphore;

void InitApplication(void)
{

	functiontable[0] = (ULONG)Asm_RALibOpen;
	functiontable[1] = (ULONG)Asm_RALibClose;
	functiontable[2] = (ULONG)Asm_RALibExpunge;
	functiontable[3] = NULL;
	functiontable[4] = (ULONG)ExtendedRequestArgs;
	functiontable[5] = (ULONG)ExtendedBuildRequestArgs;;
	functiontable[6] = -1;
	
	if (myLibrary = MakeLibrary(functiontable,NULL,NULL,sizeof(struct Library),NULL));
	{
		myLibrary->lib_Version = 2;
		myLibrary->lib_Revision = 0;
		myLibrary->lib_IdString = "subreqattack.library 0.02 (17.10.01)\n";
		myLibrary->lib_Node.ln_Name = "subreqattack.library";
		myLibrary->lib_Node.ln_Type = NT_LIBRARY;
		//AddLibrary(myLibrary);
		progport.sublibrary = myLibrary;
		mylibSemaphore.ss_Link.ln_Name = "ReqAttack SubLibrary";
		mylibSemaphore.ss_Link.ln_Pri = 0;
		AddSemaphore(&mylibSemaphore);
	}
}

BOOL CleanupApplication(void)
{

	if (myLibrary)
	{

		if (!(AttemptSemaphore(&mylibSemaphore))) return FALSE;	
		if (myLibrary->lib_OpenCnt != 0) {Permit(); return FALSE;};
		
		//Remove((struct Node *)myLibrary);
		//now free the resources...
		progport.sublibrary = NULL;
		FreeMem((APTR) ((ULONG)myLibrary - myLibrary->lib_NegSize), myLibrary->lib_PosSize + myLibrary->lib_NegSize);
		RemSemaphore(&mylibSemaphore);
		ReleaseSemaphore(&mylibSemaphore);
	}
	
	return TRUE;
}

// here's two library functions ;)
LONG __saveds ExtendedRequestArgs(REG(a0,struct Window *,Window),REG(a1,struct EasyStruct *,easyStruct),REG(a2,ULONG *,IDCMP_ptr),REG(d1,struct DeveloperAttack *,devAttack),REG(a3,APTR, ArgList))
{
	struct Window *req;
	LONG rc;
	
	req = ExtendedBuildRequestArgs(Window,
											 easyStruct,
											 IDCMP_ptr ? *IDCMP_ptr :0,
											 devAttack,
											 ArgList);
	if (!req)
	{
		rc = 0;
	} else if (req == (struct Window *)1) {
		rc = 1;
	} else {
		while ((rc = SysReqHandler(req,IDCMP_ptr,TRUE)) == -2)
		{
		}
		FreeSysRequest(req);
	}

	return rc;
}

struct Window * __saveds ExtendedBuildRequestArgs(REG(a0,struct Window *,RefWindow),REG(a1,struct EasyStruct *,easyStruct),REG(d0,ULONG ,IDCMP),REG(d1,struct DeveloperAttack *,devAttack),REG(a3,APTR, Args))
{

	struct ReqNode *reqnode = 0;
	struct IntuiText *it;
	struct MyGadget *mygad;
	char c,*sp;
	APTR nextarg;
	LONG l,y;

	struct Window *rc = 0;
	
	if (!cxinactive) if ((reqnode = NewReqNode()))
	{
		if (RefWindow)
		{
			reqnode->scr = RefWindow->WScreen;
		} else {
			reqnode->scr = LockPubScreen(0);
			if (!reqnode->scr) goto fail;
		}

		reqnode->vp  = &reqnode->scr->ViewPort;
		reqnode->cm = reqnode->vp->ColorMap;
		reqnode->dri = GetScreenDrawInfo(reqnode->scr);
		reqnode->font = reqnode->dri->dri_Font;		

		/* BODY */

		l = 0;
		RawDoFmt(easyStruct->es_TextFormat,
					Args,
				   (APTR)GetLength_PutChFunc,
				   &l);

		D(bug("*** TextFormat length %ld\n",l));

		if (!(reqnode->bodytext = MiscAllocVec(l + 1))) goto fail;

    	#ifdef NO_ASM
	    {
	    	STRPTR *strptr = reqnode->bodytext;
		nextarg = RawDoFmt(easyStruct->es_TextFormat,
					 			 Args,
					 			 (APTR)Copy_PutChFunc,
					 			 &strptr);
	    }
	#else
		nextarg = RawDoFmt(easyStruct->es_TextFormat,
					 			 Args,
					 			 (APTR)Copy_PutChFunc,
					 			 reqnode->bodytext);
    	#endif
		/* count lines and replace '\n' with '\0' */

		l = 1;
		sp = reqnode->bodytext;
		while((c = *sp++))
		{
			if (c == '\n')
			{
				l++;
			}
		}
		sp[-1] = '\n';

		/* create body intuitexts */

		if (!(reqnode->body = MiscAllocVec(l * sizeof(struct IntuiText)))) goto fail;

		reqnode->flags |= REQF_FREEBODY;

		it = reqnode->body;
		sp = reqnode->bodytext;

		y = reqnode->font->tf_YSize -
			 reqnode->font->tf_Baseline;

		if (y < 1) y = 1;
		y += reqnode->font->tf_YSize - 1;

		while(l--)
		{
			if (l > 0)
			{
				it->NextText = it + 1;
				it->NextText->TopEdge = it->TopEdge + y;
			}
			it->IText = sp;
			it->FrontPen = reqnode->dri->dri_Pens[TEXTPEN];
			it->DrawMode = JAM1;
			
			while(*sp++ != '\n');

			it++;
		}
		
		/* BUTTONS */
		
		l = 0;
		RawDoFmt(easyStruct->es_GadgetFormat,
					nextarg,
				   (APTR)GetLength_PutChFunc,
				   &l);

		if (!(reqnode->buttontext = MiscAllocVec(l + 1))) goto fail;



    	#ifdef NO_ASM
	    {
	    	STRPTR *strptr = reqnode->buttontext;

		RawDoFmt(easyStruct->es_GadgetFormat,
					nextarg,
					(APTR)Copy_PutChFunc,
					&strptr);
	    }
	#else

		RawDoFmt(easyStruct->es_GadgetFormat,
					nextarg,
					(APTR)Copy_PutChFunc,
					reqnode->buttontext);
    	#endif
		/* count gadgets and replace '|' with '\0' */

		l = 1;
		sp = reqnode->buttontext;
		while((c = *sp++))
		{
			if (c == '|')
			{
				l++;
				sp[-1] = '\0';
			}
		}

		/* create mygadget structures */
		
		if (!(reqnode->buttongadgets = MiscAllocVec(l * sizeof(struct MyGadget)))) goto fail;

		reqnode->num_buttongadgets = l;

		mygad = reqnode->buttongadgets;
		sp = reqnode->buttontext;
		while(l--)
		{
			mygad->text = sp;								          
			while (*sp++);
			mygad++;
		}

		reqnode->title = easyStruct->es_Title ? (char *)easyStruct->es_Title :
												  RefWindow ? (char *)RefWindow->Title : 0;

// and now do what this function is really suposed to do - insert devAttack to ReqNode;)

		reqnode->devattack = devAttack;

		rc = MakeMyRequester(reqnode,RefWindow,IDCMP);

	} /* if ((reqnode = NewReqNode())) */
	
fail:
	if (reqnode)
	{
		if (!RefWindow && reqnode->scr) UnlockPubScreen(0,reqnode->scr);
	}

	if (rc)
	{
		AddReqNode(reqnode);
		
		patchentrycounter++;//we need this!!!
		
	} else {
		if (reqnode) FreeReqNode(reqnode);
	}
	
	return rc;
}
