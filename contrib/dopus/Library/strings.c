/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#include "dopuslib.h"

/*****************************************************************************

    NAME */

	AROS_LH4(int, StrCombine,

/*  SYNOPSIS */
	AROS_LHA(char *, buf, A0),
	AROS_LHA(char *, one, A1),
	AROS_LHA(char *, two, A2),
	AROS_LHA(int, lim, D0),

/*  LOCATION */
	struct Library *, DOpusBase, 25, DOpus)
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
	
	register int a;

	a=strlen(one); if (a>=lim) a=lim-1;
	strncpy(buf,one,a); buf[a]=0;
	return(StrConcat(buf,two,lim));

	AROS_LIBFUNC_EXIT
}

/*****************************************************************************

    NAME */

	AROS_LH3(int, StrConcat,

/*  SYNOPSIS */
	AROS_LHA(char *, buf, A0),
	AROS_LHA(char *, cat, A1),
	AROS_LHA(int, lim, D0),

/*  LOCATION */
	struct Library *, DOpusBase, 26, DOpus)
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

	register int a,b;

	a=strlen(cat); b=strlen(buf);
	--lim;
	if (a+b<lim) {
		strncpy(&buf[b],cat,a); buf[b+a]=0;
		return(1);
	}
	if (lim>b) strncpy(&buf[b],cat,lim-b); buf[lim]=0;
	return(0);
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH2(int, Atoh,

/*  SYNOPSIS */
	AROS_LHA(char *, buf, A0),
	AROS_LHA(int, len, D0),

/*  LOCATION */
	struct Library *, DOpusBase, 66, DOpus)
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

	int a,c,d,e,f;

	c=e=0;
	for (a=0;;a++) {
		if (!buf[a] || !((buf[a]>='0' && buf[a]<='9') || (buf[a]>='a' && buf[a]<='f') ||
			(buf[a]>='A' && buf[a]<='F'))) break;
		if ((++e)==len) break;
	}
	if (e==0) return(0);
	f=1;
	for (a=1;a<e;a++) f*=16;
	for (a=0;a<e;a++) {
		if (buf[a]>='0' && buf[a]<='9') d=buf[a]-'0';
		else if (buf[a]>='A' && buf[a]<='F') d=10+(buf[a]-'A');
		else if (buf[a]>='a' && buf[a]<='f') d=10+(buf[a]-'a');
		c+=(d*f);
		f/=16;
	}
	return(c);
	
	AROS_LIBFUNC_EXIT
}
