/*
 * loadelf.h
 */

#ifndef LOADELF_H
#define LOADELF_H

#include "types.h"

/* ELF HEADER */
struct elf_header {
  u8  e_ident[16];
  u16 e_type;
  u16 e_machine;
  u32 e_version;
  u32 e_entry;
  u32 e_phoff;
  u32 e_shoff;
  u32 e_flags;
  u16 e_ehsize;
  u16 e_phentsize;
  u16 e_phnum;
  u16 e_shentsize;
  u16 e_shnum;
  u16 e_shstrndx;
};

/* e_type PROTOTYPES */
#define ET_NONE    0
#define ET_REL     1
#define ET_EXEC    2
#define ET_DYN     3
#define ET_CORE    4
#define ET_LOPROC  0xff00
#define ET_HIPROC  0xffff

/* e_machine PROTOTYPES */
#define EM_NONE          0
#define EM_M32           1
#define EM_SPARC         2
#define EM_386           3
#define EM_68K           4
#define EM_88K           5
#define EM_860           7
#define EM_MIPS          8
#define EM_MIPS_RS4_BE  10
#define EM_SPARC64      11
#define EM_PARISC       15
#define EM_SPARC32PLUS  18
#define EM_PPC          20

/* e_ident INDEXES */
#define EI_MAG0     0
#define EI_MAG1     1
#define EI_MAG2     2
#define EI_MAG3     3
#define EI_CLASS    4
#define EI_DATA     4
#define EI_VERSION  4
#define EI_PAD      4

/* EI_CLASS */
#define ELFCLASSNONE  0
#define ELFCLASS32    1
#define ELFCLASS64    2

/* EI_DATA */
#define ELFDATANONE  0
#define ELFDATA2LSB  1
#define ELFDATA2MSB  2

/* SYMBOL TABLE HEADER */
struct elf_symbol {
  u32 st_name;
  u32 st_addr;
  u32 st_size;

#ifdef MSB_FIRST
  u8  st_bind:4,
      st_type:4;
#else
  u8  st_type:4,
      st_bind:4;
#endif

  u8  st_other;
  u16 st_shndx;
};

#define SIZE_SYMBOL  16

#define STB_LOCAL    0
#define STB_GLOBAL   1
#define STB_WEAK     2
#define STB_LOPROC  13
#define STB_HIPROC  15

#define STT_NOTYPE   0
#define STT_OBJECT   1
#define STT_FUNC     2
#define STT_SECTION  3
#define STT_FILE     4
#define STT_LOPROC  13
#define STT_HIPROC  15

/* SIMPLE RELOCATION HEADER */
struct elf_relocation {
  u32 r_offset;

#ifdef MSB_FIRST
  u32 r_sym:24,
      r_type:8;
#else
  u32 r_type:8,
      r_sym:24;
#endif

  u32 r_addend;
};

#define SIZE_REL      8
#define SIZE_RELA    12
#define R_SYM(i)     (i >> 8)
#define R_TYPE(i)    (i & 0xff)
#define R_INFO(s,t)  ((s << 8) | (t & 0xff))

/* MIPS */
#define R_MIPS_32    2
#define R_MIPS_26    4
#define R_MIPS_HI16  5
#define R_MIPS_LO16  6

/* X86 */
#define R_386_NONE      0
#define R_386_32        1
#define R_386_PC32      2
#define R_386_GOT32     3
#define R_386_PLT32     4
#define R_386_COPY      5
#define R_386_GLOB_DAT  6
#define R_386_JMP_SLOT  7
#define R_386_RELATIVE  8
#define R_386_GOTOFF    9
#define R_386_GOTPC    10

/* POWERPC */
#define R_PPC_NONE             0
#define R_PPC_ADDR32           1
#define R_PPC_ADDR24           2
#define R_PPC_ADDR16           3
#define R_PPC_ADDR16_LO        4
#define R_PPC_ADDR16_HI        5
#define R_PPC_ADDR16_HA        6
#define R_PPC_ADDR14           7
#define R_PPC_ADDR14_BRTAKEN   8
#define R_PPC_ADDR14_BRNTAKEN  9
#define R_PPC_REL24           10
#define R_PPC_REL14           11
#define R_PPC_REL14_BRTAKEN   12
#define R_PPC_REL14_BRNTAKEN  13
#define R_PPC_GOT16           14
#define R_PPC_GOT16_LO        15
#define R_PPC_GOT16_HI        16
#define R_PPC_GOT16_HA        17
#define R_PPC_PLTREL24        18
#define R_PPC_COPY            19
#define R_PPC_GLOB_DAT        20
#define R_PPC_JMP_SLOT        21
#define R_PPC_RELATIVE        22
#define R_PPC_LOCAL24PC       23
#define R_PPC_UADDR32         24
#define R_PPC_UADDR16         25
#define R_PPC_REL32           26
#define R_PPC_PLT32           27
#define R_PPC_PLTREL32        28
#define R_PPC_PLT16_LO        29
#define R_PPC_PLT16_HI        30
#define R_PPC_PLT16_HA        31
#define R_PPC_SDAREL16        32
#define R_PPC_SECTOFF         33
#define R_PPC_SECTOFF_LO      34
#define R_PPC_SECTOFF_HI      35
#define R_PPC_SECTOFF_HA      36

/* SECTION HEADER */
struct elf_section_header {
  /* ELF STANDARDS */
  u32 sh_name;
  u32 sh_type;
  u32 sh_flags;
  u32 sh_addr;
  u32 sh_offset;
  u32 sh_size;
  u32 sh_link;
  u32 sh_info;
  u32 sh_addralign;
  u32 sh_entsize;

  /* added to easy my life */
  u8 *data;
  u8 *name;
  struct elf_section_header *link, *info;

  struct elf_relocation **relocation_table;
  int relocation_count;

  struct elf_symbol **symbol_table;
  int symbol_count;
};

#define SECTION_STR( section, value) \
  (value > section->sh_size || !section->data) ? (u8 *)"" : &section->data[value]

/* sh_type */
#define SHT_NULL      0
#define SHT_PROGBITS  1
#define SHT_SYMTAB    2
#define SHT_STRTAB    3
#define SHT_RELA      4
#define SHT_HASH      5
#define SHT_DYNAMIC   6
#define SHT_NOTE      7
#define SHT_NOBITS    8
#define SHT_REL       9
#define SHT_SHLIB    10
#define SHT_DYNSYM   11
#define SHT_LOPROC   0x70000000
#define SHT_HIPROC   0x7fffffff
#define SHT_LOUSER   0x80000000
#define SHT_HIUSER   0xffffffff

/* sh_flags */
#define SHF_WRITE      1
#define SHF_ALLOC      2
#define SHF_EXECINSTR  4
#define SHF_MASKPROC   0xf0000000

/* SEG TYPES */
#define PROGRAM_SIZE  32
#define PT_NULL        0
#define PT_LOAD        1
#define PT_DYNAMIC     2
#define PT_INTERP      3
#define PT_NOTE        4
#define PT_SHLIB       5
#define PT_PHDR        6
#define PT_LOPROC     0x70000000
#define PT_HIPROC     0x7fffffff

#define PF_R  4
#define PF_W  2
#define PF_X  1

struct elf_prog_header {
  u32 p_type;
  u32 p_offset;
  u32 p_vaddr;
  u32 p_paddr;
  u32 p_filesz;
  u32 p_memsz;
  u32 p_flags;
  u32 p_align;
};

struct elf_file {
  u8 *elfname, *file, *text;
  struct elf_header hdr;
  struct elf_section_header **section, *strtable;
  struct elf_prog_header **program;
  u32 error;
};

/* DYNAMIC */

#define DT_NULL       0
#define DT_NEEDED     1
#define DT_PLTRELSZ   2
#define DT_PLTGOT     3
#define DT_HASH       4
#define DT_STRTAB     5
#define DT_SYMTAB     6
#define DT_RELA       7
#define DT_RELASZ     8
#define DT_RELAENT    9
#define DT_STRSZ     10
#define DT_SYMENT    11
#define DT_INIT      12
#define DT_FINI      13
#define DT_SONAME    14
#define DT_RPATH     15
#define DT_SYMBOLIC  16
#define DT_REL       17
#define DT_RELSZ     18
#define DT_RELENT    19
#define DT_PLTREL    20
#define DT_DEBUG     21
#define DT_TEXTREL   22
#define DT_JMPREL    23
#define DT_BIND_NOW  24
#define DT_LOPROC    0x70000000
#define DT_HIPROC    0x7fffffff

#define SIZE_DYNAMIC  8

struct elf_dynamic {
  u32 d_tag;
  union {
    u32 d_val;
    u32 d_ptr;
  } d_un;
};

#endif /* LOADELF_H */