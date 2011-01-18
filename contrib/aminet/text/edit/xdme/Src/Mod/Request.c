/******************************************************************************

    MODULE
	Request.c

    DESCRIPTION
	Command frontend for the different ReqTools Requesters

    NOTES
	Needs ReqTools.library >v38

    BUGS
	none known

    TODO
	split for filereq

    EXAMPLES

    SEE ALSO

    INDEX

    HISTORY
	11-09-94 b_noll created
	14-09-94 b_noll command documentation
	21-09-94 b_noll added DEFCMD/DEFHELP-support

******************************************************************************/


/**************************************
	      Includes
**************************************/

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <libraries/dos.h>
#include <intuition/intuition.h>
#include <proto/exec.h>
#include <proto/dos.h>

#include <string.h>

#include <libraries/reqtools.h>
#include <proto/reqtools.h>

#include "defs.h"

/**************************************
	    Global Variables
**************************************/

Prototype char ReqString[MAXLINELEN];

/**************************************
      Internal Defines & Structures
**************************************/


/**************************************
	   Internal Variables
**************************************/

char ReqString[MAXLINELEN];
long ReqResult;

/**************************************
	   Internal Prototypes
**************************************/
#ifndef __AROS__
#pragma tagcall ReqToolsBase rtFileRequest	36 8BA904
#pragma tagcall ReqToolsBase rtEZRequestTags	42 8CBA905
#pragma tagcall ReqToolsBase rtGetString	48 8BA0905
#pragma tagcall ReqToolsBase rtGetLong		4E 8BA904
#pragma tagcall ReqToolsBase rtPaletteRequest	66 8BA03
#endif

/**************************************
		 Macros
**************************************/


/**************************************
	     Implementation
**************************************/


/***************************************************
		XDME COMMAND INTERFACE 11 Sep 1994
***************************************************/

/*
*!
*! >REQSTRING Title/A,Text/A,Gadgets/A,InitValue/A
*!
*!  Interface to the ReqTools StringRequester
*!  Opens a synchroneous ReqTools Requester
*!  with the given values
*!
*!  The Result is written into ReqResult
*!  Cancelling the Requester sets globalflags.Abortcommand;
*!
*!  gadgets = "|"-separated List of Strings
*!
*/
/*DEFHELP #cmd requester REQSTRING title format gadgets defvalue - display a synchron ReqTools StringRequest; the result is put in $REQRESULT. */

DEFUSERCMD("reqstring", 4, CF_COK|CF_VWM|CF_ICO, int, do_reqstring, (void),)
{
    char *title   = av[1];
    char *format  = av[2];
    char *gadgets = av[3];
    char *value   = av[4];

    if (!ReqToolsBase) {
	error ("%s:\n`reqtools.library' is needed for that Command!\n", av[0]);
	return RET_FAIL;
    } /* if */

    strcpy (ReqString, value);
    ReqResult = rtGetString (ReqString, sizeof(ReqString), title, NULL,
	RTGS_TextFmt, format,
	RTGS_GadFmt, gadgets,
	TAG_END);
    if (ReqResult == FALSE)
	SETF_ABORTCOMMAND(Ep, 1 );
    return RET_SUCC;
} /* do_reqstring */

/*
*!
*! >REQNUMBER Title/A,Text/A,Gadgets/A,InitValue/N/A,Minimum/N/A,Maximum/N/A
*!
*!  Interface to the ReqTools NumberRequester
*!  Opens a synchroneous ReqTools Requester
*!  with the given values
*!
*!  The Result is written into ReqResult
*!  Cancelling the Requester sets globalflags.Abortcommand;
*!
*!  gadgets = "|"-separated List of Strings
*!
*/
/*DEFHELP #cmd requester REQNUMBER title format gadgets defvalue min max - display a synchron ReqTools NumberRequest; the result is put in $REQRESULT. */

DEFUSERCMD("reqnumber",     6, CF_COK|CF_VWM|CF_ICO, int, do_reqnumber, (void),)
{
    char *title   = av[1];
    char *format  = av[2];
    char *gadgets = av[3];
    char *value   = av[4];
    char *minimum = av[5];
    char *maximum = av[6];

    long longnum = atol(value);

    if (!ReqToolsBase) {
	error ("%s:\n`reqtools.library' is needed for that Command!\n", av[0]);
	return RET_FAIL;
    } /* if */


    ReqResult = rtGetLong ((ULONG *)&longnum, title, NULL,
	RTGL_Min,     atol(minimum),
	RTGL_Max,     atol(maximum),
	RTGL_GadFmt,  gadgets,
	RTGL_TextFmt, format,

	RT_Window, Ep->win, TAG_END);
    if (ReqResult == FALSE)
	SETF_ABORTCOMMAND(Ep, 1 );
    else
	strcpy (ReqString, ltostr(longnum));
    return RET_SUCC;
} /* do_reqnumber */

/*
*!
*! >REQPALETTE Title/A,InitValue/N/A
*!
*!  Interface to the ReqTools PaletteRequester
*!  Opens a synchroneous ReqTools Requester
*!  with the given values
*!
*!  The Result is written into ReqResult
*!  Cancelling the Requester sets globalflags.Abortcommand;
*!
*/
/*DEFHELP #cmd requester REQPALETTE title defvalue - display a synch ReqTools PaletteRequest; the result is put in $REQRESULT. */

DEFUSERCMD("reqpalette", 2, CF_COK|CF_VWM|CF_ICO, int, do_reqpalette, (void),)
{
    char *title = av[1];
    char *value = av[2];

    if (!ReqToolsBase) {
	error ("%s:\n`reqtools.library' is needed for that Command!\n", av[0]);
	return RET_FAIL;
    } /* if */

    ReqResult = rtPaletteRequest(title, NULL,
	RTPA_Color, atol(value),

	RT_Window, Ep->win, TAG_END);
    if (ReqResult == -1)
	SETF_ABORTCOMMAND(Ep, 1 );
    else
	strcpy (ReqString, ltostr(ReqResult));
    return RET_SUCC;
} /* do_reqpalette */


/*
*!
*! >REQUEST Title/A,Text/A,Gadgets/A
*!
*!  Interface to the ReqTools EZ Requester
*!  Opens a synchroneous ReqTools Requester
*!  with the given values
*!
*!  The Result is written into ReqResult
*!
*!  gadgets = "|"-separated List of Strings
*!
*/
/*DEFHELP #cmd requester REQUEST title body gadgets - display a synch ReqTools EZRequest; the result is put in $REQRESULT. */

DEFUSERCMD("request", 3, CF_COK|CF_VWM|CF_ICO, int, do_request, (void),)
{
    char *title   = av[1];
    char *body	  = av[2];
    char *gadgets = av[3];

    if (!ReqToolsBase) {
	error ("%s:\n`reqtools.library' is needed for that Command!\n", av[0]);
	return RET_FAIL;
    } /* if */

    ReqResult = rtEZRequestTags(body, gadgets, NULL, NULL,
	RTEZ_ReqTitle, title,

	RT_Window, Ep->win, TAG_END);
    strcpy (ReqString, ltostr(ReqResult));
    return RET_SUCC;
} /* do_request */

/*
*!
*! >REQFILE Title/A,Flags/A,InitValue/A
*!
*!  Interface to the ReqTools FileRequester
*!  Opens a synchroneous ReqTools Requester
*!  with the given values
*!
*!  The Result is written into ReqResult
*!  Cancelling the Requester sets globalflags.Abortcommand;
*!
*!  flags = [PSD]*;
*!	     P=Pattern On; S=Save Mode; D=Director Mode;
*!
*/
/*DEFHELP #cmd requester REQFILE title flags defvalue - display a synch ReqTools FileRequest; the result is put in $REQRESULT. */

DEFUSERCMD("reqfile", 3, CF_COK|CF_VWM|CF_ICO, int, do_reqfile, (void),)
{
    char *title = av[1];
    char *flags = av[2];
    char *value = av[3];
    unsigned long flg = FREQF_NOBUFFER;
    struct rtFileRequester *filereq;

    if (!ReqToolsBase) {
	error ("%s:\n`reqtools.library' is needed for that Command!\n", av[0]);
	return RET_FAIL;
    } /* if */

    while (*flags)
	switch (*(flags++)) {
	case 's':
	case 'S':
	    flg |= FREQF_SAVE;
	    break;
	case 'd':
	case 'D':
	    flg |= FREQF_NOFILES;
	    break;
	case 'p':
	case 'P':
	    flg |= FREQF_PATGAD;
	    break;
	default:
	    error ("%s:\nUnknown Flag: `%c'", av[0], *flags);
	    return RET_FAIL;
	}

    strcpy (tmp_buffer, value);
    if ((filereq = rtAllocRequestA (RT_FILEREQ, NULL))) {
	ReqResult = rtFileRequest (filereq, tmp_buffer, title,
	    RTFI_Flags, flg,
	    RT_Window, Ep->win,
	    TAG_END);
	if (ReqResult) {
	    strcpy (ReqString, filereq->Dir);
	    AddPart(ReqString, tmp_buffer, sizeof(ReqString));
	} else
	    SETF_ABORTCOMMAND(Ep, 1 );
	rtFreeRequest (filereq);
    } else
	nomemory();
    return RET_SUCC;
} /* do_reqfile */

/* Prototype int do_reqchoice (void);
int do_reqchoice (void) {
    char *title = av[1];

    if (!ReqToolsBase) {
	error ("%s:\n`reqtools.library' is needed for that Command!\n", av[0]);
	return RET_FAIL;
    }

    return RET_SUCC;
} /* do_reqchoice */






#ifdef STATIC_COM

    COMMAND("reqfile",       3, CF_COK|CF_VWM|CF_ICO, (FPTR)do_reqfile    )
    COMMAND("reqnumber",     6, CF_COK|CF_VWM|CF_ICO, (FPTR)do_reqnumber  )
    COMMAND("reqpalette",    2, CF_COK|CF_VWM|CF_ICO, (FPTR)do_reqpalette )
    COMMAND("reqstring",     4, CF_COK|CF_VWM|CF_ICO, (FPTR)do_reqstring  )
    COMMAND("request",       3, CF_COK|CF_VWM|CF_ICO, (FPTR)do_request    )

#endif

#ifdef SPC_VAR

const str reqresult = %{
    reference = ReqString;
    help      = %{
	Result of the recent call to any of the commands Request, ReqString,
	ReqNumber, ReqPalette or ReqFile, assuming the requestor command was
	not aborted nor cancelled.
    %};
%};

#endif

/******************************************************************************
*****  END Request.c
******************************************************************************/

