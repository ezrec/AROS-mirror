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

#ifdef __i386__
#define DEFAULT_ARCH bfd_arch_i386
#define DEFAULT_MACH bfd_mach_i386_i386_intel_syntax
#endif

#ifdef __M68000__
#define DEFAULT_ARCH bfd_arch_m68k
#endif

#ifdef __PPC__
#define DEFAULT_ARCH bfd_arch_powerpc;
#endif

#ifdef __x86_64__
#define DEFAULT_ARCH bfd_arch_i386
#define DEFAULT_MACH bfd_mach_x86_64_intel_syntax
#endif

#ifndef DEFAULT_ARCH
#define DEFAULT_ARCH bfd_arch_unknown

#warning Default CPU is not supported, please add definitions

#endif

#ifndef DEFAULT_MACH
#define DEFAULT_MACH 0
#endif

void set_default_machine(bfd *abfd, disassemble_info *dinfo)
{
    dinfo->arch = DEFAULT_ARCH; 
    dinfo->mach = DEFAULT_MACH;
#if AROS_BIG_ENDIAN
    dinfo->endian = BFD_ENDIAN_BIG;
#else
    dinfo->endian = BFD_ENDIAN_LITTLE;
#endif

    abfd->arch = dinfo->arch;
    abfd->endian = dinfo->endian;
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

void _abort(char *file, unsigned int line)
{
    struct Task *me;
    struct AbortContext *ctx;

    me = FindTask(NULL);
    ctx = (struct AbortContext *)me->tc_UserData;
    ctx->file = file;
    ctx->line = line;
    longjmp(ctx->buf, -1);
}
