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

	AROS_LH4(int, SendPacket,

/*  SYNOPSIS */
	AROS_LHA(struct MsgPort *, port, A0),
	AROS_LHA(int , action, 		D0),
	AROS_LHA(ULONG * , args, 	A1),
	AROS_LHA(int , nargs, 		D1),

/*  LOCATION */
	struct Library *, DOpusBase, 36, DOpus)
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

	struct StandardPacket *packet;
	struct MsgPort *repport;
	struct Process *myproc;
	int count,res1,a;
	ULONG *pargs;
	
	
	kprintf("!!! SendPacket() CALLED. PACKETS ARE NOT IMPLEMENTED IN AROS !!!\n");

	myproc=(struct Process *)FindTask(NULL);
	if (!(packet=AllocMem(sizeof(struct StandardPacket),MEMF_PUBLIC|MEMF_CLEAR)))
		return(0);
	if (!(repport=LCreatePort(NULL,0))) {
		FreeMem(packet,sizeof(struct StandardPacket));	
		return(0);
	}

	packet->sp_Msg.mn_Node.ln_Name=(char *)&(packet->sp_Pkt);
	packet->sp_Pkt.dp_Link=&(packet->sp_Msg);
	packet->sp_Pkt.dp_Port=repport;
	packet->sp_Pkt.dp_Type=action;

	pargs=(ULONG *)&(packet->sp_Pkt.dp_Arg1);
	if (nargs>7) nargs=7;
	for (count=0;count<nargs;count++) pargs[count]=args[count];

	PutMsg(port,(struct Message *)packet);
	WaitPort(repport);
	GetMsg(repport);

	res1=packet->sp_Pkt.dp_Res1;
	a=packet->sp_Pkt.dp_Res2;
	FreeMem(packet,sizeof(struct StandardPacket));
	LDeletePort(repport);
	if (myproc) myproc->pr_Result2=a;
	return(res1);   
	
	AROS_LIBFUNC_EXIT
}

/*****************************************************************************

    NAME */

	AROS_LH3(void *, LAllocRemember,

/*  SYNOPSIS */
	AROS_LHA(struct DOpusRemember **, key, A0),
	AROS_LHA(ULONG, size, 	D0),
	AROS_LHA(ULONG, type, 	D1),

/*  LOCATION */
	struct Library *, DOpusBase, 48, DOpus)
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

	struct DOpusRemember *node,*cur;
	size+=sizeof(struct DOpusRemember);
	if (!(node=AllocMem(size,type))) return(NULL);
	node->next=NULL;
	if ((*key)==NULL) {
		node->current=node;
		(*key)=node;
	}
	else {
		cur=(*key)->current;
		(*key)->current=node;
		cur->next=node;
	}
	node->size=size;
	size=(ULONG)node+sizeof(struct DOpusRemember);
	return (void *)((ULONG)size);
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH1(void, LFreeRemember,

/*  SYNOPSIS */
	AROS_LHA(struct DOpusRemember **, key, A0),

/*  LOCATION */
	struct Library *, DOpusBase, 49, DOpus)
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

	struct DOpusRemember *node,*cur;

	if ((*key)==NULL) return;
	node=(*key);
	while (node) {
		cur=node->next;
		FreeMem(node,node->size);
		node=cur;
	}
	(*key)=NULL;
	
	AROS_LIBFUNC_EXIT
}

/*****************************************************************************

    NAME */

	AROS_LH2(void, LFreeRemEntry,

/*  SYNOPSIS */
	AROS_LHA(struct DOpusRemember **, key, A0),
	AROS_LHA(char *, pointer, A1),

/*  LOCATION */
	struct Library *, DOpusBase, 97, DOpus)
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

	struct DOpusRemember *node,*last=NULL;

	if ((*key)==NULL) return;
	pointer-=sizeof(struct DOpusRemember);

	node=(*key);
	while (node) {
		if (node==(struct DOpusRemember *)pointer) {
			if (last) last->next=node->next;
			if ((*key)->current==node) (*key)->current=last;
			if (node==(*key)) {
				last=(*key)->current;
				if (((*key)=node->next)) (*key)->current=last;
			}
			FreeMem(node,node->size);
			break;
		}
		last=node;
		node=node->next;
	}
	
	AROS_LIBFUNC_EXIT
}
