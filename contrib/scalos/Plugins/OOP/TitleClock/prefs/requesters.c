/*
 *  requesters.c    Various routines for ASL requester and such like
 *
 * $Date$
 * $Revision$
 *
 *  0.1
 *  20010804    DM  + Created
 *
 */

#include <exec/types.h>

#include <proto/asl.h>
#include <proto/exec.h>
#include <proto/dos.h>

#include <intuition/intuition.h>
#include <libraries/asl.h>

#include <stdio.h>
#include <string.h>

STRPTR  SaveFileReq(struct Window *win)
{
	struct FileRequester    *aslfr;
	STRPTR                  fbuf = NULL;
	ULONG                   fblen;

	if(NULL != (aslfr = AllocAslRequest(ASL_FileRequest, NULL)) )
	{
		if(AslRequestTags(aslfr,ASLFR_Window, win,
								ASLFR_SleepWindow, TRUE,
								ASLFR_RejectIcons, TRUE,
								ASLFR_DoSaveMode, TRUE,
								TAG_DONE) )
		{
			//printf("OK\n");
			fblen = 0;
			if(aslfr->fr_Drawer)
			{
				/* add 1 for any possible / character */
				fblen += strlen(aslfr->fr_Drawer);
				if(fblen) fblen++;
				//printf("Dir='%s'\n", aslfr->fr_Drawer);
			}

			if(aslfr->fr_File)
			{
				fblen += strlen(aslfr->fr_File);
				//printf("File='%s'\n", aslfr->fr_File);
			}
			if(fblen) fblen++;
			//printf("length of string name buffer=%lu\n", fblen);

			fbuf = AllocVec(fblen, MEMF_ANY|MEMF_CLEAR);
			if(fbuf)
			{
				if(aslfr->fr_Drawer)
				{
					strcpy(fbuf, aslfr->fr_Drawer);
				}

				if(aslfr->fr_File)
				{
					AddPart(fbuf, aslfr->fr_File, fblen);
				}
				//printf("complete name='%s'\n", fbuf);
			}

			FreeAslRequest(aslfr);
		}
		else
		{
			FreeAslRequest(aslfr);
			aslfr = NULL;
			fbuf = NULL;
		}
	}
	return(fbuf);
}


STRPTR  OpenFileReq(struct Window *win)
{
	struct FileRequester    *aslfr;
	STRPTR                  fbuf = NULL;
	ULONG                   fblen;

	if(NULL != (aslfr = AllocAslRequest(ASL_FileRequest, NULL)) )
	{
		if(AslRequestTags(aslfr,ASLFR_Window, win,
								ASLFR_SleepWindow, TRUE,
								ASLFR_RejectIcons, TRUE,
								TAG_DONE) )
		{
			//printf("OK\n");
			fblen = 0;
			if(aslfr->fr_Drawer)
			{
				/* add 1 for any possible / character */
				fblen += strlen(aslfr->fr_Drawer);
				if(fblen) fblen++;
				//printf("Dir='%s'\n", aslfr->fr_Drawer);
			}

			if(aslfr->fr_File)
			{
				fblen += strlen(aslfr->fr_File);
				//printf("File='%s'\n", aslfr->fr_File);
			}
			if(fblen) fblen++;
			//printf("length of string name buffer=%lu\n", fblen);

			fbuf = AllocVec(fblen, MEMF_ANY|MEMF_CLEAR);
			if(fbuf)
			{
				if(aslfr->fr_Drawer)
				{
					strcpy(fbuf, aslfr->fr_Drawer);
				}

				if(aslfr->fr_File)
				{
					AddPart(fbuf, aslfr->fr_File, fblen);
				}
				//printf("complete name='%s'\n", fbuf);
			}

			FreeAslRequest(aslfr);
		}
		else
		{
			FreeAslRequest(aslfr);
			aslfr = NULL;
			fbuf = NULL;
		}
	}
	return(fbuf);
}

#if 0
// Small test program for the above functions
struct Library  *AslBase;

int main(void)
{
	STRPTR  fbuf;

	if(NULL != (AslBase = OpenLibrary("asl.library", 38)) )
	{
		fbuf = OpenFileReq(NULL);
		if(fbuf)
		{
			printf("File name='%s'\n", fbuf);
			FreeVec(fbuf);
		}


		fbuf = SaveFileReq(NULL);
		if(fbuf)
		{
			printf("File name='%s'\n", fbuf);
			FreeVec(fbuf);
		}

		CloseLibrary(AslBase);
	}
	return(0);
}
#endif


