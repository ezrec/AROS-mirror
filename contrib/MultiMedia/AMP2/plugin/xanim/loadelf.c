/*
 * loadelf.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __PPC__
  #include <powerpc/powerpc.h>
  #include <powerpc/powerpc_protos.h>
#endif

#include "types.h"
#include "dlfcn.h"
#include "loadelf.h"

#define PRINTF if (1 == 0) printf

static char dt_name[][16] = {
  "DT_NULL",
  "DT_NEEDED",
  "DT_PLTRELSZ",
  "DT_PLTGOT",
  "DT_HASH",
  "DT_STRTAB",
  "DT_SYMTAB",
  "DT_RELA",
  "DT_RELASZ",
  "DT_RELAENT",
  "DT_STRSZ",
  "DT_SYMENT",
  "DT_INIT",
  "DT_FINI",
  "DT_SONAME",
  "DT_RPATH",
  "DT_SYMBOLIC",
  "DT_REL",
  "DT_RELSZ",
  "DT_RELENT",
  "DT_PLTREL",
  "DT_DEBUG",
  "DT_TEXTREL",
  "DT_JMPREL",
  "DT_BIND_NOW"
};

static char pt_name[][16] = {
  "PT_NULL",
  "PT_LOAD",
  "PT_DYNAMIC",
  "PT_INTERP",
  "PT_NOTE",
  "PT_SHLIB",
  "PT_PHDR"
};

static char r_386[][16] = {
  "R_386_NONE",
  "R_386_32",
  "R_386_PC32",
  "R_386_GOT32",
  "R_386_PLT32",
  "R_386_COPY",
  "R_386_GLOB_DAT",
  "R_386_JMP_SLOT",
  "R_386_RELATIVE",
  "R_386_GOTOFF",
  "R_386_GOTPC"
};

static char r_powerpc[][24] = {
  "R_PPC_NONE",
  "R_PPC_ADDR32",
  "R_PPC_ADDR24",
  "R_PPC_ADDR16",
  "R_PPC_ADDR16_LO",
  "R_PPC_ADDR16_HI",
  "R_PPC_ADDR16_HA",
  "R_PPC_ADDR14",
  "R_PPC_ADDR14_BRTAKEN",
  "R_PPC_ADDR14_BRNTAKEN",
  "R_PPC_REL24",
  "R_PPC_REL14",
  "R_PPC_REL14_BRTAKEN",
  "R_PPC_REL14_BRNTAKEN",
  "R_PPC_GOT16",
  "R_PPC_GOT16_LO",
  "R_PPC_GOT16_HI",
  "R_PPC_GOT16_HA",
  "R_PPC_PLTREL24",
  "R_PPC_COPY",
  "R_PPC_GLOB_DAT",
  "R_PPC_JMP_SLOT",
  "R_PPC_RELATIVE",
  "R_PPC_LOCAL24PC",
  "R_PPC_UADDR32",
  "R_PPC_UADDR16",
  "R_PPC_REL32",
  "R_PPC_PLT32",
  "R_PPC_PLTREL32",
  "R_PPC_PLT16_LO",
  "R_PPC_PLT16_HI",
  "R_PPC_PLT16_HA",
  "R_PPC_SDAREL16",
  "R_PPC_SECTOFF",
  "R_PPC_SECTOFF_LO",
  "R_PPC_SECTOFF_HI",
  "R_PPC_SECTOFF_HA"
};

static void *find_reloc(struct elf_file *elf, struct elf_symbol *symbol, u8 *strtab)
{
  reloc_t *r = reloc;
  u8 *name;

  /* local symbol */
  name = strtab + symbol->st_name;
  if (symbol->st_addr) {
    PRINTF("find_reloc local '%s' @ %08lx (%ld)\n", name, (u32)elf->file + symbol->st_addr, (u32)elf->file + symbol->st_addr);
    return elf->file + symbol->st_addr;
  }

  /* global symbol */
  name = strtab + symbol->st_name;
  while (r->name != NULL) {
    if (!strcmp(r->name, name)) {
      PRINTF("find_reloc global '%s' @ %08lx (%ld)\n", name, (u32)r->addr, (u32)r->addr);
      return r->addr;
    }
    r++;
  }

  printf("find_reloc: '%s' not found\n", name);
  elf->error = 1;

  return NULL;
}

int elf_load(u8 *name, struct elf_file *elf)
{
  u32 size, i, j, n;
  FILE *fp = NULL;
  u8 *ptr;

  memset(elf, 0, sizeof(*elf));
  elf->elfname = strdup(name);

  fp = fopen(name, "rb");
  if (fp == NULL) {
    printf("file %s not found\n", name);
    return -1;
  }

  fseek(fp, 0, SEEK_END);
  size = ftell(fp);

PRINTF("filesize: %ld\n", size);

  fseek(fp, 0, SEEK_SET);

  elf->file = malloc(size);
  fread(elf->file, 1, size, fp);
  fclose(fp);

  memcpy(&elf->hdr, elf->file, sizeof(elf->hdr));

  PRINTF("elf type: %d, machine: %d\n", elf->hdr.e_type, elf->hdr.e_machine);

  /* allocate program memory */
  if (elf->hdr.e_phnum) {
    elf->program = malloc(elf->hdr.e_phnum * sizeof(*elf->program));
    for (i=0; i<elf->hdr.e_phnum; i++) {
      struct elf_prog_header *program;

      /* position it to its section position */
      ptr = elf->file + elf->hdr.e_phoff + (i * elf->hdr.e_phentsize);

      /* load each program entry */
      elf->program[i] = program = malloc(sizeof(*program));
      memset(program, 0, sizeof(*program));

      memcpy(program, ptr, elf->hdr.e_phentsize);

PRINTF("program %2ld, type: %ld (%s), filesz: %ld, vaddr: %ld, memsz: %ld\n", i, program->p_type, pt_name[program->p_type], program->p_filesz, program->p_vaddr, program->p_memsz);

      /* calculate memsize */
      if (program->p_type == PT_LOAD) {
        size = program->p_vaddr + program->p_memsz;
      }
    }
  }

PRINTF("memsize: %ld\n", size);

  /* load file to memory */
  elf->text = malloc(size);
  memset(elf->text, 0, size);
  for (i=0; i<elf->hdr.e_phnum; i++) {
    struct elf_prog_header *program = elf->program[i];
    if (program->p_type == PT_LOAD) {
      memcpy(elf->text + program->p_vaddr, elf->file + program->p_offset, program->p_filesz);
    }
  }

  /* allocate section memory */
  elf->section = malloc(elf->hdr.e_shnum * sizeof(*elf->section));
  for (i=0; i<elf->hdr.e_shnum; i++) {
    struct elf_section_header *section;

    /* position it to its section position */
    ptr = elf->file + elf->hdr.e_shoff + (i * elf->hdr.e_shentsize);

    /* load each session entry */
    elf->section[i] = section = malloc(sizeof(*section));
    memset(section, 0, sizeof(*section));

    memcpy(section, ptr, elf->hdr.e_shentsize);

    if (section->sh_type != SHT_NOBITS) {
      ptr = elf->file + section->sh_offset;
      section->data = (u8 *)malloc(section->sh_size);
      memset(section->data, 0, section->sh_size);
      memcpy(section->data, ptr, section->sh_size);
    } else {
      /* FIXME */
      section->data = (u8 *)malloc(section->sh_size);
      memset(section->data, 0, section->sh_size);
    }
  }

  /* free file and point file to loaded program */
  free(elf->file);
  elf->file = elf->text; /* FIXME */

  /* FIXUP section names */
  elf->strtable = elf->section[elf->hdr.e_shstrndx];
  for (i=0; i<elf->hdr.e_shnum; i++) {
    struct elf_section_header *section;

    section = elf->section[i];
    if (section == NULL) {
      continue;
    }

    section->name = strdup(&elf->strtable->data[section->sh_name]);
    if (section->sh_link) {
      section->link = elf->section[section->sh_link];
    }
    if (section->sh_info) {
      section->info = elf->section[section->sh_info];
    }

PRINTF("section %2ld, type: %2ld (%08lx), name: %s\n", i, section->sh_type, section->sh_type, section->name);

    if (section->sh_flags & SHF_ALLOC) {
PRINTF("alloc: %ld\n", section->sh_size);
    }

    switch (section->sh_type) {
      case SHT_REL: { /* relocations */
        n = section->sh_size / SIZE_REL;
        section->relocation_table = (struct elf_relocation **)malloc(n * sizeof(struct elf_relocation *));
        section->relocation_count = n;
        for (j=0; j<n; j++) {
          section->relocation_table[j] = (struct elf_relocation *)&section->data[j * SIZE_REL];
        }
      }
      break; /* relocations */

/* "A" */

      case SHT_RELA: { /* relocations */
        n = section->sh_size / SIZE_RELA;
        section->relocation_table = (struct elf_relocation **)malloc(n * sizeof(struct elf_relocation *));
        section->relocation_count = n;
        for (j=0; j<n; j++) {
          section->relocation_table[j] = (struct elf_relocation *)&section->data[j * SIZE_RELA];
        }
      }
      break; /* relocations */

      case SHT_DYNSYM:
      case SHT_SYMTAB: { /* symbols */
        n = section->sh_size / SIZE_SYMBOL;
        section->symbol_table = (struct elf_symbol **)malloc(n * sizeof(struct elf_symbol *));
        section->symbol_count = n;
        for (j=0; j<n; j++) {
          section->symbol_table[j] = (struct elf_symbol *)&section->data[j * SIZE_SYMBOL];
        }
      }
      break; /* symbols */

      case SHT_DYNAMIC: { /* dynamic */
        struct elf_symbol *symbol;
        struct elf_relocation *relocation;
        u32 relsz = 0, relent = 0, rel = 0;
        u8 *symtab = NULL, *strtab = NULL, *target = NULL;
        struct elf_dynamic *dynamic;

        u32 pltrel = 0, pltrelsz = 0, jmprel = 0; /* PLT table */

        dynamic = (struct elf_dynamic *)section->data;
        n = section->sh_size / SIZE_DYNAMIC;

PRINTF("dynamic size: %ld @ %ld\n", n, section->sh_offset);

        for (j=0; j<n; j++) {

PRINTF("%ld (%s) : %ld\n", dynamic[j].d_tag, (dynamic[j].d_tag > 24) ? "UNKNOWN" : dt_name[dynamic[j].d_tag], dynamic[j].d_un.d_val);

          switch (dynamic[j].d_tag) {
            case DT_RELSZ:
              relsz = dynamic[j].d_un.d_val;
              break;

            case DT_RELENT:
              relent = dynamic[j].d_un.d_val;
              break;

            case DT_REL:
              rel = dynamic[j].d_un.d_ptr;
              break;

            case DT_SYMTAB:
              symtab = elf->file + dynamic[j].d_un.d_ptr;
              break;

            case DT_STRTAB:
              strtab = elf->file + dynamic[j].d_un.d_ptr;
              break;

/* PLT table */

            case DT_PLTREL:
              pltrel = dynamic[j].d_un.d_val;
              break;

            case DT_JMPREL:
              jmprel = dynamic[j].d_un.d_ptr;
              break;

            case DT_PLTRELSZ:
              pltrelsz = dynamic[j].d_un.d_val;
              break;

/* "A" */ // What's the difference between RELxxx and RELAxxx ???

            case DT_RELASZ:
              relsz = dynamic[j].d_un.d_val;
              break;

            case DT_RELAENT:
              relent = dynamic[j].d_un.d_val;
              break;

            case DT_RELA:
              rel = dynamic[j].d_un.d_ptr;
              break;
          }
        }

// FIXME: use relent maybe???

#undef SIZE_REL
#define SIZE_REL SIZE_RELA

PRINTF("REL %ld %ld %ld -> %ld @ %d\n", relsz, relent, rel, relsz / SIZE_REL, SIZE_REL);
PRINTF("PLT %ld %ld %ld\n", pltrelsz, pltrel, jmprel);

        n = (relsz / SIZE_REL) + (pltrelsz / SIZE_REL);
        section->relocation_table = (struct elf_relocation **)malloc(n * sizeof(struct elf_relocation *));
        section->relocation_count = n;

        n = 0;
        ptr = elf->file + rel;
        for (j=0; j<(relsz / SIZE_REL); j++) {
          section->relocation_table[n] = (struct elf_relocation *)&ptr[j * SIZE_REL];
          n++;
        }

        ptr = elf->file + jmprel;
        for (j=0; j<(pltrelsz / SIZE_REL); j++) {
          section->relocation_table[n] = (struct elf_relocation *)&ptr[j * SIZE_REL];
          n++;
        }

        for (j=0; j<section->relocation_count; j++) {
          relocation = section->relocation_table[j];

          symbol = &((struct elf_symbol *)symtab)[relocation->r_sym];

//PRINTF("reloc: %08lx, %d (%08lx, %08lx)\n", relocation->r_offset, relocation->r_type, ((unsigned long *)relocation)[1], ((unsigned long *)relocation)[2]);

#if 1

PRINTF("reloc: %08lx, %d (%s), %ld, symbol: %d, %ld (%s) (addr: %08lx)\n",
relocation->r_offset,
relocation->r_type,

#ifdef __PPC__
r_powerpc[relocation->r_type],
#else
r_386[relocation->r_type],
#endif

relocation->r_addend,
symbol->st_type, symbol->st_name, strtab + symbol->st_name, symbol->st_addr);

#endif

          target = elf->file + relocation->r_offset; // Is this correct ???

PRINTF("taget: %08lx, offset: %ld\n", *(u32 *)target, relocation->r_offset);

#ifdef __PPC__ // PowerPC

          switch(relocation->r_type) {

// IV41

            case R_PPC_ADDR32: {
              u32 relative = *(u32 *)target;
              u32 *addr = find_reloc(elf, symbol, strtab);
              *(u32 *)target = (u32)addr + relocation->r_addend;
PRINTF("ADDR32 relative: %08lx, new: %08lx\n", relative, *(u32 *)target);
              }
              break;

            case R_PPC_RELATIVE: { // FIXME
              u32 destination = (u32)elf->file + relocation->r_addend; // Is this correct???
              *(u32 *)target = destination; // Is this correct???
PRINTF("RELATIVE target: %08lx, new: %08lx\n", (u32)target, destination);
              }
              break;

// IV32

            case R_PPC_REL24: {
              u32 relative = *(u32 *)target;
              u32 *addr = find_reloc(elf, symbol, strtab);
              u32 temp;
              temp = (u32)addr - (u32)target + relocation->r_addend; // Is this correct???
              *(u32 *)target = ((*(u32 *)target) & 0xfc000003) | (temp & 0x03fffffc);
PRINTF("REL24 relative: %08lx (%08lx), target: %08lx, new: %08lx\n", temp, relative, (u32)addr, *(u32 *)target);
              }
              break;

            case R_PPC_ADDR16_LO: {
              u32 destination;
              if (relocation->r_addend > 0) {
                destination = (u32)elf->file + relocation->r_addend; // Is this correct???
              } else {
                destination = (u32)find_reloc(elf, symbol, strtab);
              }
PRINTF("ADDR16_LO target: %08lx (%04x), new: %08lx vs %08lx\n", (u32)target, *(u16 *)target, destination, (u32)elf->file + relocation->r_addend);
              *(u16 *)target = (destination & 0xffff);
              }
              break;

            case R_PPC_ADDR16_HA: {
              u32 destination;
              if (relocation->r_addend > 0) {
                destination = (u32)elf->file + relocation->r_addend; // Is this correct???
              } else {
                destination = (u32)find_reloc(elf, symbol, strtab);
              }
PRINTF("ADDR16_HA target: %08lx (%04x), new: %08lx vs %08lx\n", (u32)target, *(u16 *)target, destination, (u32)elf->file + relocation->r_addend);
              *(u16 *)target = ((destination + 0x8000) >> 16);
              }
              break;

// RM

            #define b(offset) 0x48000000 | (offset & 0x03fffffc)

            case R_PPC_JMP_SLOT: {
              u32 relative = *(u32 *)target;
              u32 *addr = find_reloc(elf, symbol, strtab);
              u32 temp;
              temp = (u32)addr - (u32)target + relocation->r_addend; // Is this correct???
              *(u32 *)target = b(temp);
PRINTF("JMP_SLOT relative: %08lx (%08lx), target: %08lx, new: %08lx\n", temp, relative, (u32)addr, *(u32 *)target);
              }
              break;

            case R_PPC_NONE: { /* none */
              }
              break;

            default: { /* unknown */
              printf("unknown relocation type: %d\n", relocation->r_type);
              exit(0);
              }
              break;

          }

#else // x86

          switch(relocation->r_type) {
            case R_386_RELATIVE: { /* word32 B + A */
              u32 relative = *(u32 *)target;
              *(u32 *)target = (u32)elf->file + relative;
PRINTF("RELATIVE relative: %08lx, new: %08lx\n", relative, *(u32 *)target);
              }
              break;

            case R_386_PC32: { /* word32 S+A-P */
              u32 relative = *(u32 *)target;
              u32 *addr = find_reloc(elf, symbol, strtab);
              *(u32 *)target = (u32)addr - (u32)target + relative; // Is this correct???
PRINTF("PC32 relative: %08lx (%ld), target: %08lx, new: %08lx\n", relative, relative, (u32)addr, *(u32 *)target);
              }
              break;

            case R_386_32: { /* word32 S+A */
              u32 relative = *(u32 *)target;
              u32 *addr = find_reloc(elf, symbol, strtab);
              *(u32 *)target = (u32)addr + relative; // Is this correct???
PRINTF("32 relative: %08lx, target: %08lx, new: %08lx\n", relative, (u32)addr, *(u32 *)target);
              }
              break;

            case R_386_GLOB_DAT: { /* word32 S */
              u32 relative = *(u32 *)target; // only for print below...
              u32 *addr = find_reloc(elf, symbol, strtab);
              *(u32 *)target = (u32)addr; // Is this correct???
PRINTF("GLOB_DAT relative: %08lx, target: %08lx, new: %08lx\n", relative, (u32)addr, *(u32 *)target);
              }
              break;

            case R_386_JMP_SLOT: { /* word32 S */
              u32 relative = *(u32 *)target; // only for print below...
              u32 *addr = find_reloc(elf, symbol, strtab);
              *(u32 *)target = (u32)addr; // Is this correct???
PRINTF("JMP_SLOT relative: %08lx, target: %08lx, new: %08lx\n", relative, (u32)addr, *(u32 *)target);
              }
              break;

            case R_386_NONE: { /* none */
              }
              break;

            default: { /* unknown */
              printf("unknown relocation type: %d\n", relocation->r_type);
              exit(0);
              }
              break;

/*
R_386_NONE 0 none none
R_386_32 1 word32 S+A
R_386_PC32 2 word32 S+A-P
R_386_GOT32 3 word32 G + A
R_386_PLT32 4 word32 L + A - P
R_386_COPY 5 none none
R_386_GLOB_DAT 6 word32 S
R_386_JMP_SLOT 7 word32 S
R_386_RELATIVE 8 word32 B + A
R_386_GOTOFF 9 word32 S + A - GOT
R_386_GOTPC 10 word32 GOT + A - P

A This means the addend used to compute the value of the relocatable field.
B This means the base address at which a shared object has been loaded into memory
during execution. Generally, a shared object file is built with a 0 base virtual address,
but the execution address will be different.
G This means the offset into the global offset table at which the address of the
relocation entry's symbol will reside during execution. See "Global Offset Table''
below for more information.
GOT This means the address of the global offset table. See "Global Offset Table'' below
for more information.
L This means the place (section offset or address) of the procedure linkage table entry
for a symbol. A procedure linkage table entry redirects a function call to the proper
destination. The link editor builds the initial procedure linkage table, and the
dynamic linker modifies the entries during execution. See "Procedure Linkage
Table'' below for more information.
P This means the place (section offset or address) of the storage unit being relocated
(computed using r_offset ).
S This means the value of the symbol whose index resides in the relocation entry.
*/
          }

#endif

        }
      }
      break; /* dynamic */
    }
  }

  PRINTF("elf @ %08lx\n", (u32)elf->file);

#ifdef __PPC__
  /* Flush the cache */
  SetCache(CACHE_ICACHEINV, 0, 0);
  SetCache(CACHE_DCACHEFLUSH, 0, 0);
#endif

  if (elf->error) {
    return -1;
  }

  return 0;
}

#if 0
struct elf_section_header *new_section( int type, u8 *name)
{
   struct elf_section_header  *section;
   
   section = malloc(sizeof (*section));
   memset(section, 0, sizeof(*section));
   section->sh_type = type;
   section->name = name;

   return section;
}

void
elf_deletesection( struct elf_section_header *section)
{

   if (section->name)
      free(section->name);
      
   if (section->data)
      free(section->data);
      
   if (section->symbol_table)
      free(section->symbol_table);

   if (section->relocation_table)
      free(section->relocation_table);
      
   free(section);
}
#endif

struct elf_symbol
*elf_findsymbol( struct elf_file *elf, u8 *symbol_name)
{
   int                   i;
   struct elf_section_header  *sh_symbol, *sh_strsymbol = NULL;
   struct elf_symbol          *symbol;

if (symbol_name != NULL) PRINTF("find symbol: %s\n", symbol_name);

sh_symbol = NULL;

   for (i = 1; i < elf->hdr.e_shnum; i++)
//   if  (elf->section[i] && elf->section[i]->sh_type == SHT_SYMTAB)
   if  (elf->section[i] && (elf->section[i]->sh_type == SHT_SYMTAB || elf->section[i]->sh_type == SHT_DYNSYM))
      {
      sh_symbol = elf->section[i];
      sh_strsymbol = sh_symbol->link;
      }

if (sh_symbol == NULL) { PRINTF("no symbols\n"); return NULL; } else { PRINTF("%d symbols\n", sh_symbol->symbol_count); }

   /* now, find out symbol name */
   for (i = 0; i < sh_symbol->symbol_count; i++)
      {
      u8            *name;
      symbol = sh_symbol->symbol_table[i];

      if (symbol)
         {
/*
PRINTF("symbol type: %d\n", symbol->st_type);
*/

         switch(symbol->st_type)
            {
            case STT_SECTION:
                          /* FIXME: added due to bug elsewhere, remove */
                          if (symbol->st_shndx >= elf->hdr.e_shnum) {
                            PRINTF("st_shndx too high : %d vs %d\n", symbol->st_shndx, elf->hdr.e_shnum);
                            continue;
                          }

                      name = elf->section[symbol->st_shndx]->name;
                          break;
            case STT_NOTYPE:
            case STT_OBJECT:
            case STT_FUNC:
                      name = SECTION_STR(sh_strsymbol, symbol->st_name);
                          break;
            default:       name = "";
            }

         if (symbol_name == NULL) { /* DUMP */
           if (*name) { PRINTF("symbol address: %08lx, name: %s\n", symbol->st_addr, name); }
         } else

         if (*name && !strcmp(name, symbol_name))
            return symbol;
         }
      }
   return NULL;
}

#if 0
int
elf_setsymbol_addr( struct elf_file *elf, u8 *symbol_name, u32 addr)
{
   struct elf_symbol     *symbol;

   symbol = elf_findsymbol( elf, symbol_name);
   if (!symbol)
      return -1;
   symbol->st_addr = addr;
   return 0;
}
#endif

u32
elf_getsymbol_addr( struct elf_file *elf, u8 *symbol_name)
{
   struct elf_symbol     *symbol;

   symbol = elf_findsymbol( elf, symbol_name);
   if (!symbol)
      return 0;
   else
      return symbol->st_addr;
}

#if 0
u8
*elf_getsymbol_name( struct elf_file *elf, 
     struct elf_section_header *symtab, int pos)
{
   struct elf_section_header  *sym_str;
   struct elf_symbol          **syms, *symbol;
   int                   s_count; //, n;
   u8                    *name;
   
   /* get syms */
   syms    = symtab->symbol_table;
   s_count = symtab->symbol_count;
   sym_str = symtab->link;
   
   if (pos > s_count)
      return (u8 *)"";

   symbol = syms[ pos];
   switch (symbol->st_type)
            {
            case STT_SECTION:
                      name = elf->section[ symbol->st_shndx]->name;
                          break;
            case STT_NOTYPE:
            case STT_OBJECT:
            case STT_FUNC:
                      name = SECTION_STR( sym_str, symbol->st_name);
                          break;
            default:       name = "";
            }
   return name;
}
#endif

/*
_end, _etext, or _edata Identifier

Purpose
  Define the first addresses following the program, initialized data, and all data.

Syntax
  extern _end;
  extern _etext;
  extern _edata;
Description
  The external names _end, _etext, and _edata are defined by the loader for all programs.
  They are not subroutines but identifiers associated with the following addresses:

    _etext  The first address following the program text.   
    _edata  The first address following the initialized data region.  
    _end    The first address following the data region that is not initialized.
            The name end (with no underscore) defines the same address as does _end (with underscore).   


__bss_start  start of bss, cleared by crt0

// Zero bss
for (dst = __bss_start; dst< _end; dst++) *dst = 0;


*/

/* DLFCN WRAPPER */

void *dlopen(const char *file, int mode)
{
  struct elf_file *elf;

  PRINTF("dlopen: '%s' with mode %d\n", file, mode);

  elf = malloc(sizeof(*elf));
  memset(elf, 0, sizeof(*elf));

  if (elf_load((u8 *)file, elf) < 0) {
    printf("invalid elf file\n");
    free(elf);
    return NULL;
  } else {
    /* HACK to show all symbols */
    elf_findsymbol(elf, NULL);

    /* FIXME: Call .init */

    return elf;
  }
}

void *dlsym(void *handle, const char *name)
{
  struct elf_file *elf = (struct elf_file *)handle;
  u32 addr = elf_getsymbol_addr(elf, (u8 *)name);

  if (addr) {
    PRINTF("dlsym: '%s' @ %08lx (offset %08lx)\n", name, (u32)elf->file + addr, addr);
    return (elf->file + addr);
  } else {
    PRINTF("dlsym: '%s' not found\n", name);
    return NULL;
  }
}

int dlclose(void *handle)
{
  struct elf_file *elf = (struct elf_file *)handle;

  if (elf) {
    /* FIXME: Call .fini */
    /* FIXME: Memory leak */
    free(elf);
    return 0;
  } else {
    return -1;
  }
}

char *dlerror(void)
{
  return ""; /* FIXME */
}