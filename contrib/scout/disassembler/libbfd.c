#ifdef __AROS__
#include <aros/cpu.h>
#else
#define AROS_BIG_ENDIAN 1
#endif

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

bfd_vma
bfd_getb32 (addr)
     register const bfd_byte *addr;
{
  unsigned long v;

  v = (unsigned long) addr[0] << 24;
  v |= (unsigned long) addr[1] << 16;
  v |= (unsigned long) addr[2] << 8;
  v |= (unsigned long) addr[3];
  return (bfd_vma) v;
}

bfd_vma
bfd_getl32 (addr)
     register const bfd_byte *addr;
{
  unsigned long v;

  v = (unsigned long) addr[0];
  v |= (unsigned long) addr[1] << 8;
  v |= (unsigned long) addr[2] << 16;
  v |= (unsigned long) addr[3] << 24;
  return (bfd_vma) v;
}

void init_default_bfd(bfd *abfd)
{
    abfd->arch = DEFAULT_ARCH; 
    abfd->mach = DEFAULT_MACH;
#if AROS_BIG_ENDIAN
    abfd->endian = BFD_ENDIAN_BIG;
#else
    abfd->endian = BFD_ENDIAN_LITTLE;
#endif
}
