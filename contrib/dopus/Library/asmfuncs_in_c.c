/* AROS: The asm code in functions.a converted into C */

#include "dopuslib.h"
#include <proto/arossupport.h>

/*****************************************************************************

    NAME */

	AROS_LH1(void, GetWBScreen,

/*  SYNOPSIS */
	AROS_LHA(struct Screen *, scrbuf, A0),

/*  LOCATION */
	struct Library *, DOpusBase, 51, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/

#define WBSCREEN_NAME "Workbench"
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	struct Screen *scr;
	
	if (IntuitionBase->LibNode.lib_Version < 36) {
		GetScreenData(scrbuf, sizeof (struct Screen), WBENCHSCREEN, NULL);
	} else {
kprintf("GetWB()\n");
		scr = LockPubScreen(WBSCREEN_NAME);
		if (NULL == scr) {
			kprintf("ALERT !! COULD NOT LOCK WBSCREEN\n");
		}
		CopyMem(scr, scrbuf, sizeof (struct Screen));
		UnlockPubScreen(WBSCREEN_NAME, scr);
		
	}
	
	AROS_LIBFUNC_EXIT
}




/*****************************************************************************

    NAME */

	AROS_LH1(void, Seed,

/*  SYNOPSIS */
	AROS_LHA(int , seed, D0),

/*  LOCATION */
	struct Library *, DOpusBase, 55, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	srand(seed);	
	
	AROS_LIBFUNC_EXIT
	
}


/*****************************************************************************

    NAME */

	AROS_LH1(int, Random,

/*  SYNOPSIS */
	AROS_LHA(int , limit, D0),

/*  LOCATION */
	struct Library *, DOpusBase, 56, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	return 1 + (int) (limit * rand() / (RAND_MAX + 1.0));
	
	AROS_LIBFUNC_EXIT
	
}


/*****************************************************************************

    NAME */

	AROS_LH2(void, StrToUpper,

/*  SYNOPSIS */
	AROS_LHA(char *, from, A0),
	AROS_LHA(char *, to, A1),

/*  LOCATION */
	struct Library *, DOpusBase, 57, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	char c;
	
	while ((c = *from ++)) {
		*to ++ = LToUpper(c);
	}
	
	AROS_LIBFUNC_EXIT
}

/*****************************************************************************

    NAME */

	AROS_LH2(void, StrToLower,

/*  SYNOPSIS */
	AROS_LHA(char *, from, A0),
	AROS_LHA(char *, to, A1),

/*  LOCATION */
	struct Library *, DOpusBase, 58, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	char c;
	
	while ((c = *from ++)) {
		*to ++ = LToLower(c);
	}
	
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH4(void, DisableGadget,

/*  SYNOPSIS */
	AROS_LHA(struct Gadget *, gad, A0),
	AROS_LHA(struct RastPort *, rp,  A1),
	AROS_LHA(int, xoff, D0),
	AROS_LHA(int, yoff, D1),

/*  LOCATION */
	struct Library *, DOpusBase, 80, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	if (!(gad->Flags & GFLG_DISABLED)) {				
		Forbid();
		gad->Flags |= GFLG_DISABLED;
		Permit();

		GhostGadget(gad, rp, xoff, yoff);
	}
	
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH4(void, EnableGadget,

/*  SYNOPSIS */
	AROS_LHA(struct Gadget *, gad, A0),
	AROS_LHA(struct RastPort *, rp,  A1),
	AROS_LHA(int, xoff, D0),
	AROS_LHA(int, yoff, D1),

/*  LOCATION */
	struct Library *, DOpusBase, 81, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	if (gad->Flags & GFLG_DISABLED) {
		Forbid();
		gad->Flags &= ~GFLG_DISABLED;
		Permit();
		
		GhostGadget(gad, rp, xoff, yoff);
	}
	
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */
    
const UWORD ditherdata[2] = { 0x8888, 0x2222 };

	AROS_LH4(void, GhostGadget,

/*  SYNOPSIS */
	AROS_LHA(struct Gadget *, gad, A0),
	AROS_LHA(struct RastPort *, rp,  A1),
	AROS_LHA(int, xoff, D0),
	AROS_LHA(int, yoff, D1),

/*  LOCATION */
	struct Library *, DOpusBase, 82, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	ULONG old_drmd;
	
	old_drmd = GetDrMd(rp);
	
	SetDrMd(rp, COMPLEMENT);
	
	SetAfPt(rp, (UWORD *)ditherdata, 1);
	
	RectFill(rp
		, gad->LeftEdge + xoff
		, gad->TopEdge  + yoff
		, gad->LeftEdge + gad->Width  - 1 - xoff
		, gad->TopEdge  + gad->Height - 1 - yoff
	);
	
	SetDrMd(rp, old_drmd);
	SetAfPt(rp, NULL, 0);
	
	AROS_LIBFUNC_EXIT
}


#if 0
/*****************************************************************************

    NAME */
    
    /* DrawRadioButton. An other copy of this func exists. Had to
    remove the header to make genfunctable.awk not generating two entries in
    functable.c

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	int new_x, new_y;
	
	SetAPen(rp, hi);
	
	new_x = x;
	new_y = y - 1;
	
	Move(rp, new_x, new_y);
	
	Draw(rp, x + w + 1, new_y - 2);
	
	Move(rp, new_x, new_y);
	
	Draw(rp, new_x, y + h);
	
	new_x = x - 1;
	Move(rp, new_x, new_y);
	
	new_y = y + h;
	Draw(rp, new_x, new_y - 1);
	
	SetAPen(rp, lo);
	
	Move(rp, new_x, new_y);
	
	new_x = x + w;
	Draw(rp, new_x + 1, new_y);
	
	Draw(rp, new_x + 1, y - 1);
	
	Move(rp, new_x, new_y);
	Draw(rp, new_x, y);
	
	
	AROS_LIBFUNC_EXIT
}
#endif

#include <proto/exec.h>
#include <aros/asmcall.h>

void SwapMem(char *src, char *dst, int size)
{
	register char tmp;
	register int i;
	
	for (i = 0; i < size; i ++) {
		tmp = *dst;
		*dst ++ = *src;
		*src ++ = tmp;
	}
}

AROS_UFH2 (void, stuffChar, 
	AROS_UFHA(char, c, D0),
	AROS_UFHA(char **, buf, A3)
)
{
	AROS_USERFUNC_INIT
	*(*buf) ++ = c;
	AROS_USERFUNC_EXIT
}

void LSprintf(char *str, char *fmt, ...)
{
	char *buf;
	
	buf = str;
	
	RawDoFmt(fmt,&fmt+1,(VOID_FUNC)stuffChar, &buf);

}

	

/*****************************************************************************

    NAME */

	AROS_LH2(struct IORequest *, LCreateExtIO,

/*  SYNOPSIS */
	AROS_LHA(struct MsgPort *, port, A0),
	AROS_LHA(int , size,  D0),

/*  LOCATION */
	struct Library *, DOpusBase, 11, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	struct IORequest *io;
	
	if (0 == size) {
		return NULL;
	}
	
	io = CreateIORequest(port, size);
	if (NULL != io) {
		io->io_Message.mn_Node.ln_Type = NT_MESSAGE;
	}
	
	return io;
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH2(struct MsgPort *, LCreatePort,

/*  SYNOPSIS */
	AROS_LHA(char *, name, A0),
	AROS_LHA(int , pri,  D0),

/*  LOCATION */
	struct Library *, DOpusBase, 12, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	struct MsgPort *port;
	
	port = CreateMsgPort();
	if (NULL != port) {
		/* Set port name and pri */
		port->mp_Node.ln_Name = name;
		port->mp_Node.ln_Pri = pri;
		
		if (NULL != name) {
			/* public port, add it */
			AddPort(port);
		}
	}
	
	return port;
	
	AROS_LIBFUNC_EXIT
}



/*****************************************************************************

    NAME */

	AROS_LH1(void, LDeleteExtIO,

/*  SYNOPSIS */
	AROS_LHA(struct IORequest *, ioext, A0),

/*  LOCATION */
	struct Library *, DOpusBase, 13, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	DeleteIORequest(ioext);
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH1(void, LDeletePort,

/*  SYNOPSIS */
	AROS_LHA(struct MsgPort *, port, A0),

/*  LOCATION */
	struct Library *, DOpusBase, 14, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	if (NULL != port->mp_Node.ln_Name) {
		RemPort(port);
	}
	
	DeleteMsgPort(port);
	
	AROS_LIBFUNC_EXIT
}



/*****************************************************************************

    NAME */

	AROS_LH1(char, LToUpper,

/*  SYNOPSIS */
	AROS_LHA(char, ch, D0),

/*  LOCATION */
	struct Library *, DOpusBase, 15, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	if (ch >= 'a' && ch <= 'z') {
		ch -= 'a' - 'A';
	}
	
	return ch;
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH1(char, LToLower,

/*  SYNOPSIS */
	AROS_LHA(char, ch, D0),

/*  LOCATION */
	struct Library *, DOpusBase, 16, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	if (ch >= 'A' && ch <= 'z') {
		ch += 'a' - 'A';
	}
	
	return ch;
	
	AROS_LIBFUNC_EXIT
}




/*****************************************************************************

    NAME */

	AROS_LH2(void, LStrCat,

/*  SYNOPSIS */
	AROS_LHA(char *, s1, A0),
	AROS_LHA(char *, s2, A1),

/*  LOCATION */
	struct Library *, DOpusBase, 17, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	while (*s1)
		s1 ++;
		
	while (( *s1 ++ = *s2 ++ ))
		;
	*s1 = 0;
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH3(void, LStrnCat,

/*  SYNOPSIS */
	AROS_LHA(char *, s1, A0),
	AROS_LHA(char *, s2, A1),
	AROS_LHA(int, len, D0),

/*  LOCATION */
	struct Library *, DOpusBase, 18, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	while (*s1)
		s1 ++;
		
	/* Take 0 termination into account */
	len --;
		
	while ( len -- && ( *s1 ++ = *s2 ++ ))
		;
	*s1 = 0;
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH2(void, LStrCpy,

/*  SYNOPSIS */
	AROS_LHA(char *, to, A0),
	AROS_LHA(char *, from, A1),

/*  LOCATION */
	struct Library *, DOpusBase, 19, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	while ( ( *to ++ = *from ++ ))
		;
	
	*to = 0;
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH3(void, LStrnCpy,

/*  SYNOPSIS */
	AROS_LHA(char *, to, A0),
	AROS_LHA(char *, from, A1),
	AROS_LHA(int, len, A1),

/*  LOCATION */
	struct Library *, DOpusBase, 20, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	while (len-- >= 0)
	{
	    char c = *from++;
	    
	    *to++ = c;
	    
	    if (c == '\0') break;
	}
	
	to[-1] = '\0';
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH2(int, LStrCmpI,

/*  SYNOPSIS */
	AROS_LHA(char *, s1, A0),
	AROS_LHA(char *, s2, A1),

/*  LOCATION */
	struct Library *, DOpusBase, 23, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	register char c1, c2, diff, ret;
	
	for (;;) {
		c1 = LToUpper(*s1 ++);
		c2 = LToUpper(*s2 ++);
		
		diff = c1 - c2;
		
		if (diff > 0) {
			ret = 1;
			break;
		} else if (diff < 0) {
			ret = -1;
			break;
		} else {
		
			if (0 == c1) {
				ret = 0;
				break;
			}
		}
		
	}
	return ret;
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH3(int, LStrnCmpI,

/*  SYNOPSIS */
	AROS_LHA(char *, s1, A0),
	AROS_LHA(char *, s2, A1),
	AROS_LHA(int, len, A1),

/*  LOCATION */
	struct Library *, DOpusBase, 24, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	register char c1, c2, diff, ret;

	for (;;) {
		c1 = LToUpper(*s1 ++);
		c2 = LToUpper(*s2 ++);
		
		diff = c1 - c2;
		
		if (diff > 0) {
			ret = 1;
			break;
		} else if (diff < 0) {
			ret = -1;
			break;
		} else {
		
			if (0 == c1 || 0 == len) {
				ret = 0;
				break;
			}
		}
		
	}
	return ret;
	AROS_LIBFUNC_EXIT
}



/*****************************************************************************

    NAME */

	AROS_LH3(int, LStrnCmp,

/*  SYNOPSIS */
	AROS_LHA(char *, s1, A0),
	AROS_LHA(char *, s2, A1),
	AROS_LHA(int, len, A1),

/*  LOCATION */
	struct Library *, DOpusBase, 22, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	register char c1, c2, diff, ret;
	
	for (;;) {
		c1 = *s1 ++;
		c2 = *s2 ++;
		
		diff = c1 - c2;
		
		if (diff > 0) {
			ret = 1;
			break;
		} else if (diff < 0) {
			ret = -1;
			break;
		} else {
		
			if (0 == c1 || 0 == len) {
				ret = 0;
				break;
			}
		}
		
	}
	return ret;
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH2(int, LStrCmp,

/*  SYNOPSIS */
	AROS_LHA(char *, s1, A0),
	AROS_LHA(char *, s2, A1),

/*  LOCATION */
	struct Library *, DOpusBase, 21, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	register char c1, c2, diff, ret;
	
	for (;;) {
		c1 = *s1 ++;
		c2 = *s2 ++;
		
		diff = c1 - c2;
		
		if (diff > 0) {
			ret = 1;
			break;
		} else if (diff < 0) {
			ret = -1;
			break;
		} else {
		
			if (0 == c1) {
				ret = 0;
				break;
			}
		}
		
	}
	return ret;
	AROS_LIBFUNC_EXIT
}

