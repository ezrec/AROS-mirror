#include "ownincs/VT340console.h"
#include "csource/VT340/VT340_rexx_var.c"

/* allowed globals.. */
struct	ExecBase			*SysBase;
struct	DosLibrary		*DOSBase;
struct	GfxBase			*GfxBase;
struct	IntuitionBase	*IntuitionBase;
struct	Library			*RexxSysBase;


              /* REXX Init-Funktionen zu VT340 Emulator.. */



STATIC BOOL GetUniquePortName(UBYTE *name)
{
	BOOL unique = FALSE;

	Forbid();

	if(FindPort(name))
	{
		REGISTER UBYTE *nptr = name;

		while(*nptr)
			nptr++;

		if(nptr[1] == '\0')
		{
			UBYTE i, first, last;

			nptr--;
			if(*nptr >= '0'  &&  *nptr <= '9')
			{
				first = 'a';
				last = 'z';
			}
			else
			{
				first = '1';
				last = '9';
			}
			nptr++;

			for(i=first; i<=last; i++)
			{	
				*nptr = i;

				if(FindPort(name) == NULL)
				{
					unique = TRUE;
					break;	
				}
			}	
		}
	}	
	else
		unique = TRUE;

	Permit();

	return(unique);
}


VOID VT340_openrexx(struct VT340Console *con)
{
	VT340_closerexx(con);

	if(con->rexxvars = InvokeRexx())
	{
		GetUniquePortName(con->rexxname);
		con->rexxsig = upRexxPort(con->rexxvars, con->rexxname, VT340_RexxCommandList, NULL, VT340_RexxDispatcher, con);
	}

	*con->io->xem_signal |= con->rexxsig;

	if(con->rexxsig == 0)
		con->rexxname[0] = '\0';
	else
		VT340_sendrexx(con, "ENV:vt340/vt340.setup");
}


VOID VT340_closerexx(struct VT340Console *con)
{
	dnRexxPort(con->rexxvars);
	ShutRexx(con->rexxvars);
	con->rexxvars = NULL;
	con->rexxsig = 0;
}


BOOL VT340_sendrexx(struct VT340Console *con, UBYTE *cmd)
{
	return(asyncRexxCmd(con->rexxvars, cmd) != NULL);
}


ULONG VT340_RexxDispatcher(APTR vars, struct VT340Console *con, struct RexxMsg *msg, struct rexxCommandList *rcl, UBYTE *p)
{
	p++;		/* kill this ugly white space leading the arg-string */

	if(((BOOL (*)())(rcl->userdata))(vars, (APTR)con, msg, p))
		return(1);	/* handler replied rexx-msg */
	else
		return(0);	/* rexx-interface shall reply rexx-msg with '0, 0, NULL' */
}


BOOL VT340_reset(APTR var, struct VT340Console *con, struct RexxMsg *msg, UBYTE *p)
{
	VT340_resetcon(con, TRUE);

	if(con->ustat & U_CURSOR)
		VT340_cursorflip(con);

	return(0);		/* rexx-msg is not replied */
}

BOOL VT340_setclass(APTR var, struct VT340Console *con, struct RexxMsg *msg, UBYTE *p)
{
	if(!stricmp(p, "VT52"))
	{
		con->r_pstat |= (P_VT340_MODE | P_CONVERT);
		con->r_pstat &= ~P_ANSI_MODE;
	}
	else
	{
		if(!stricmp(p, "VT102"))
		{
			con->r_pstat |=  (P_ANSI_MODE | P_CONVERT);
			con->r_pstat &= ~P_VT340_MODE;
		}
		else
		{
			if(!stricmp(p, "VT340_7"))
				con->r_pstat |=  (P_ANSI_MODE | P_VT340_MODE | P_CONVERT);
			else
			{
				con->r_pstat |=  (P_ANSI_MODE | P_VT340_MODE);
				con->r_pstat &= ~P_CONVERT;
			}
		}
	}

	return(0);		/* rexx-msg is not replied */
}


BOOL VT340_setid(APTR var, struct VT340Console *con, struct RexxMsg *msg, UBYTE *p)
{
	if(!stricmp(p, "VT100"))
		con->r_identify = IDENT_VT100;
	else
	{
		if(!stricmp(p, "VT101"))
			con->r_identify = IDENT_VT101;
		else
		{
			if(!stricmp(p, "VT102"))
				con->r_identify = IDENT_VT102;
			else
			{
				if(!stricmp(p, "VT220"))
					con->r_identify = IDENT_VT220;
				else
					con->r_identify = IDENT_VT340;
			}
		}
	}

	return(0);		/* rexx-msg is not replied */
}


BOOL VT340_newlinemode(APTR var, struct VT340Console *con, struct RexxMsg *msg, UBYTE *p)
{
	if(!stricmp(p, "ON"))
		con->r_ustat |= U_NEWLINE;
	else
		con->r_ustat &= ~U_NEWLINE;

	return(0);		/* rexx-msg is not replied */
}


BOOL VT340_insertmode(APTR var, struct VT340Console *con, struct RexxMsg *msg, UBYTE *p)
{
	if(!stricmp(p, "ON"))
		con->r_ustat |= U_INSERT;
	else
		con->r_ustat &= ~U_INSERT;

	return(0);		/* rexx-msg is not replied */
}


BOOL VT340_wrapmode(APTR var, struct VT340Console *con, struct RexxMsg *msg, UBYTE *p)
{
	if(!stricmp(p, "ON"))
		con->r_tstat |= T_WRAP;
	else
		con->r_tstat &= ~T_WRAP;

	return(0);		/* rexx-msg is not replied */
}


BOOL VT340_cursorvisible(APTR var, struct VT340Console *con, struct RexxMsg *msg, UBYTE *p)
{
	if(!stricmp(p, "ON"))
		con->r_ustat |= U_CURSOR;
	else
		con->r_ustat &= ~U_CURSOR;

	return(0);		/* rexx-msg is not replied */
}


BOOL VT340_smoothscroll(APTR var, struct VT340Console *con, struct RexxMsg *msg, UBYTE *p)
{
	if(!stricmp(p, "ON"))
		con->r_pstat |= P_SMOOTH_SCROLL;
	else
		con->r_pstat &= ~P_SMOOTH_SCROLL;

	return(0);		/* rexx-msg is not replied */
}


BOOL VT340_use24lines(APTR var, struct VT340Console *con, struct RexxMsg *msg, UBYTE *p)
{
	if(!stricmp(p, "OFF"))
		con->r_pstat &= ~P_24LINES;
	else
		con->r_pstat |= P_24LINES;

	return(0);		/* rexx-msg is not replied */
}


BOOL VT340_adjustlines(APTR var, struct VT340Console *con, struct RexxMsg *msg, UBYTE *p)
{
	if(!stricmp(p, "ON"))
		con->r_pstat |= P_ADJUST_LINES;
	else
		con->r_pstat &= ~P_ADJUST_LINES;

	return(0);		/* rexx-msg is not replied */
}


BOOL VT340_destructivebs(APTR var, struct VT340Console *con, struct RexxMsg *msg, UBYTE *p)
{
	if(!stricmp(p, "ON"))
		con->r_pstat |= P_DESTRUCTIVEBS;
	else
		con->r_pstat &= ~P_DESTRUCTIVEBS;

	return(0);		/* rexx-msg is not replied */
}


BOOL VT340_destructiveff(APTR var, struct VT340Console *con, struct RexxMsg *msg, UBYTE *p)
{
	if(!stricmp(p, "ON"))
		con->r_pstat |= P_DESTRUCTIVEFF;
	else
		con->r_pstat &= ~P_DESTRUCTIVEFF;

	return(0);		/* rexx-msg is not replied */
}


BOOL VT340_destructivedel(APTR var, struct VT340Console *con, struct RexxMsg *msg, UBYTE *p)
{
	if(!stricmp(p, "ON"))
		con->r_pstat |= P_DESTRUCTIVEDEL;
	else
		con->r_pstat &= ~P_DESTRUCTIVEDEL;

	return(0);		/* rexx-msg is not replied */
}


BOOL VT340_swapbsdel(APTR var, struct VT340Console *con, struct RexxMsg *msg, UBYTE *p)
{
	if(!stricmp(p, "OFF"))
		con->r_pstat &= ~P_BSDEL;
	else
		con->r_pstat |= P_BSDEL;

	return(0);		/* rexx-msg is not replied */
}


BOOL VT340_useisolatin1(APTR var, struct VT340Console *con, struct RexxMsg *msg, UBYTE *p)
{
	if(!stricmp(p, "ON"))
		con->r_pstat |= P_ISO_LATIN1;
	else
		con->r_pstat &= ~P_ISO_LATIN1;

	return(0);		/* rexx-msg is not replied */
}


BOOL VT340_savescreento(APTR var, struct VT340Console *con, struct RexxMsg *msg, UBYTE *p)
{
	stccpy(con->screen_buf, p, SCREEN_BUF);

	return(0);		/* rexx-msg is not replied */
}


BOOL VT340_answerbackmsg(APTR var, struct VT340Console *con, struct RexxMsg *msg, UBYTE *p)
{
	stccpy(con->answer_back, p, ANSWER_BACK);

	return(0);		/* rexx-msg is not replied */
}


BOOL VT340_titlebar(APTR var, struct VT340Console *con, struct RexxMsg *msg, UBYTE *p)
{
	if(!(con->ustat & U_HOST_STATUSLINE))
	{
		if(!stricmp(p, "ON"))
			con->r_pstat |= P_OWN_STATUSLINE;
		else
			con->r_pstat &= ~P_OWN_STATUSLINE;
	}

	return(0);		/* rexx-msg is not replied */
}


BOOL VT340_emit(APTR var, struct VT340Console *con, struct RexxMsg *msg, UBYTE *p)
{
	return(0);		/* rexx-msg is not replied */
}


/* end of source-code */
