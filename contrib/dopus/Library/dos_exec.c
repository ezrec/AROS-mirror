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

int __saveds DoSendPacket(register struct MsgPort *port __asm("a0"), register int action __asm("d0"), register IPTR *args __asm("a1"), register int nargs __asm("d1"))
{
    struct StandardPacket *packet;
    struct MsgPort *repport;
    struct Process *myproc;
    int count,res1=0;
    ULONG *pargs;

    if (packet=AllocMem(sizeof(struct StandardPacket),MEMF_PUBLIC|MEMF_CLEAR))
     {
      if (repport=CreatePort(NULL,0))
       {
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

        if (myproc=(struct Process *)FindTask(NULL)) myproc->pr_Result2=packet->sp_Pkt.dp_Res2;

        DeletePort(repport);
       }
      FreeMem(packet,sizeof(struct StandardPacket));
     }
    return res1;
}

struct DOpusPool
 {
  APTR fast;
  APTR chip;
 };

APTR __saveds DoAllocRemember(register struct DOpusRemember **key __asm("a0"), register ULONG size __asm("d0"), register ULONG type __asm("d1"))
{
    APTR *pool;
    ULONG *mem;

    if (*key == NULL)
     {
      *key = AllocMem(sizeof(struct DOpusPool),MEMF_ANY | MEMF_CLEAR);
      if (*key == NULL) return (0);
     }
//kprintf("LAllocRemember(%lx,%ld,%s): ",*key,size,(type & MEMF_CHIP)?"CHIP":"ANY");
    if (type & MEMF_CHIP) pool = &((struct DOpusPool *)*key)->chip;
    else pool = &((struct DOpusPool *)*key)->fast;

    if (*pool == NULL) *pool = CreatePool(type | MEMF_CLEAR, 1024, 1024);
//kprintf("pool = %lx\t",*pool);
    size += sizeof(ULONG) * 2;
    if (*pool) if (mem = AllocPooled(*pool,size))
     {
//kprintf("mem = %lx\n",mem);
      mem[0] = type;
      mem[1] = size;
      return (mem+2);
     }
//kprintf("error!\n");
    return 0;
/*
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
    return((ULONG)size);
*/
}

void __saveds DoFreeRemember(register struct DOpusRemember **key __asm("a0"))
{
    struct DOpusPool *pool = (struct DOpusPool *)(*key);
    if (*key)
     {
//kprintf("LFreeRemember(%lx)\n",*key);
      if (pool->fast) DeletePool(pool->fast);
      if (pool->chip) DeletePool(pool->chip);
      FreeMem(*key,sizeof(struct DOpusPool));
      *key = NULL;
     }
/*
    struct DOpusRemember *node,*cur;

    if ((*key)==NULL) return;
    node=(*key);
    while (node) {
        cur=node->next;
        FreeMem(node,node->size);
        node=cur;
    }
    (*key)=NULL;
*/
}

void __saveds DoFreeRemEntry(register struct DOpusRemember **key __asm("a0"), register char *pointer __asm("a1"))
{
    struct DOpusPool *pool;
    ULONG *ptr = (ULONG *)pointer;

    if ((*key) == NULL) return;
    if (*(ptr-2) & MEMF_CHIP) pool = ((struct DOpusPool *)(*key))->chip;
    else pool = ((struct DOpusPool *)(*key))->fast;

    FreePooled(pool,ptr-2,*(ptr-1));
//kprintf("LFreeRemEntry(%lx,%lx): pool = %lx\n",*key,ptr-1,pool);
/*
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
*/
}

struct IORequest * __saveds DoCreateExtIO(register struct MsgPort *port __asm("a0"), register int size __asm("d0"))
 {
D(bug("LCreateExtIO()\n")/*;Delay(50)*/;)
  return CreateExtIO(port,size);
 }

void __saveds DoDeleteExtIO(register struct IORequest *extio __asm("a0"))
 {
D(bug("LDeleteExtIO()\n");/*Delay(50);*/)
  DeleteExtIO(extio);
 }

struct MsgPort * __saveds DoCreatePort(register char *name __asm("a0"), register int pri __asm("d0"))
 {
D(bug("LCreatePort()\n");/*Delay(50);*/)
  return CreatePort(name,pri);
 }

void __saveds DoDeletePort(register struct MsgPort *port __asm("a0"))
 {
D(bug("LDeletePort()\n");/*Delay(50);*/)
  DeletePort(port);
 }


