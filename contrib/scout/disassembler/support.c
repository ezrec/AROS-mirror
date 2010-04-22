#include <aros/cpu.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <exec/tasks.h>
#include <proto/arossupport.h>
#include <proto/exec.h>

#include <setjmp.h>

#include "dis-asm.h"
#include "library.h"
#include "support.h"
#include "bfd.h"

void set_machine_info(bfd *abfd, disassemble_info *dinfo)
{
    dinfo->arch = abfd->arch;
    dinfo->mach = abfd->mach;
    dinfo->endian = abfd->endian;
}

void set_memory_bounds(disassemble_info *dinfo, APTR address)
{
    struct MemHeader *mh;

    Forbid();

    mh=(struct MemHeader *)SysBase->MemList.lh_Head;
    while(mh->mh_Node.ln_Succ!=NULL)
    {
	if(address>=mh->mh_Lower&&address<mh->mh_Upper)
	{
	    dinfo->buffer_vma = (IPTR)mh->mh_Lower;
	    dinfo->buffer = mh->mh_Lower;
	    dinfo->buffer_length = mh->mh_Upper - mh->mh_Lower;
	    break;
	}
	mh=(struct MemHeader *)mh->mh_Node.ln_Succ;
    }
    Permit();
}
