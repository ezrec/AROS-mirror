/*
 * This file contains some things extracted from binutils package.
 * It is not a bfd implementation in any way, it is just enough for
 * libopcodes to build and work.
 */

#ifndef __BFD_H_SEEN__
#define __BFD_H_SEEN__

#include <exec/types.h>

#ifdef __WORDSIZE
#define BFD_DEFAULT_TARGET_SIZE __WORDSIZE
#else
#define BFD_DEFAULT_TARGET_SIZE 32
#endif

typedef IPTR bfd_vma;
typedef SIPTR bfd_signed_vma;
typedef IPTR symvalue;
typedef unsigned int flagword;
typedef unsigned char bfd_byte;
typedef int bfd_boolean;

#define STRING_COMMA_LEN(STR) (STR), (sizeof (STR) - 1)
#define CONST_STRNEQ(STR1,STR2) (strncmp ((STR1), (STR2), sizeof (STR2) - 1) == 0)
#define LITMEMCPY(DEST,STR2) memcpy ((DEST), (STR2), sizeof (STR2) - 1)
#define LITSTRCPY(DEST,STR2) memcpy ((DEST), (STR2), sizeof (STR2))
#define sprintf_vma(s, x) sprintf(s, "%p", (void *)x)

void __attribute((noreturn)) _abort(char *file, unsigned int line);

#define abort() _abort(__FILE__, __LINE__)
#define fprintf(stream, fmt, ...) {void *s = stream; void *f = fmt; s = s; f = f;}

typedef struct bfd_symbol
{
  /* A pointer to the BFD which owns the symbol. This information
     is necessary so that a back end can work out what additional
     information (invisible to the application writer) is carried
     with the symbol.

     This field is *almost* redundant, since you can use section->owner
     instead, except that some symbols point to the global sections
     bfd_{abs,com,und}_section.  This could be fixed by making
     these globals be per-bfd (or per-target-flavor).  FIXME.  */
  struct bfd *the_bfd; /* Use bfd_asymbol_bfd(sym) to access this field.  */

  /* The text of the symbol. The name is left alone, and not copied; the
     application may not alter it.  */
  const char *name;

  /* The value of the symbol.  This really should be a union of a
     numeric value with a pointer, since some flags indicate that
     a pointer to another symbol is stored here.  */
  symvalue value;

  /* Attributes of a symbol.  */
#define BSF_NO_FLAGS    0x00

  /* The symbol has local scope; <<static>> in <<C>>. The value
     is the offset into the section of the data.  */
#define BSF_LOCAL      0x01

  /* The symbol has global scope; initialized data in <<C>>. The
     value is the offset into the section of the data.  */
#define BSF_GLOBAL     0x02

  /* The symbol has global scope and is exported. The value is
     the offset into the section of the data.  */
#define BSF_EXPORT     BSF_GLOBAL /* No real difference.  */

  /* A normal C symbol would be one of:
     <<BSF_LOCAL>>, <<BSF_FORT_COMM>>,  <<BSF_UNDEFINED>> or
     <<BSF_GLOBAL>>.  */

  /* The symbol is a debugging record. The value has an arbitrary
     meaning, unless BSF_DEBUGGING_RELOC is also set.  */
#define BSF_DEBUGGING  0x08

  /* The symbol denotes a function entry point.  Used in ELF,
     perhaps others someday.  */
#define BSF_FUNCTION    0x10

  /* Used by the linker.  */
#define BSF_KEEP        0x20
#define BSF_KEEP_G      0x40

  /* A weak global symbol, overridable without warnings by
     a regular global symbol of the same name.  */
#define BSF_WEAK        0x80

  /* This symbol was created to point to a section, e.g. ELF's
     STT_SECTION symbols.  */
#define BSF_SECTION_SYM 0x100

  /* The symbol used to be a common symbol, but now it is
     allocated.  */
#define BSF_OLD_COMMON  0x200

  /* The default value for common data.  */
#define BFD_FORT_COMM_DEFAULT_VALUE 0

  /* In some files the type of a symbol sometimes alters its
     location in an output file - ie in coff a <<ISFCN>> symbol
     which is also <<C_EXT>> symbol appears where it was
     declared and not at the end of a section.  This bit is set
     by the target BFD part to convey this information.  */
#define BSF_NOT_AT_END    0x400

  /* Signal that the symbol is the label of constructor section.  */
#define BSF_CONSTRUCTOR   0x800

  /* Signal that the symbol is a warning symbol.  The name is a
     warning.  The name of the next symbol is the one to warn about;
     if a reference is made to a symbol with the same name as the next
     symbol, a warning is issued by the linker.  */
#define BSF_WARNING       0x1000

  /* Signal that the symbol is indirect.  This symbol is an indirect
     pointer to the symbol with the same name as the next symbol.  */
#define BSF_INDIRECT      0x2000

  /* BSF_FILE marks symbols that contain a file name.  This is used
     for ELF STT_FILE symbols.  */
#define BSF_FILE          0x4000

  /* Symbol is from dynamic linking information.  */
#define BSF_DYNAMIC       0x8000

  /* The symbol denotes a data object.  Used in ELF, and perhaps
     others someday.  */
#define BSF_OBJECT        0x10000

  /* This symbol is a debugging symbol.  The value is the offset
     into the section of the data.  BSF_DEBUGGING should be set
     as well.  */
#define BSF_DEBUGGING_RELOC 0x20000

  /* This symbol is thread local.  Used in ELF.  */
#define BSF_THREAD_LOCAL  0x40000

  /* This symbol represents a complex relocation expression,
     with the expression tree serialized in the symbol name.  */
#define BSF_RELC 0x80000

  /* This symbol represents a signed complex relocation expression,
     with the expression tree serialized in the symbol name.  */
#define BSF_SRELC 0x100000

  /* This symbol was created by bfd_get_synthetic_symtab.  */
#define BSF_SYNTHETIC 0x200000

  flagword flags;

  /* A pointer to the section to which this symbol is
     relative.  This will always be non NULL, there are special
     sections for undefined and absolute symbols.  */
  struct bfd_section *section;

  /* Back end special data.  */
  union
    {
      void *p;
      bfd_vma i;
    }
  udata;
}
asymbol;

enum bfd_architecture
{
  bfd_arch_unknown,   /* File arch not known.  */
  bfd_arch_obscure,   /* Arch known, not one of these.  */
  bfd_arch_m68k,      /* Motorola 68xxx */
#define bfd_mach_m68000 1
#define bfd_mach_m68008 2
#define bfd_mach_m68010 3
#define bfd_mach_m68020 4
#define bfd_mach_m68030 5
#define bfd_mach_m68040 6
#define bfd_mach_m68060 7
#define bfd_mach_cpu32  8
#define bfd_mach_fido   9
#define bfd_mach_mcf_isa_a_nodiv 10
#define bfd_mach_mcf_isa_a 11
#define bfd_mach_mcf_isa_a_mac 12
#define bfd_mach_mcf_isa_a_emac 13
#define bfd_mach_mcf_isa_aplus 14
#define bfd_mach_mcf_isa_aplus_mac 15
#define bfd_mach_mcf_isa_aplus_emac 16
#define bfd_mach_mcf_isa_b_nousp 17
#define bfd_mach_mcf_isa_b_nousp_mac 18
#define bfd_mach_mcf_isa_b_nousp_emac 19
#define bfd_mach_mcf_isa_b 20
#define bfd_mach_mcf_isa_b_mac 21
#define bfd_mach_mcf_isa_b_emac 22
#define bfd_mach_mcf_isa_b_float 23
#define bfd_mach_mcf_isa_b_float_mac 24
#define bfd_mach_mcf_isa_b_float_emac 25
#define bfd_mach_mcf_isa_c 26
#define bfd_mach_mcf_isa_c_mac 27
#define bfd_mach_mcf_isa_c_emac 28
#define bfd_mach_mcf_isa_c_nodiv 29
#define bfd_mach_mcf_isa_c_nodiv_mac 30
#define bfd_mach_mcf_isa_c_nodiv_emac 31
  bfd_arch_vax,       /* DEC Vax */
  bfd_arch_i960,      /* Intel 960 */
    /* The order of the following is important.
       lower number indicates a machine type that
       only accepts a subset of the instructions
       available to machines with higher numbers.
       The exception is the "ca", which is
       incompatible with all other machines except
       "core".  */

#define bfd_mach_i960_core      1
#define bfd_mach_i960_ka_sa     2
#define bfd_mach_i960_kb_sb     3
#define bfd_mach_i960_mc        4
#define bfd_mach_i960_xa        5
#define bfd_mach_i960_ca        6
#define bfd_mach_i960_jx        7
#define bfd_mach_i960_hx        8

  bfd_arch_or32,      /* OpenRISC 32 */

  bfd_arch_sparc,     /* SPARC */
#define bfd_mach_sparc                 1
/* The difference between v8plus and v9 is that v9 is a true 64 bit env.  */
#define bfd_mach_sparc_sparclet        2
#define bfd_mach_sparc_sparclite       3
#define bfd_mach_sparc_v8plus          4
#define bfd_mach_sparc_v8plusa         5 /* with ultrasparc add'ns.  */
#define bfd_mach_sparc_sparclite_le    6
#define bfd_mach_sparc_v9              7
#define bfd_mach_sparc_v9a             8 /* with ultrasparc add'ns.  */
#define bfd_mach_sparc_v8plusb         9 /* with cheetah add'ns.  */
#define bfd_mach_sparc_v9b             10 /* with cheetah add'ns.  */
/* Nonzero if MACH has the v9 instruction set.  */
#define bfd_mach_sparc_v9_p(mach) \
  ((mach) >= bfd_mach_sparc_v8plus && (mach) <= bfd_mach_sparc_v9b \
   && (mach) != bfd_mach_sparc_sparclite_le)
/* Nonzero if MACH is a 64 bit sparc architecture.  */
#define bfd_mach_sparc_64bit_p(mach) \
  ((mach) >= bfd_mach_sparc_v9 && (mach) != bfd_mach_sparc_v8plusb)
  bfd_arch_spu,       /* PowerPC SPU */
#define bfd_mach_spu           256 
  bfd_arch_mips,      /* MIPS Rxxxx */
#define bfd_mach_mips3000              3000
#define bfd_mach_mips3900              3900
#define bfd_mach_mips4000              4000
#define bfd_mach_mips4010              4010
#define bfd_mach_mips4100              4100
#define bfd_mach_mips4111              4111
#define bfd_mach_mips4120              4120
#define bfd_mach_mips4300              4300
#define bfd_mach_mips4400              4400
#define bfd_mach_mips4600              4600
#define bfd_mach_mips4650              4650
#define bfd_mach_mips5000              5000
#define bfd_mach_mips5400              5400
#define bfd_mach_mips5500              5500
#define bfd_mach_mips6000              6000
#define bfd_mach_mips7000              7000
#define bfd_mach_mips8000              8000
#define bfd_mach_mips9000              9000
#define bfd_mach_mips10000             10000
#define bfd_mach_mips12000             12000
#define bfd_mach_mips16                16
#define bfd_mach_mips5                 5
#define bfd_mach_mips_loongson_2e      3001
#define bfd_mach_mips_loongson_2f      3002
#define bfd_mach_mips_sb1              12310201 /* octal 'SB', 01 */
#define bfd_mach_mips_octeon           6501
#define bfd_mach_mipsisa32             32
#define bfd_mach_mipsisa32r2           33
#define bfd_mach_mipsisa64             64
#define bfd_mach_mipsisa64r2           65
  bfd_arch_i386,      /* Intel 386 */
#define bfd_mach_i386_i386 1
#define bfd_mach_i386_i8086 2
#define bfd_mach_i386_i386_intel_syntax 3
#define bfd_mach_x86_64 64
#define bfd_mach_x86_64_intel_syntax 65
  bfd_arch_we32k,     /* AT&T WE32xxx */
  bfd_arch_tahoe,     /* CCI/Harris Tahoe */
  bfd_arch_i860,      /* Intel 860 */
  bfd_arch_i370,      /* IBM 360/370 Mainframes */
  bfd_arch_romp,      /* IBM ROMP PC/RT */
  bfd_arch_convex,    /* Convex */
  bfd_arch_m88k,      /* Motorola 88xxx */
  bfd_arch_m98k,      /* Motorola 98xxx */
  bfd_arch_pyramid,   /* Pyramid Technology */
  bfd_arch_h8300,     /* Renesas H8/300 (formerly Hitachi H8/300) */
#define bfd_mach_h8300    1
#define bfd_mach_h8300h   2
#define bfd_mach_h8300s   3
#define bfd_mach_h8300hn  4
#define bfd_mach_h8300sn  5
#define bfd_mach_h8300sx  6
#define bfd_mach_h8300sxn 7
  bfd_arch_pdp11,     /* DEC PDP-11 */
  bfd_arch_powerpc,   /* PowerPC */
#define bfd_mach_ppc           32
#define bfd_mach_ppc64         64
#define bfd_mach_ppc_403       403
#define bfd_mach_ppc_403gc     4030
#define bfd_mach_ppc_505       505
#define bfd_mach_ppc_601       601
#define bfd_mach_ppc_602       602
#define bfd_mach_ppc_603       603
#define bfd_mach_ppc_ec603e    6031
#define bfd_mach_ppc_604       604
#define bfd_mach_ppc_620       620
#define bfd_mach_ppc_630       630
#define bfd_mach_ppc_750       750
#define bfd_mach_ppc_860       860
#define bfd_mach_ppc_a35       35
#define bfd_mach_ppc_rs64ii    642
#define bfd_mach_ppc_rs64iii   643
#define bfd_mach_ppc_7400      7400
#define bfd_mach_ppc_e500      500
#define bfd_mach_ppc_e500mc    5001
  bfd_arch_rs6000,    /* IBM RS/6000 */
#define bfd_mach_rs6k          6000
#define bfd_mach_rs6k_rs1      6001
#define bfd_mach_rs6k_rsc      6003
#define bfd_mach_rs6k_rs2      6002
  bfd_arch_hppa,      /* HP PA RISC */
#define bfd_mach_hppa10        10
#define bfd_mach_hppa11        11
#define bfd_mach_hppa20        20
#define bfd_mach_hppa20w       25
  bfd_arch_d10v,      /* Mitsubishi D10V */
#define bfd_mach_d10v          1
#define bfd_mach_d10v_ts2      2
#define bfd_mach_d10v_ts3      3
  bfd_arch_d30v,      /* Mitsubishi D30V */
  bfd_arch_dlx,       /* DLX */
  bfd_arch_m68hc11,   /* Motorola 68HC11 */
  bfd_arch_m68hc12,   /* Motorola 68HC12 */
#define bfd_mach_m6812_default 0
#define bfd_mach_m6812         1
#define bfd_mach_m6812s        2
  bfd_arch_z8k,       /* Zilog Z8000 */
#define bfd_mach_z8001         1
#define bfd_mach_z8002         2
  bfd_arch_h8500,     /* Renesas H8/500 (formerly Hitachi H8/500) */
  bfd_arch_sh,        /* Renesas / SuperH SH (formerly Hitachi SH) */
#define bfd_mach_sh            1
#define bfd_mach_sh2        0x20
#define bfd_mach_sh_dsp     0x2d
#define bfd_mach_sh2a       0x2a
#define bfd_mach_sh2a_nofpu 0x2b
#define bfd_mach_sh2a_nofpu_or_sh4_nommu_nofpu 0x2a1
#define bfd_mach_sh2a_nofpu_or_sh3_nommu 0x2a2
#define bfd_mach_sh2a_or_sh4  0x2a3
#define bfd_mach_sh2a_or_sh3e 0x2a4
#define bfd_mach_sh2e       0x2e
#define bfd_mach_sh3        0x30
#define bfd_mach_sh3_nommu  0x31
#define bfd_mach_sh3_dsp    0x3d
#define bfd_mach_sh3e       0x3e
#define bfd_mach_sh4        0x40
#define bfd_mach_sh4_nofpu  0x41
#define bfd_mach_sh4_nommu_nofpu  0x42
#define bfd_mach_sh4a       0x4a
#define bfd_mach_sh4a_nofpu 0x4b
#define bfd_mach_sh4al_dsp  0x4d
#define bfd_mach_sh5        0x50
  bfd_arch_alpha,     /* Dec Alpha */
#define bfd_mach_alpha_ev4  0x10
#define bfd_mach_alpha_ev5  0x20
#define bfd_mach_alpha_ev6  0x30
  bfd_arch_arm,       /* Advanced Risc Machines ARM.  */
#define bfd_mach_arm_unknown   0
#define bfd_mach_arm_2         1
#define bfd_mach_arm_2a        2
#define bfd_mach_arm_3         3
#define bfd_mach_arm_3M        4
#define bfd_mach_arm_4         5
#define bfd_mach_arm_4T        6
#define bfd_mach_arm_5         7
#define bfd_mach_arm_5T        8
#define bfd_mach_arm_5TE       9
#define bfd_mach_arm_XScale    10
#define bfd_mach_arm_ep9312    11
#define bfd_mach_arm_iWMMXt    12
#define bfd_mach_arm_iWMMXt2   13
  bfd_arch_ns32k,     /* National Semiconductors ns32000 */
  bfd_arch_w65,       /* WDC 65816 */
  bfd_arch_tic30,     /* Texas Instruments TMS320C30 */
  bfd_arch_tic4x,     /* Texas Instruments TMS320C3X/4X */
#define bfd_mach_tic3x         30
#define bfd_mach_tic4x         40
  bfd_arch_tic54x,    /* Texas Instruments TMS320C54X */
  bfd_arch_tic80,     /* TI TMS320c80 (MVP) */
  bfd_arch_v850,      /* NEC V850 */
#define bfd_mach_v850          1
#define bfd_mach_v850e         'E'
#define bfd_mach_v850e1        '1'
  bfd_arch_arc,       /* ARC Cores */
#define bfd_mach_arc_5         5
#define bfd_mach_arc_6         6
#define bfd_mach_arc_7         7
#define bfd_mach_arc_8         8
 bfd_arch_m32c,     /* Renesas M16C/M32C.  */
#define bfd_mach_m16c        0x75
#define bfd_mach_m32c        0x78
  bfd_arch_m32r,      /* Renesas M32R (formerly Mitsubishi M32R/D) */
#define bfd_mach_m32r          1 /* For backwards compatibility.  */
#define bfd_mach_m32rx         'x'
#define bfd_mach_m32r2         '2'
  bfd_arch_mn10200,   /* Matsushita MN10200 */
  bfd_arch_mn10300,   /* Matsushita MN10300 */
#define bfd_mach_mn10300               300
#define bfd_mach_am33          330
#define bfd_mach_am33_2        332
  bfd_arch_fr30,
#define bfd_mach_fr30          0x46523330
  bfd_arch_frv,
#define bfd_mach_frv           1
#define bfd_mach_frvsimple     2
#define bfd_mach_fr300         300
#define bfd_mach_fr400         400
#define bfd_mach_fr450         450
#define bfd_mach_frvtomcat     499     /* fr500 prototype */
#define bfd_mach_fr500         500
#define bfd_mach_fr550         550
  bfd_arch_mcore,
  bfd_arch_mep,
#define bfd_mach_mep           1
#define bfd_mach_mep_h1        0x6831
  bfd_arch_ia64,      /* HP/Intel ia64 */
#define bfd_mach_ia64_elf64    64
#define bfd_mach_ia64_elf32    32
  bfd_arch_ip2k,      /* Ubicom IP2K microcontrollers. */
#define bfd_mach_ip2022        1
#define bfd_mach_ip2022ext     2
 bfd_arch_iq2000,     /* Vitesse IQ2000.  */
#define bfd_mach_iq2000        1
#define bfd_mach_iq10          2
  bfd_arch_mt,
#define bfd_mach_ms1           1
#define bfd_mach_mrisc2        2
#define bfd_mach_ms2           3
  bfd_arch_pj,
  bfd_arch_avr,       /* Atmel AVR microcontrollers.  */
#define bfd_mach_avr1          1
#define bfd_mach_avr2          2
#define bfd_mach_avr25         25
#define bfd_mach_avr3          3
#define bfd_mach_avr31         31
#define bfd_mach_avr35         35
#define bfd_mach_avr4          4
#define bfd_mach_avr5          5
#define bfd_mach_avr51         51
#define bfd_mach_avr6          6
  bfd_arch_bfin,        /* ADI Blackfin */
#define bfd_mach_bfin          1
  bfd_arch_cr16,       /* National Semiconductor CompactRISC (ie CR16). */
#define bfd_mach_cr16          1
  bfd_arch_cr16c,       /* National Semiconductor CompactRISC. */
#define bfd_mach_cr16c         1
  bfd_arch_crx,       /*  National Semiconductor CRX.  */
#define bfd_mach_crx           1
  bfd_arch_cris,      /* Axis CRIS */
#define bfd_mach_cris_v0_v10   255
#define bfd_mach_cris_v32      32
#define bfd_mach_cris_v10_v32  1032
  bfd_arch_s390,      /* IBM s390 */
#define bfd_mach_s390_31       31
#define bfd_mach_s390_64       64
  bfd_arch_score,     /* Sunplus score */ 
  bfd_arch_openrisc,  /* OpenRISC */
  bfd_arch_mmix,      /* Donald Knuth's educational processor.  */
  bfd_arch_xstormy16,
#define bfd_mach_xstormy16     1
  bfd_arch_msp430,    /* Texas Instruments MSP430 architecture.  */
#define bfd_mach_msp11          11
#define bfd_mach_msp110         110
#define bfd_mach_msp12          12
#define bfd_mach_msp13          13
#define bfd_mach_msp14          14
#define bfd_mach_msp15          15
#define bfd_mach_msp16          16
#define bfd_mach_msp21          21
#define bfd_mach_msp31          31
#define bfd_mach_msp32          32
#define bfd_mach_msp33          33
#define bfd_mach_msp41          41
#define bfd_mach_msp42          42
#define bfd_mach_msp43          43
#define bfd_mach_msp44          44
  bfd_arch_xc16x,     /* Infineon's XC16X Series.               */
#define bfd_mach_xc16x         1
#define bfd_mach_xc16xl        2
#define bfd_mach_xc16xs         3
  bfd_arch_xtensa,    /* Tensilica's Xtensa cores.  */
#define bfd_mach_xtensa        1
   bfd_arch_maxq,     /* Dallas MAXQ 10/20 */
#define bfd_mach_maxq10    10
#define bfd_mach_maxq20    20
  bfd_arch_z80,
#define bfd_mach_z80strict      1 /* No undocumented opcodes.  */
#define bfd_mach_z80            3 /* With ixl, ixh, iyl, and iyh.  */
#define bfd_mach_z80full        7 /* All undocumented instructions.  */
#define bfd_mach_r800           11 /* R800: successor with multiplication.  */
  bfd_arch_last
  };

enum bfd_flavour
{
  bfd_target_unknown_flavour,
  bfd_target_aout_flavour,
  bfd_target_coff_flavour,
  bfd_target_ecoff_flavour,
  bfd_target_xcoff_flavour,
  bfd_target_elf_flavour,
  bfd_target_ieee_flavour,
  bfd_target_nlm_flavour,
  bfd_target_oasys_flavour,
  bfd_target_tekhex_flavour,
  bfd_target_srec_flavour,
  bfd_target_ihex_flavour,
  bfd_target_som_flavour,
  bfd_target_os9k_flavour,
  bfd_target_versados_flavour,
  bfd_target_msdos_flavour,
  bfd_target_ovax_flavour,
  bfd_target_evax_flavour,
  bfd_target_mmo_flavour,
  bfd_target_mach_o_flavour,
  bfd_target_pef_flavour,
  bfd_target_pef_xlib_flavour,
  bfd_target_sym_flavour
};

enum bfd_endian { BFD_ENDIAN_BIG, BFD_ENDIAN_LITTLE, BFD_ENDIAN_UNKNOWN };

/*
 * struct bfd is incomplete. In contains only some generic data enough for most common
 * disassemblers to work. Some exotic disassemblers (ARC and CRIS for example) will need
 * more data.
 */

typedef struct bfd
{
    enum bfd_architecture arch;
    unsigned long mach;
    enum bfd_endian endian;
} bfd;

#define bfd_get_arch(abfd) abfd->arch
#define bfd_get_mach(abfd) abfd->mach
#define bfd_big_endian(abfd) (abfd->endian == BFD_ENDIAN_BIG)

bfd_vma	bfd_getb32(const unsigned char *);
bfd_vma	bfd_getl32(const unsigned char *);

/* Incomplete, ARM is claimed to need this */

typedef struct bfd_section
{
    int dummy;
} asection;

#endif
