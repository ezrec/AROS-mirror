#include <stdio.h>

#include <clib/macros.h>
#include <exec/memory.h>
#include <exec/types.h>
#include <dos/dos.h>
#include <dos/exall.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <libraries/locale.h>
#include <utility/tagitem.h>
#include <dos/dos.h>

#define DEBUG 0
#include <aros/debug.h>


#include "debug.h"
#include "m68k.h"
#include "conditioncodes.h"
#include "addressingmodes.h"

#define ARG_FILE 	0
#define ARG_DUMPCPU	1
#define ARG_STEP	2
#define ARG_NUM  	3

IPTR args[ARG_NUM] = { 0, 0};

char * oplength[] = {".b",".w",".l"};


#define TEMPLATE "FILE/A,DUMP_CPU/S,STEP/S"



#define STACKSIZE 10000

#define SHOW_CPU kprintf


BOOL end = FALSE;

struct M68k CPU;

extern struct Libbases Libbasetable[];





/* ADDI, ORI, EORI, ANDI */
void Group0(UWORD inst)
{
  ULONG val;
  ULONG mask;
  UBYTE reg_field, n;
  ULONG v8,v16,v32;

  switch ((inst >> 6) & 0x3f)
  {
    case 0x00: /* ORI.b #x,<ea>*/
SHOW_OPCODE("ORI.b #%d",SWAP_WORD(*(UWORD *)(CPU.PC+2)));
      if ((inst & 0x3f) == 0x3c)
      {
        /* ORI.b #x,SR */
        CPU.SR |= (UBYTE)SWAP_WORD(*(UWORD *)(CPU.PC+2));
        CPU.lastop = OP_UPTODATE;
SHOW_OPCODE(",SR\n");
      }
      else
      {
        CPU.lastresult  = read_addressing_mode_nochange_offset((inst >> 3) & 0x07, inst & 0x07, 0);
        CPU.lastresult |= SWAP_WORD(*(UWORD *)CPU.PC+2);
        write_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 0);
        CPU.lastop = OP_OR_BYTE;
      }
      CPU.instsize += 2;
SHOW_OPCODE("\n");
      return;
    break;

    case 0x01: /* ORI.w #x,<ea>*/
SHOW_OPCODE("ORI.w #%d",SWAP_WORD(*(UWORD *)(CPU.PC+2)));
      CPU.lastresult = read_addressing_mode_nochange_offset((inst >> 3) & 0x07, inst & 0x07, 1);
      CPU.instsize   += 2;
      CPU.lastresult |= SWAP_WORD(*(UWORD *)(CPU.PC+2));
      write_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 1);
      CPU.lastop = OP_OR_WORD;
SHOW_OPCODE("\n");
      return;
    break;

    case 0x02: /* ORI.l #x,<ea>*/
SHOW_OPCODE("ORI.l #%d",SWAP(*(ULONG *)(CPU.PC+2)));
      CPU.lastresult = read_addressing_mode_nochange_offset((inst >> 3) & 0x07, inst & 0x07, 2);
      CPU.instsize   += 4;
      CPU.lastresult |= SWAP(*(ULONG *)(CPU.PC+2));
      write_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 2);
      CPU.lastop = OP_OR_LONG;
SHOW_OPCODE("\n");
      return;
    break;

    /* case 0x04,0x05,0x06,0x07 see further below ! */

    case 0x08: /* ANDI.b #x,<ea>*/
SHOW_OPCODE("ANDI.b #%d",SWAP_WORD(*(WORD *)(CPU.PC+2)));
      if ((inst & 0x3f) == 0x3c)
      {
        /* ANDI.b #x,SR */
        CPU.SR &= (UBYTE)SWAP_WORD(*(UWORD *)(CPU.PC+2));
        CPU.lastop = OP_UPTODATE;
SHOW_OPCODE(",SR\n");
      }
      else
      {
        CPU.lastresult = read_addressing_mode_nochange_offset((inst >> 3) & 0x07, inst & 0x07, 0);
        CPU.lastresult &= (UBYTE)SWAP_WORD(*(UWORD *)(CPU.PC+2));
        write_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 0);
        CPU.lastop = OP_AND_BYTE;
      }
      CPU.instsize += 2;
SHOW_OPCODE("\n");
      return;
    break;

    case 0x09: /* ANDI.w #x,<ea>*/
SHOW_OPCODE("ANDI.w #%d",SWAP_WORD(*(UWORD *)(CPU.PC+2)));
      CPU.lastresult = read_addressing_mode_nochange_offset((inst >> 3) & 0x07, inst & 0x07,1);
      CPU.instsize += 2;
      CPU.lastresult &= SWAP_WORD(*(UWORD *)(CPU.PC+2));
      write_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 1);
      CPU.lastop = OP_AND_WORD;
SHOW_OPCODE("\n");
      return;
    break;

    case 0x0a: /* ANDI.l #x,<ea>*/
SHOW_OPCODE("ANDI.l #%d",*(UBYTE *)((CPU.PC+2)));
      CPU.lastresult = read_addressing_mode_nochange_offset((inst >> 3) & 0x07, inst & 0x07, 2);
      CPU.instsize  += 4;
      CPU.lastresult &= SWAP(*(ULONG *)(CPU.PC+2));
      write_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 2);
      CPU.lastop = OP_AND_LONG;
SHOW_OPCODE("\n");
      return;
    break;

    case 0x10: /* SUBI.b #x,<ea>*/
SHOW_OPCODE("SUBI.b #%d",SWAP_WORD(*(UWORD *)(CPU.PC+2)));
      CPU.lastresult = read_addressing_mode_nochange_offset((inst >> 3) & 0x07, inst & 0x07, 0);
      CPU.oldvalue   = CPU.lastresult;
      CPU.instsize   += 2;
      CPU.subvalue   = (UBYTE)SWAP_WORD(*(UWORD *)(CPU.PC+2));
      CPU.lastresult -= CPU.subvalue;
      write_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 0);
      
      if ((UBYTE)CPU.lastresult > (UBYTE)CPU.oldvalue)
        CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
      else
        CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
      CPU.lastop = OP_SUB_BYTE;
SHOW_OPCODE("\n");
      return;
    break;

    case 0x11: /* SUBI.w #x,<ea>*/
SHOW_OPCODE("SUBI.w #%d",SWAP_WORD(*(UWORD *)(CPU.PC+2)));
      CPU.lastresult = read_addressing_mode_nochange_offset((inst >> 3) & 0x07, inst & 0x07, 1);
      CPU.oldvalue   = CPU.lastresult;
      CPU.instsize   += 2;
      CPU.subvalue   = SWAP_WORD(*(UWORD *)(CPU.PC+2));
      CPU.lastresult -= CPU.subvalue;
      write_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 1);

      if ((UWORD)CPU.lastresult > (UWORD)CPU.oldvalue)
        CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
      else
        CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
      CPU.lastop = OP_SUB_WORD;
SHOW_OPCODE("\n");
      return;
    break;

    case 0x12: /* SUBI.l #x,<ea>*/
SHOW_OPCODE("SUBI.l #%d",SWAP(*(ULONG *)(CPU.PC+2)));
      CPU.lastresult = read_addressing_mode_nochange_offset((inst >> 3) & 0x07, inst & 0x07, 2);
      CPU.oldvalue   = CPU.lastresult;
      CPU.instsize   += 4;
      CPU.subvalue   = SWAP(*(ULONG *)(CPU.PC+2));
      CPU.lastresult -= CPU.subvalue;
      write_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 2);

      if ((ULONG)CPU.lastresult > (ULONG)CPU.oldvalue)
        CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
      else
        CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);

      CPU.lastop = OP_SUB_LONG;
SHOW_OPCODE("\n");
      return;
    break;

    case 0x18: /* ADDI.b #x,<ea>*/
SHOW_OPCODE("ADDI.b #%d",SWAP_WORD(*(UWORD *)(CPU.PC+2)));
      CPU.lastresult = read_addressing_mode_nochange_offset((inst >> 3) & 0x07, inst & 0x07, 0);
      CPU.oldvalue   = CPU.lastresult;
      CPU.instsize   += 2;
      CPU.addvalue   = (UBYTE)SWAP_WORD(*(UWORD *)(CPU.PC+2));
      CPU.lastresult += CPU.addvalue;
      write_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 0);

      if ((UBYTE)CPU.lastresult < (UBYTE)CPU.oldvalue)
        CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
      else
        CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);

      CPU.lastop = OP_ADD_BYTE;
SHOW_OPCODE("\n");
    break;

    case 0x19: /* ADDI.w #x,<ea>*/
SHOW_OPCODE("ADDI.w #%d",SWAP_WORD(*(UWORD *)(CPU.PC+2)));
      CPU.lastresult = read_addressing_mode_nochange_offset((inst >> 3) & 0x07, inst & 0x07, 1);
      CPU.oldvalue   = CPU.lastresult;
      CPU.instsize   += 2;
      CPU.addvalue   = SWAP_WORD(*(UWORD *)(CPU.PC+2));
      CPU.lastresult += CPU.addvalue;
      write_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 1);

      if ((UWORD)CPU.lastresult < (UWORD)CPU.oldvalue)
        CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
      else
        CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);

      CPU.lastop = OP_ADD_WORD;
SHOW_OPCODE("\n");
    break;

    case 0x1a: /* ADDI.l #x,<ea>*/
SHOW_OPCODE("ADDI.l #%d",SWAP(*(ULONG *)(CPU.PC+2)));
      CPU.lastresult = read_addressing_mode_nochange_offset((inst >> 3) & 0x07, inst & 0x07, 2);
      CPU.oldvalue   = CPU.lastresult;
      CPU.instsize   += 4;
      CPU.addvalue   = SWAP(*(ULONG *)(CPU.PC+2));
      CPU.lastresult += CPU.addvalue;
      write_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 2);

      if ((ULONG)CPU.lastresult < (ULONG)CPU.oldvalue)
        CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
      else
        CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);

      CPU.lastop = OP_ADD_LONG;
SHOW_OPCODE("\n");
    break;

    case 0x20: /* BTST #i,<ea> */
      CPU.instsize += 2;

SHOW_OPCODE("BTST #%d,",SWAP_WORD(*(UWORD*)(CPU.PC+2)));
      if (0 == ((inst >> 3) & 0x07))
      {
        /* Destination is a register Dn */
        reg_field = inst & 0x07;
        mask = (0x01 << (SWAP_WORD(*(UWORD*)(CPU.PC+2)) & 31 ) );
        if (mask & CPU.D[reg_field])
          CPU.SR &= ~CPUFLAG_Z;
        else
          CPU.SR |= CPUFLAG_Z;
        CPU.lastop |= OP_DONTCALCFLAGZ;
SHOW_OPCODE("D%d\n",reg_field);
        return;
      }
      else
      {
        /* Destination is a memory address */
        val = read_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, 0);
        mask = (0x01 << (SWAP_WORD(*(UWORD*)(CPU.PC+2)) & 7 ) );
        if (mask & val)
          CPU.SR &= ~CPUFLAG_Z;
        else
          CPU.SR |= CPUFLAG_Z;
        CPU.lastop |= OP_DONTCALCFLAGZ;

SHOW_OPCODE("\n");
        return;
      }      
    break;


    case 0x21: /* BCHG #i,<ea> */
      CPU.instsize += 2;

SHOW_OPCODE("BCHG #%d,",SWAP_WORD(*(UWORD*)(CPU.PC+2)));
      if (0 == ((inst >> 3) & 0x07))
      {
        /* Destination is a register Dn */
        reg_field = inst & 0x07;
        mask = (0x01 << (SWAP_WORD(*(UWORD*)(CPU.PC+2)) & 31 ) );
        if (mask & CPU.D[reg_field])
          CPU.SR &= ~CPUFLAG_Z;
        else
          CPU.SR |= CPUFLAG_Z;
        CPU.lastop |= OP_DONTCALCFLAGZ;

        CPU.D[reg_field] ^= mask;
SHOW_OPCODE("D%d\n",reg_field);
        return;
      }
      else
      {
        /* Destination is a memory address */
        val = read_addressing_mode_nochange_offset((inst >> 3) & 0x07, inst & 0x07, 0);
        mask = (0x01 << (SWAP_WORD(*(UWORD*)(CPU.PC+2)) & 7 ) );
        if (mask & val)
          CPU.SR &= ~CPUFLAG_Z;
        else
          CPU.SR |= CPUFLAG_Z;
        CPU.lastop |= OP_DONTCALCFLAGZ;

        write_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, val^mask, 0);
SHOW_OPCODE("\n");
        return;
      }      
    break;


    case 0x22: /* BCLR #i,<ea> */
      CPU.instsize += 2;
      
SHOW_OPCODE("BCLR #%d,",SWAP_WORD(*(UWORD*)(CPU.PC+2)));
      if (0 == ((inst >> 3) & 0x07))
      {
        /* Destination is a register Dn */
        reg_field = inst & 0x07;
        mask = (0x01 << (SWAP_WORD(*(UWORD*)(CPU.PC+2)) & 31 ) );
        if (mask & CPU.D[reg_field])
          CPU.SR &= ~CPUFLAG_Z;
        else
          CPU.SR |= CPUFLAG_Z;
        CPU.lastop |= OP_DONTCALCFLAGZ;

        CPU.D[reg_field] &= ~mask;
SHOW_OPCODE("D%d\n",reg_field);
        return;
      }
      else
      {
        /* Destination is a memory address */
        val = read_addressing_mode_nochange_offset((inst >> 3) & 0x07, inst & 0x07, 0);
        mask = (0x01 << (SWAP_WORD(*(UWORD*)(CPU.PC+2)) & 7 ) );
        if (mask & val)
          CPU.SR &= ~CPUFLAG_Z;
        else
          CPU.SR |= CPUFLAG_Z;
        CPU.lastop |= OP_DONTCALCFLAGZ;

        CPU.D[reg_field] &= ~mask;

        write_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, val & ~(mask), 0);
SHOW_OPCODE("\n");
        return;
      }      
    break;


    case 0x23: /* BSET #i,<ea> */
      CPU.instsize += 2;
      
SHOW_OPCODE("BSET #%d,",SWAP_WORD(*(UWORD*)(CPU.PC+2)));
      if (0 == ((inst >> 3) & 0x07))
      {
        /* Destination is a register Dn */
        reg_field = inst & 0x07;
        mask = (0x01 << (SWAP_WORD(*(UWORD*)(CPU.PC+2)) & 31 ) );
        if (mask & CPU.D[reg_field])
          CPU.SR &= ~CPUFLAG_Z;
        else
          CPU.SR |= CPUFLAG_Z;
        CPU.lastop |= OP_DONTCALCFLAGZ;

        CPU.D[reg_field] |= mask;
SHOW_OPCODE("D%d\n",reg_field);
        return;
      }
      else
      {
        /* Destination is a memory address */
        val = read_addressing_mode_nochange_offset((inst >> 3) & 0x07, inst & 0x07, 0);
        mask = (0x01 << (SWAP_WORD(*(UWORD*)(CPU.PC+2)) & 7 ) );
        if (mask & val)
          CPU.SR &= ~CPUFLAG_Z;
        else
          CPU.SR |= CPUFLAG_Z;
        CPU.lastop |= OP_DONTCALCFLAGZ;

        write_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, val | mask, 0);
SHOW_OPCODE("\n");
        return;
      }      
    break;


    case 0x28: /* EORI.b #x,<ea>*/
SHOW_OPCODE("EORI.b 0x%x,",SWAP_WORD(*(UWORD *)(CPU.PC+2)));
      if ((inst & 0x3f) == 0x3c)
      {
        /* EORI.b #x,SR */
        CPU.SR ^= (UBYTE)SWAP_WORD(*(WORD *)(CPU.PC+2));
        CPU.lastop = OP_UPTODATE;
SHOW_OPCODE(",SR\n");
      }
      else
      {
        CPU.lastresult = read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, 0);
        CPU.lastresult ^= (UBYTE)SWAP_WORD(*(UWORD *)(CPU.PC+2));
        write_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 0);
        CPU.lastop = OP_EOR_BYTE;
      }
      CPU.instsize += 2;
SHOW_OPCODE("\n");
      return;
    break;

    case 0x29: /* EORI.w #x,<ea>*/
SHOW_OPCODE("EORI.w 0x%x,",SWAP_WORD(*(UWORD *)(CPU.PC+2)));
      CPU.lastresult  = read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, 1);
      CPU.instsize   += 2;
      CPU.lastresult ^= SWAP_WORD(*(UWORD *)(CPU.PC+2));
      write_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 1);
      CPU.lastop = OP_EOR_WORD;
SHOW_OPCODE("\n");
    break;

    case 0x2a: /* EORI.l #x,<ea>*/
SHOW_OPCODE("EORI.l 0x%x,",SWAP(*(ULONG *)(CPU.PC+2)));
      CPU.lastresult = read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, 2);
      CPU.instsize   += 4;
      CPU.lastresult ^= SWAP(*(ULONG *)(CPU.PC+2));
      write_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 2);
      CPU.lastop = OP_EOR_LONG;
SHOW_OPCODE("\n");
    break;

    case 0x30: /* CMPI.b #x,<ea>*/
SHOW_OPCODE("CMPI.b #%d",SWAP_WORD(*(UWORD *)(CPU.PC+2)));
      CPU.lastresult  = read_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, 0);
      CPU.oldvalue    = CPU.lastresult;
      CPU.instsize   += 2;
      CPU.subvalue    = (UBYTE)SWAP_WORD(*(UWORD *)(CPU.PC+2));
      CPU.lastresult -= CPU.subvalue;

      CPU.lastop = OP_CMP_BYTE;
SHOW_OPCODE("\n");
      return;
    break;

    case 0x31: /* CMPI.w #x,<ea>*/
SHOW_OPCODE("CMPI.w #%d",SWAP_WORD(*(UWORD *)(CPU.PC+2)));
      CPU.lastresult = read_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, 1);
      CPU.oldvalue    = CPU.lastresult;
      CPU.instsize   += 2;
      CPU.subvalue    = SWAP_WORD(*(UWORD *)(CPU.PC+2));
      CPU.lastresult -= CPU.subvalue;

      CPU.lastop = OP_CMP_WORD;
SHOW_OPCODE("\n");
      return;
    break;

    case 0x32: /* CMPI.l #x,<ea>*/
SHOW_OPCODE("CMPI.l #%d",SWAP(*(ULONG *)(CPU.PC+2)));
      CPU.lastresult = read_addressing_mode_offset((inst >> 3) & 0x07, inst & 0x07, 2);
      CPU.oldvalue    = CPU.lastresult;
      CPU.instsize   += 4;
      CPU.subvalue    = SWAP(*(ULONG *)(CPU.PC+2));
      CPU.lastresult -= CPU.subvalue;

      CPU.lastop = OP_CMP_LONG;
SHOW_OPCODE("\n");
      return;
    break;


    case 0x04:
    case 0x0c:
    case 0x14:
    case 0x1c:
    case 0x24:
    case 0x2c:
    case 0x34:
    case 0x3c: /* BTST Dn,<ea> */
      n = (inst >> 9) & 0x07;
SHOW_OPCODE("BTST D%d,",n);
      if (0 == ((inst >> 3) & 0x07))
      {
        /* Destination is a register Dn */
        reg_field = inst & 0x07;
        mask = 0x01 << (CPU.D[n] & 31 );
        if (mask & CPU.D[reg_field])
          CPU.SR &= ~CPUFLAG_Z;
        else
          CPU.SR |= CPUFLAG_Z;
        CPU.lastop |= OP_DONTCALCFLAGZ;
SHOW_OPCODE("D%d\n",reg_field);
        return;
      }
      else
      {
        /* Destination is a memory address */
        val = read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 0);
        mask = 0x01 << (CPU.D[n] & 7 );
        if (mask & val)
          CPU.SR &= ~CPUFLAG_Z;
        else
          CPU.SR |= CPUFLAG_Z;
        CPU.lastop |= OP_DONTCALCFLAGZ;

SHOW_OPCODE("\n");
        return;
      }      
    break;


    case 0x05:
    case 0x0d:
    case 0x15:
    case 0x1d:
    case 0x25:
    case 0x2d:
    case 0x35:
    case 0x3d: /* BCHG Dn,<ea>*/
      n = (inst >> 9) & 0x07;
SHOW_OPCODE("BCHG D%d,",n);
      if (0 == ((inst >> 3) & 0x07))
      {
        /* Destination is a register Dn */
        reg_field = inst & 0x07;
        mask = 0x01 << (CPU.D[n] & 31 );
        if (mask & CPU.D[reg_field])
          CPU.SR &= ~CPUFLAG_Z;
        else
          CPU.SR |= CPUFLAG_Z;
        CPU.lastop |= OP_DONTCALCFLAGZ;

        CPU.D[reg_field] ^= mask;
SHOW_OPCODE("D%d\n",reg_field);
        return;
      }
      else
      {
        /* Destination is a memory address */
        val = read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, 0);
        mask = 0x01 << (CPU.D[n] & 7 );
        if (mask & val)
          CPU.SR &= ~CPUFLAG_Z;
        else
          CPU.SR |= CPUFLAG_Z;
        CPU.lastop |= OP_DONTCALCFLAGZ;

        write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, val ^ mask, 0);
SHOW_OPCODE("\n");
        return;
      }      
    break;


    case 0x06:
    case 0x0e:
    case 0x16:
    case 0x1e:
    case 0x26:
    case 0x2e:
    case 0x36:
    case 0x3e: /* BCLR Dn,<ea>*/
      n = (inst >> 9) & 0x07;
SHOW_OPCODE("BCLR D%d,",n);
      if (0 == ((inst >> 3) & 0x07))
      {
        /* Destination is a register Dn */
        reg_field = inst & 0x07;
        mask = 0x01 << (CPU.D[n] & 31 );
        if (mask & CPU.D[reg_field])
          CPU.SR &= ~CPUFLAG_Z;
        else
          CPU.SR |= CPUFLAG_Z;
        CPU.lastop |= OP_DONTCALCFLAGZ;

        CPU.D[reg_field] &= ~mask;
SHOW_OPCODE("D%d\n",reg_field);
        return;
      }
      else
      {
        /* Destination is a memory address */
        val = read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, 0);
        mask = 0x01 << (CPU.D[n] & 7 );
        if (mask & val)
          CPU.SR &= ~CPUFLAG_Z;
        else
          CPU.SR |= CPUFLAG_Z;
        CPU.lastop |= OP_DONTCALCFLAGZ;

        write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, val & ~(mask), 0);
SHOW_OPCODE("\n");
        return;
      }      
    break;

    case 0x07:
    case 0x0f:
    case 0x17:
    case 0x1f:
    case 0x27:
    case 0x2f:
    case 0x37:
    case 0x3f: /* BSET Dn,<ea> */
      n = (inst >> 9) & 0x07;
SHOW_OPCODE("BSET D%d,",n);
      if (0 == ((inst >> 3) & 0x07))
      {
        /* Destination is a register Dn */
        reg_field = inst & 0x07;
        mask = 0x01 << (CPU.D[n] & 31 );
        if (mask & CPU.D[reg_field])
          CPU.SR &= ~CPUFLAG_Z;
        else
          CPU.SR |= CPUFLAG_Z;
        CPU.lastop |= OP_DONTCALCFLAGZ;

        CPU.D[reg_field] |= mask;
SHOW_OPCODE("D%d\n",reg_field);
        return;
      }
      else
      {
        /* Destination is a memory address */
        val = read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, 0);
        mask = 0x01 << (CPU.D[n] & 7 );
        if (mask & val)
          CPU.SR &= ~CPUFLAG_Z;
        else
          CPU.SR |= CPUFLAG_Z;
        CPU.lastop |= OP_DONTCALCFLAGZ;

        write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, val | mask, 0);
SHOW_OPCODE("\n");
        return;
      }      
    break;

  }
}


/* Move Byte */
void Group1(UWORD inst)
{
SHOW_OPCODE("MOVE");
  CPU.lastresult = read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 0);
SHOW_OPCODE(",");
  write_addressing_mode((inst >> 6) & 0x07, (inst >> 9) & 0x07, CPU.lastresult, 0);

  if (0x040 != (inst & 0x1c0))
    CPU.lastop = OP_MOVE_BYTE;
SHOW_OPCODE("\n");
}


/* Move Long */
void Group2(UWORD inst)
{
SHOW_OPCODE("MOVE");
  CPU.lastresult = read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 2);
SHOW_OPCODE(",");
  write_addressing_mode((inst >> 6) & 0x07, (inst >> 9) & 0x07, CPU.lastresult, 2);
SHOW_OPCODE("\n");
  if (0x040 != (inst & 0x1c0))
    CPU.lastop = OP_MOVE_LONG;
}


/* Move Word */
void Group3(UWORD inst)
{
SHOW_OPCODE("MOVE");
  CPU.lastresult = read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 1);
SHOW_OPCODE(",");
  write_addressing_mode((inst >> 6) & 0x07, (inst >> 9) & 0x07, CPU.lastresult, 1);
SHOW_OPCODE("\n");
  if (0x040 != (inst & 0x1c0))
    CPU.lastop = OP_MOVE_WORD;
}


/* Misc. Instructions */
void Group4(UWORD inst)
{
  UBYTE reg_field;
  ULONG val;
  UWORD flags;
  switch((inst >> 6) & 0x3f)
  {
    case 0x00: /* NEGX.b */
SHOW_OPCODE("NEG.b ");
      if (0 != (CPU.SR & CPUFLAG_X))
        CPU.lastresult = (BYTE)0-(BYTE)read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, 0);
      else
        CPU.lastresult = (BYTE)-1-(BYTE)read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, 0);
      write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 0);
      CPU.lastop = OP_NEGX_BYTE;
SHOW_OPCODE("\n");
      return;
    break;
    
    case 0x01: /* NEGX.w */
SHOW_OPCODE("NEG.w ");
      if (0 != (CPU.SR & CPUFLAG_X))
        CPU.lastresult = (WORD)0-(WORD)read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, 1);
      else
        CPU.lastresult = (WORD)-1-(WORD)read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, 1);
      write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 1);
      CPU.lastop = OP_NEGX_WORD;
SHOW_OPCODE("\n");
      return;
    break;
    
    case 0x02: /* NEGX.l */
SHOW_OPCODE("NEG.l ");
      if (0 != (CPU.SR & CPUFLAG_X))
        CPU.lastresult = (LONG)0-(LONG)read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, 2);
      else
        CPU.lastresult = (LONG)-1-(LONG)read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, 2);
      write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 2);
      CPU.lastop = OP_NEGX_LONG;
SHOW_OPCODE("\n");
      return;
    break;

    case 0x03:
SHOW_OPCODE("MOVE SR,");
      write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, CPU.SR, 1);
SHOW_OPCODE("\n");
      return;
    break;

    case 0x04:
    case 0x0c:
    case 0x14:
    case 0x1c:
    case 0x24:
    case 0x2c:
    case 0x34:
    case 0x3c: /* CHK.l <ea>,dn */
SHOW_OPCODE("CHK");
      reg_field = (inst >> 9) & 0x07;
      CPU.lastop = OP_UPTODATE;
      if ((LONG)CPU.D[reg_field] < 0)
      {
        CPU.SR |= CPUFLAG_N;
        /* !!! cause a TRAP */
      } 
      else
      if ((LONG)CPU.D[reg_field] > read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 2))
      {
        CPU.SR &= ~CPUFLAG_N;
        /* !!! cause a TRAP */
      }
SHOW_OPCODE(",D%d\n",reg_field);
      return;
    break;

    case 0x06:
    case 0x0e:
    case 0x16:
    case 0x1e:
    case 0x26:
    case 0x2e:
    case 0x36:
    case 0x3e: /* CHK.w <ea>,dn */
SHOW_OPCODE("CHK");
      reg_field = (inst >> 9) & 0x07;
      if ((WORD)CPU.D[reg_field] < 0 || 
          (WORD)CPU.D[reg_field] > read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 1))
      {
        /* !!! cause a TRAP */
      }
SHOW_OPCODE(",D%d\n",reg_field);
      return;
    break;


    case 0x7: /* LEA <ea>, A0 */
SHOW_OPCODE("LEA");
      CPU.A[0] = getaddress_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 2);
SHOW_OPCODE(",A0\n");
      return;
    break;

    case 0x8: /* CLR.b <ea> */
SHOW_OPCODE("CLR.b ");
      write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 0, 0);
      CPU.SR = (CPU.SR & CPUFLAG_X) | CPUFLAG_Z;
      CPU.lastop = OP_UPTODATE;
SHOW_OPCODE("\n");
      return;
    break;
    
    case 0x9: /* CLR.w <ea> */
SHOW_OPCODE("CLR.w ");
      write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 0, 1);
      CPU.SR = (CPU.SR & CPUFLAG_X) | CPUFLAG_Z;
      CPU.lastop = OP_UPTODATE;
SHOW_OPCODE("\n");
      return;
    break;
    
    case 0xa: /* CLR.l <ea> */
SHOW_OPCODE("CLR.l ");
      write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 0, 2);
      CPU.SR = (CPU.SR & CPUFLAG_X) | CPUFLAG_Z;
      CPU.lastop = OP_UPTODATE;
SHOW_OPCODE("\n");
      return;
    break;
    
    case 0x0b:
SHOW_OPCODE("MOVE SR,");
      write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, CPU.SR, 1);
SHOW_OPCODE("\n");
      return;
    break;

    case 0xf: /* LEA <ea>, A1 */
SHOW_OPCODE("LEA ");
      CPU.A[1] = getaddress_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 2);
SHOW_OPCODE(",A1\n");
      return;
    break;

    case 0x10: /* NEG.b */
SHOW_OPCODE("NEG.b ");
      CPU.lastresult = (BYTE)0-(BYTE)read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, 0);
      write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 0);
      CPU.lastop = OP_NEG_BYTE;
SHOW_OPCODE("\n");
      return;
    break;
    
    case 0x11: /* NEG.w */
SHOW_OPCODE("NEG.w ");
      CPU.lastresult = (WORD)0-(WORD)read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, 1);
      write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 1);
      CPU.lastop = OP_NEG_WORD;
SHOW_OPCODE("\n");
      return;
    break;
    
    case 0x12: /* NEG.l */
SHOW_OPCODE("NEG.l ");
      CPU.lastresult = (LONG)0-(LONG)read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, 2);
      write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 2);
      CPU.lastop = OP_NEG_LONG;
SHOW_OPCODE("\n");
      return;
    break;

    case 0x13: /* MOVE <ea>,SR */
SHOW_OPCODE("MOVE");
      CPU.SR = (WORD)read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 1) & 0x1f;
SHOW_OPCODE(",SR\n");
      CPU.lastop = OP_UPTODATE;
    break;

    case 0x17: /* LEA <ea>, A2 */
SHOW_OPCODE("LEA ");
      CPU.A[2] = getaddress_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 2);
SHOW_OPCODE(",A2\n");
      return;
    break;

    case 0x18: /* NOT.b */
SHOW_OPCODE("NOT");
      CPU.lastresult = ~(BYTE)read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, 0);
      write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 0);
      CPU.lastop = OP_NOT_BYTE;
SHOW_OPCODE("\n");
    break;
    
    case 0x19: /* NOT.w */
SHOW_OPCODE("NOT");
      CPU.lastresult = ~(WORD)read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, 1);
      write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 1);
      CPU.lastop = OP_NOT_WORD;
SHOW_OPCODE("\n");
    break;
    
    case 0x1a: /* NOT.l */
SHOW_OPCODE("NOT");
      CPU.lastresult = ~(LONG)read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, 2);
      write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 2);
      CPU.lastop = OP_NOT_LONG;
SHOW_OPCODE("\n");
    break;



    case 0x1f: /* LEA <ea>, A3 */
SHOW_OPCODE("LEA ");
      CPU.A[3] = getaddress_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 2);
SHOW_OPCODE(",A3\n");
      return;
    break;

    
    case 0x20:
    {
      switch ((inst >> 3) & 0x7)
      {
        case 1: /* LINK (32 bit) */
SHOW_OPCODE("LINK A%d,%d\n",reg_field,SWAP(*(ULONG *)(CPU.PC+2)));
          reg_field = inst & 0x07;
          CPU.A[7] -= 4;
          *(ULONG *)CPU.A[7] = SWAP(CPU.A[reg_field]);
          CPU.A[reg_field] = CPU.A[7];
          CPU.A[7] += SWAP(*(ULONG *)(CPU.PC+2));
          CPU.instsize +=2;
           
        break;
      }
    }
   
    case 0x21: /* SWAP, PEA */
      if (0 == ((inst >> 3) & 0x07))
      {
        CPU.A[7] -= 4;
        *(ULONG *)CPU.A[7] = SWAP(read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 2));
        return;
      }
      else
      {
        reg_field = inst & 0x07;
SHOW_OPCODE("SWAP D%d\n",reg_field);
        val = CPU.D[reg_field];
        CPU.D[reg_field] = (val << 16) | (val >> 16);
        CPU.lastresult = CPU.D[reg_field];
        CPU.lastop = OP_SWAP;
      }
    break;
    
    case 0x22: /* EXT byte to word || MOVEM */
      reg_field = inst & 0x07;
      if (0 == (inst & 0x38))
      {
        if ((BYTE)CPU.D[reg_field] < 0)
        {
          ((UWORD *)&CPU.D[reg_field])[L_WORD] |= 0xff00;
          CPU.SR = (CPU.SR & CPUFLAG_X) | CPUFLAG_N;
        }
        else
        {
          ((UWORD *)&CPU.D[reg_field])[L_WORD] &= 0x007f;
          if (0 == (WORD)CPU.D[reg_field])
            CPU.SR = (CPU.SR & CPUFLAG_X) | CPUFLAG_Z;
          else
            CPU.SR &= CPUFLAG_X;
        }
        CPU.lastop     = OP_UPTODATE;
SHOW_OPCODE("EXT.w D%d\n",reg_field);
      }
      else
      {
        /* MOVEM.w register to memory */
        UWORD list = SWAP_WORD(*(UWORD *)(CPU.PC+2));
        UWORD mask = 0x01;
        UWORD reg = 15;
        CPU.instsize += 2;
SHOW_OPCODE("MOVEM ");
        while (mask)
        {
          if (list & mask)
          {
#ifdef DEBUGGING
if (reg > 7)
  SHOW_OPCODE("A%d",reg-8);
else
  SHOW_OPCODE("D%d",reg);
#endif
            write_addressing_mode((inst >> 3) & 0x7, inst & 0x07, CPU.D[reg], 1);
SHOW_OPCODE("/");
          }
          mask <<= 1;
          reg--;
        }
SHOW_OPCODE("\n");
      }
      return;
    break;
    
    case 0x23: /* EXT word to long */
      reg_field = inst & 0x07;
      if (0 == (inst & 0x38))
      {
        if ((WORD)CPU.D[reg_field] < 0)
        {
          (ULONG)CPU.D[reg_field] |= 0xffff0000;
          CPU.SR = (CPU.SR & CPUFLAG_X) | CPUFLAG_N;
        }
        else
        {
          (ULONG)CPU.D[reg_field] &= 0x00007fff;
          if (0 == CPU.D[reg_field])
            CPU.SR = (CPU.SR & CPUFLAG_X) | CPUFLAG_Z;
          else
            CPU.SR &= CPUFLAG_X;
        }
        CPU.lastop     = OP_UPTODATE;
SHOW_OPCODE("EXT.l D%d\n",reg_field);
      }
      else
      {
        /* MOVEM.l register to memory */
        UWORD list = SWAP_WORD(*(UWORD *)(CPU.PC+2));
        UWORD mask = 0x01;
        UWORD reg = 15;
        CPU.instsize += 2;
SHOW_OPCODE("MOVEM.w ");
        while (mask)
        {
          if (list & mask)
          {
#ifdef DEBUGGING
if (reg > 7)
  SHOW_OPCODE("A%d",reg-8);
else
  SHOW_OPCODE("D%d",reg);
#endif
            write_addressing_mode((inst >> 3) & 0x7, inst & 0x07, CPU.D[reg], 2);
SHOW_OPCODE("/");
          }
          mask <<= 1;
          reg--;
        }
SHOW_OPCODE("\n");
      }
      return;
    break;
    

    case 0x27: /* EXT byte to long || LEA <ea>, a4 */
      switch ((inst >>3 ) & 0x07)
      {
        case 0x0: /* EXT byte to long */
          reg_field = inst & 0x07;
          if ((BYTE)CPU.D[reg_field] < 0)
          {
            (UWORD)CPU.D[reg_field] |= 0xffffff00;
            CPU.SR = (CPU.SR & CPUFLAG_X) | CPUFLAG_N;
          }
          else
          {
            (UWORD)CPU.D[reg_field] &= 0x0000007f;
            if (0 == CPU.D[reg_field])
              CPU.SR = (CPU.SR & CPUFLAG_X) | CPUFLAG_Z;
            else
              CPU.SR &= CPUFLAG_X;
          }
          CPU.lastop     = OP_UPTODATE;

SHOW_OPCODE("EXTB.l D%d\n",reg_field);
          return;
        break;

        case 0x1:
        break;
        
        case 0x3:
        break;
        
        case 0x4:
        break;

        default: /* LEA <ea>, A4 */
SHOW_OPCODE("LEA ");
          CPU.A[4] = getaddress_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 2);
SHOW_OPCODE(",A4\n");
          return;
      }
    break;
    
    case 0x28: /* TST.b <ea> */
SHOW_OPCODE("TST");
      val = read_addressing_mode((inst >>3) & 0x07, inst & 0x07, 0);
      /* I expect that the next command is a Bcc so I generate the CCs */
      flags = CPU.SR & CPUFLAG_X;
      if (0 == (BYTE)val)
        flags |= CPUFLAG_Z;
      if ((BYTE)val < 0)
        flags |= CPUFLAG_N;
      CPU.SR = flags;
      CPU.lastop = OP_UPTODATE;
SHOW_OPCODE("\n");
    break;
    
    case 0x29: /* TST.w <ea> */
SHOW_OPCODE("TST");
      val = read_addressing_mode((inst >>3) & 0x07, inst & 0x07, 1);
      /* I expect that the next command is a Bcc so I generate the CCs */
      flags = CPU.SR & CPUFLAG_X;
      if (0 == (WORD)val)
        flags |= CPUFLAG_Z;
      if ((WORD)val < 0)
        flags |= CPUFLAG_N;
      CPU.SR = flags;
      CPU.lastop = OP_UPTODATE;
SHOW_OPCODE("\n");
      return;
    break;
    
    case 0x2a: /* TST.l <ea> */
SHOW_OPCODE("TST");
      val = read_addressing_mode((inst >>3) & 0x07, inst & 0x07, 2);
      /* I expect that the next command is a Bcc so I generate the CCs */
      flags = CPU.SR & CPUFLAG_X;
      if (0 == (LONG)val)
        flags |= CPUFLAG_Z;
      if ((LONG)val < 0)
        flags |= CPUFLAG_N;
      CPU.SR = flags;
      CPU.lastop = OP_UPTODATE;
SHOW_OPCODE("\n");
      return;
    break;

    case 0x2b: /* TAS <ea> */
SHOW_OPCODE("TAS");
      val = (UBYTE)read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, 0);
      write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, (val | 0x80), 0);
      flags = CPU.SR & CPUFLAG_X;
      if (0 == (BYTE)val)
        flags |= CPUFLAG_Z;
      if ((BYTE)val < 0)
        flags |= CPUFLAG_N;
      CPU.SR = flags;
      CPU.lastop = OP_UPTODATE;
SHOW_OPCODE("\n");
      return;
    break;
    
    case 0x2f: /* LEA <ea>, A5 */
SHOW_OPCODE("LEA ");
      CPU.A[5] = getaddress_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 2);
      return;
SHOW_OPCODE(",A5\n");
    break;

    case 0x32: /* MOVEM.w */
    {
      if (0 == inst & 0x38)
      {
      }
      else
      {
        /* MOVEM memory to register */
        UWORD list = SWAP_WORD(*(UWORD *)(CPU.PC+2));
        UWORD mask = 0x1;
        UWORD reg = 0;
        CPU.instsize += 2;
SHOW_OPCODE("MOVEM.w");
        while (mask)
        {
          if (mask & reg)
          {
            ((UWORD *)&CPU.D[reg])[L_WORD] = read_addressing_mode((inst >> 3) & 0x7, inst &0x7, 1);
#ifdef DEBUGGING
if (reg > 7)
  SHOW_OPCODE("A%d/",reg-8);
else
  SHOW_OPCODE("D%d/",reg);
#endif
          }
          reg++;
          mask <<= 1;
        }
      }
SHOW_OPCODE("\n");
      return;
    }
    break;

    case 0x33: /* MOVEM.l */
    {
      if (0 == inst & 0x38)
      {
      }
      else
      {
        /* MOVEM memory to register */
        UWORD list = SWAP_WORD(*(UWORD *)(CPU.PC+2));
        UWORD mask = 0x1;
        UWORD reg = 0;
        CPU.instsize += 2;
SHOW_OPCODE("MOVEM.l");
        while (mask)
        {
          if (mask & reg)
          {
            CPU.D[reg] = read_addressing_mode((inst >> 3) & 0x7, inst &0x7, 2);
#ifdef DEBUGGING
if (reg > 7)
  SHOW_OPCODE("A%d/",reg-8);
else
  SHOW_OPCODE("D%d/",reg);
#endif
          }
          reg++;
          mask <<= 1;
        }
      }
SHOW_OPCODE("\n");
      return;
    }
    break;
    
    case 0x37: /* LEA <ea>, A6 */
SHOW_OPCODE("LEA ");
      CPU.A[6] = getaddress_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 2);
SHOW_OPCODE(",A6\n");
      return;
    break;

   
    case 0x39: /* RTS, LINK, RTE */
    {
      switch (inst & 0x3f)
      {
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17: /* LINK (16 bit) */
          reg_field = inst & 0x07;
          CPU.A[7] -= 4;
          *(ULONG *)CPU.A[7] = SWAP(CPU.A[reg_field]);
          CPU.A[reg_field] = CPU.A[7];
          CPU.A[7] += SWAP_WORD(*(WORD *)(CPU.PC+2));
          CPU.instsize +=2;
SHOW_OPCODE("LINK.w A%d,%d\n",reg_field,SWAP_WORD(*(WORD*)(CPU.PC+2)));
          return;
        break;
        
        case 0x18:
        case 0x19:
        case 0x1a:
        case 0x1b:
        case 0x1c:
        case 0x1d:
        case 0x1e:
        case 0x1f: /* UNLINK (16 bit) */
          reg_field = inst & 0x07;
          CPU.A[7] = CPU.A[reg_field];
          CPU.A[reg_field] = *(ULONG *)CPU.A[7];
          CPU.A[7] += 4;
SHOW_OPCODE("UNLINK.w A%d\n",reg_field);
          return;          
        break;
        
        case 0x30: /* RESET */
SHOW_OPCODE("RESET\n");
          CPU.PC = 0;
          CPU.instsize = 0;
          return;
        break;
        
        case 0x31: /* NOP */
SHOW_OPCODE("NOP\n");
          return;
        break;  
        
        case 0x32: /* STOP */
SHOW_OPCODE("STOP\n");
          /* !!! Check for SUPERVISOR state */
          CPU.SR = SWAP_WORD(*(UWORD *)(CPU.PC+2));
          CPU.instsize += 2;
          return;
        break;
        
        case 0x33: /* RTE */
          /* !!! Check for SUPERVISOR state */
          CPU.SR = SWAP_WORD(*(UWORD *)CPU.A[7]);
          CPU.PC = SWAP(*(ULONG *)(CPU.A[7]+2));
          CPU.A[7] += 6;
        break;
        
        case 0x34: /* RTD */
SHOW_OPCODE("RTD\n");
          CPU.PC = SWAP(*(ULONG *)CPU.A[7]);
          CPU.A[7] += 4 + (LONG)(WORD)SWAP_WORD(*(WORD *)CPU.PC+2);
          CPU.instsize += 2;
        break;
        
        case 0x35: /* RTS */
SHOW_OPCODE("RTS\n");
          CPU.PC = SWAP(*(ULONG *)CPU.A[7]);
          CPU.A[7] += 4;
          CPU.instsize = 0;
        break;
        
        case 0x37: /* RTR */
SHOW_OPCODE("RTR\n");
          CPU.SR = SWAP_WORD(*(UWORD *)CPU.A[7]);
          CPU.PC = SWAP(*(ULONG *)(CPU.A[7]+2));
          CPU.A[7] += 6;
        break;
      }
    }
    break;
    
    case 0x3a: /* JSR */
SHOW_OPCODE("JSR ");
      val = read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 2 /* long */);
      CPU.PC += CPU.instsize;
      CPU.instsize = 0;
      CPU.A[7]-=4;
      *(ULONG *)CPU.A[7] = SWAP(CPU.PC);
      CPU.PC = val;

SHOW_OPCODE("   STACK: %8x %8x %8x\n",((ULONG *)CPU.A[7])[0],((ULONG *)CPU.A[7])[1],((ULONG *)CPU.A[7])[2]);      

    break;
    
    case 0x3b: /* JMP */
SHOW_OPCODE("JMP");
      CPU.PC = read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 2);
      CPU.instsize = 0;
SHOW_OPCODE("\n");
    break;

    case 0x3f: /* LEA <ea>, A7 */
SHOW_OPCODE("LEA ");
      CPU.A[7] = getaddress_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 2);
SHOW_OPCODE(",A7\n");
      return;
    break;

  }  
}


/* ADDQ, SUBQ, DBcc */
void Group5(UWORD inst)
{
  ULONG val;
  UBYTE size = (inst >> 6) & 0x3;
  if (0x3 == size)
  {
    /* DBcc */
SHOW_OPCODE("D");
    if (FALSE == test_cc(inst, 1))
    {
SHOW_OPCODE(" D%d,%d\n",inst & 0x07,(LONG)(WORD)SWAP_WORD(*(UWORD *)(CPU.PC+2))+2);
      ((UWORD *)&CPU.D[inst & 0x07])[L_WORD] -= 1;
      if (-1 != (WORD)((UWORD *)&CPU.D[inst & 0x07])[L_WORD])
      {
        CPU.PC += (LONG)(WORD)SWAP_WORD(*(UWORD *)(CPU.PC+2))+2;
        CPU.instsize = 0;
      }
      else
      {
        CPU.instsize += 2;
      }
    }
    else
    {
SHOW_OPCODE(" D%d,%d\n",inst & 0x07,(LONG)(WORD)SWAP_WORD(*(UWORD *)(CPU.PC+2))+2);
      CPU.instsize += 2;
    }
  }
  else
  switch ((inst >> 8) & 1)
  {
    case 0: /* ADDQ */
      CPU.lastresult = read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, size);
#ifdef DEBUGGING
switch (size)
{
  case 0:
SHOW_OPCODE("ADDQ.b #%d,",(inst>>9) & 0x07);
  break;
  
  case 1:
SHOW_OPCODE("ADDQ.w #%d,",(inst >> 9) & 0x07);
  break;
  
  case 2:
SHOW_OPCODE("ADDQ.l #%d,",(inst >> 9) & 0x07);
  break;
}
#endif
      CPU.oldvalue    = CPU.lastresult;
      CPU.addvalue    = (inst >> 9) & 0x07;
      CPU.lastresult += CPU.addvalue;
      write_addressing_mode((inst >>3) & 0x07, inst & 0x07, CPU.lastresult, size);

      if (0x8 != (inst & 0x38))
      {
        CPU.lastop = OP_TYPE2|size;
        switch (size)
        {
          case 0:
            if ((UBYTE)CPU.lastresult < (UBYTE)CPU.oldvalue)
              CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
            else
             CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
          break;
          
          case 1:
            if ((UWORD)CPU.lastresult < (UWORD)CPU.oldvalue)
              CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
            else
             CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
          break;
          
          case 2:
            if ((ULONG)CPU.lastresult < (ULONG)CPU.oldvalue)
              CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
            else
             CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
          break;
        }
      }
SHOW_OPCODE("\n");
      return;
    break;

    case 1: /* SUBQ */
      CPU.lastresult = read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, size);
#ifdef DEBUGGING
switch (size)
{
  case 0:
SHOW_OPCODE("SUBQ.b #%d,",(inst>>9) & 0x07);
  break;
  
  case 1:
SHOW_OPCODE("SUBQ.w #%d,",(inst >> 9) & 0x07);
  break;
  
  case 2:
SHOW_OPCODE("SUBQ.l #%d,",(inst >> 9) & 0x07);
  break;
}
#endif
      CPU.oldvalue    = CPU.lastresult;
      CPU.subvalue    = (inst >> 9) & 0x07;
      CPU.lastresult -= CPU.subvalue;
      write_addressing_mode((inst >>3) & 0x07, inst & 0x07, CPU.lastresult, size);
      if (0x8 != (inst & 0x38))
      {
        CPU.lastop = OP_TYPE4|size;
        switch (size)
        {
          case 0:
            if ((UBYTE)CPU.lastresult > (UBYTE)CPU.oldvalue)
              CPU.SR |= (CPUFLAG_C|CPUFLAG_X); 
            else
              CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
          break;

          case 1:
            if ((UWORD)CPU.lastresult > (UWORD)CPU.oldvalue)
              CPU.SR |= (CPUFLAG_C|CPUFLAG_X); 
            else
              CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
          break;

          case 2:
            if ((ULONG)CPU.lastresult > (ULONG)CPU.oldvalue)
              CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
            else
              CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
          break;
        }
      }
SHOW_OPCODE("\n");
      return;
    break;
  }
}

/* BSR, BRA, Bcc */
void Group6(UWORD inst)
{
  ULONG offset;
  UWORD flags;
  
UWORD size=0;

  if (0 == (UBYTE)inst)
  {
    /* word displacement */
    CPU.instsize += 2;
size = 1;
  }
  else if (0xff == (UBYTE)inst)
  {
    /* long displacement */
    CPU.instsize += 4;
size = 2;
  }
  
  if (0x0100 == (inst & 0x0f00) /* BSR */ || TRUE == test_cc(inst,size))
  {
    if (0x0100 == (inst & 0x0f00))
    {
      CPU.A[7] -= 4;
      *(ULONG *)CPU.A[7] = SWAP(CPU.PC+CPU.instsize);
    }
    
    if (0 == (UBYTE)inst)
    {
      /* word displacement */
      offset = (LONG)(WORD)SWAP_WORD(*(WORD *)(CPU.PC+2))-2;  
    }
    else if (0xff == (UBYTE)inst)
    {
      /* long displacement */
      offset = (LONG)SWAP(*(LONG *)(CPU.PC+2))-4;
    }
    else
    {
      /* byte displacement */
      offset = (LONG)(WORD)(BYTE)(inst & 0xff);
    }

    CPU.instsize += offset;
    SHOW_OPCODE(" %d\n",offset);
  }
  return;
}

/* MOVEQ */
void Group7(UWORD inst)
{
SHOW_OPCODE("MOVEQ #%i,D%d\n",(BYTE)inst,(inst >> 9) & 0x07 );
  CPU.D[(inst >> 9) & 0x07] = (LONG)(WORD)(BYTE)(inst&0xff);
  CPU.lastresult            = (LONG)(WORD)(BYTE)(inst&0xff);
  CPU.lastop = OP_MOVE_LONG;
}


/* OR, DIVS, DIVU */
void Group8(UWORD inst)
{
  UBYTE size = (inst >> 6) & 0x03;
  ULONG val;
  UBYTE reg_field = (inst >> 9) & 0x07;

SHOW_OPCODE("OR");

  switch ((inst >> 8) & 1)
  {
    case 0: /* <ea> & Dn -> Dn */ 
      val = read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, size);
      switch (size)
      {
        case 0:
          ((UBYTE *)&CPU.D[reg_field])[L_BYTE] |= (UBYTE)val;
        break;
        
        case 1:
          ((UWORD *)&CPU.D[reg_field])[L_WORD] |= (UWORD)val;
        break;
        
        case 2:
          (ULONG)CPU.D[reg_field] |= (ULONG)val;
        break;
      }
      CPU.lastresult = CPU.D[reg_field];
      CPU.lastop = OP_TYPE1|size;
SHOW_OPCODE(",D%d\n",reg_field);
      return;
    break;
    
    case 1: /* <ea> | Dn -> <ea> */
      switch ((inst >> 6) & 0x07)
      {
        case 0x07: /* DIVS */
SHOW_OPCODE("DIVS");
          val = (LONG)(WORD)read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 1);
SHOW_OPCODE(",D%d",reg_field);
          if (0 != val)
          {
            val = ((LONG)CPU.D[reg_field] / (LONG)val) & 0xffff;
            val |= (((LONG)CPU.D[reg_field] % (LONG)val) << 16);
            CPU.D[reg_field] = val;
            CPU.lastop = OP_DIVS_WORD;
          }
          else
          {
SHOW_OPCODE(" -->Trap!");
          }
SHOW_OPCODE("\n");
        break;
        
        case 0x06:
SHOW_OPCODE("DIVS");
          val = (LONG)(WORD)read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 1);
SHOW_OPCODE(",D%d\n",reg_field);
          if (0 != val)
          {
            val = ((LONG)CPU.D[reg_field] / (LONG)val) & 0xffff;
            val |= (((LONG)CPU.D[reg_field] % (LONG)val) << 16);
            CPU.D[reg_field] = val;
            CPU.lastop = OP_DIVS_WORD;
          }
          else
          {
SHOW_OPCODE(" -->Trap!");
          }
SHOW_OPCODE("\n");
        break;
        
        default:
          CPU.lastresult = read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, size);
#ifdef DEBUGGING
switch (size)
{
  case 0:
    SHOW_OPCODE(".b D%d,",reg_field);
  break;
        
  case 1:
    SHOW_OPCODE(".w D%d,",reg_field);
  break;
        
  case 2:
    SHOW_OPCODE(".l D%d,",reg_field);
  break;
}
#endif
          CPU.lastresult |= CPU.D[reg_field];
          write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, size);
          CPU.lastop = OP_TYPE1|size;
SHOW_OPCODE("\n");
          return;
        break;
      }
  }
}


/* SUB, SUBA, SUBX */
void Group9(UWORD inst)
{
  UBYTE reg_field = (inst >> 9) & 0x07;
  UBYTE size = (inst >> 6) & 0x03;
  ULONG val;

SHOW_OPCODE("SUB");
  if (0 == (inst & 0x100))
  {
    if (0 == (inst & 0x30))
    {
      /* SUBX */
      if (0 == (inst & 0x8))
      {
        /* Register operation */
        CPU.oldvalue = CPU.D[reg_field];
        switch(size)
        {
          case 0: /* BYTE */
            CPU.subvalue = CPU.D[inst & 0x07];
            if (CPU.SR & CPUFLAG_X)
              CPU.subvalue += 1;
            ((BYTE *)&CPU.D[reg_field])[L_BYTE] -= CPU.subvalue;
            CPU.lastresult = CPU.D[reg_field];
            CPU.lastop     = OP_SUB_BYTE;
            if ((UBYTE)CPU.lastresult > (UBYTE)CPU.oldvalue)
              CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
            else
              CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
SHOW_OPCODE(".b D%d,D%d\n",inst&0x07,reg_field);
            return;
          break;
        
          case 1: /* WORD */
            CPU.subvalue = CPU.D[inst & 0x07];
            if (CPU.SR & CPUFLAG_X)
              CPU.subvalue += 1;
            ((WORD *)&CPU.D[reg_field])[L_WORD] -= CPU.subvalue;
            CPU.lastresult = CPU.D[reg_field];
            CPU.lastop     = OP_SUB_WORD;
SHOW_OPCODE(".w D%d,D%d\n",inst&0x07,reg_field);
            if ((UWORD)CPU.lastresult > (UWORD)CPU.oldvalue)
              CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
            else
             CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
            return;
          break;
        
          case 2: /* LONG */
            CPU.subvalue = CPU.D[inst & 0x07];
            if (CPU.SR & CPUFLAG_X)
              CPU.subvalue += 1;
            CPU.D[reg_field] -= CPU.subvalue;
            CPU.lastresult    = CPU.D[reg_field];
            CPU.lastop        = OP_SUB_LONG;
SHOW_OPCODE(".l D%d,D%d\n",inst&0x07,reg_field);
            if ((ULONG)CPU.lastresult > (ULONG)CPU.oldvalue)
              CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
            else
             CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
            return;
          break;
        }
      }
      else
      {
        /* SUBX -(Ax),-(Ay) */  
SHOW_OPCODE("SUBX");
        CPU.oldvalue = read_addressing_mode_nochange(0x4, reg_field, size);
        CPU.lastresult = CPU.oldvalue;
        CPU.subvalue    = read_addressing_mode(0x4, inst & 0x07, size);
        if (CPU.SR & CPUFLAG_X)
          CPU.subvalue++;
        CPU.lastresult -= CPU.subvalue;
SHOW_OPCODE(",");
        write_addressing_mode(0x4, reg_field, CPU.lastresult, size);
        CPU.lastop = OP_TYPE4|size;
SHOW_OPCODE("\n");
        switch(size)
        {
          case 0x0:
            if ((UBYTE)CPU.lastresult > (UBYTE)CPU.oldvalue)
              CPU.SR |= (CPUFLAG_C|CPUFLAG_X); 
            else
              CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
          break;
          
          case 1:
            if ((UWORD)CPU.lastresult > (UWORD)CPU.oldvalue)
              CPU.SR |= (CPUFLAG_C|CPUFLAG_X); 
            else
              CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
          break;
          
          case 2:
            if ((ULONG)CPU.lastresult > (ULONG)CPU.oldvalue)
              CPU.SR |= (CPUFLAG_C|CPUFLAG_X); 
            else
              CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
          break;
        }  
      }
    }
    else
    {
      /* <ea> - Dn -> Dn */
      CPU.oldvalue = CPU.D[reg_field];
      switch (size)
      {
        case 0: /* BYTE */
          CPU.subvalue = (UBYTE)read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, size);
          ((UBYTE *)&CPU.D[reg_field])[L_BYTE] -= CPU.subvalue;
          CPU.lastresult               = CPU.D[reg_field];
          CPU.lastop                   = OP_SUB_BYTE;

          if ((UBYTE)CPU.lastresult > (UBYTE)CPU.oldvalue)
            CPU.SR |= (CPUFLAG_C|CPUFLAG_X); 
          else
            CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);

SHOW_OPCODE(",D%d\n",reg_field);
          return;
        break;

        case 1: /* WORD */
          CPU.subvalue = (UWORD)read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, size);
          ((UWORD *)&CPU.D[reg_field])[L_WORD] -= CPU.subvalue;
          CPU.lastresult               = CPU.D[reg_field];
          CPU.lastop                   = OP_SUB_WORD;

          if ((UWORD)CPU.lastresult > (UWORD)CPU.oldvalue)
            CPU.SR |= (CPUFLAG_C|CPUFLAG_X); 
          else
            CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
SHOW_OPCODE(",D%d\n",reg_field);
          return;
        break;
      
        case 2: /* LONG */
          CPU.subvalue = read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, size);
          CPU.D[reg_field] -= CPU.subvalue;
          CPU.lastresult    = CPU.D[reg_field];
          CPU.lastop        = OP_SUB_LONG;

          if ((ULONG)CPU.lastresult > (ULONG)CPU.oldvalue)
            CPU.SR |= (CPUFLAG_C|CPUFLAG_X); 
          else
            CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
SHOW_OPCODE(",D%d\n",reg_field);
          return;
        break;
      }
    }
SHOW_OPCODE(",D%d\n",reg_field);
  }
  else
  {
    /* <ea> - Dn -> <ea> */
    CPU.lastresult = read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, size);
    CPU.oldvalue = CPU.lastresult;
#ifdef DEBUGGING
switch (size)
{
  case 0: /* BYTE */
    SHOW_OPCODE(".b D%d,",reg_field);
  break;

  case 1: /* WORD */
    SHOW_OPCODE(".w D%d,",reg_field);
  break;

  case 2: /* LONG */
    SHOW_OPCODE(".l D%d,",reg_field);
  break;
}  
#endif
    CPU.subvalue    = CPU.D[reg_field];
    CPU.lastresult -= CPU.subvalue;

    if (0x8 != (inst & 0x38))
    {
      /* not SUBA! SUBA does not affect the ccr */
      CPU.lastop = OP_TYPE4|size;

      switch (size)
      {
        case 0x0:
          if ((UBYTE)CPU.lastresult > (UBYTE)CPU.oldvalue)
            CPU.SR |= (CPUFLAG_C|CPUFLAG_X); 
          else
            CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
        break;
          
        case 1:
          if ((UWORD)CPU.lastresult > (UWORD)CPU.oldvalue)
            CPU.SR |= (CPUFLAG_C|CPUFLAG_X); 
          else
            CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
        break;
          
        case 2:
          if ((ULONG)CPU.lastresult > (ULONG)CPU.oldvalue)
            CPU.SR |= (CPUFLAG_C|CPUFLAG_X); 
          else
            CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
        break;
      }  
    } 

    write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, size);
SHOW_OPCODE("\n");
  }
}


void Group10(UWORD inst)
{
  SHOW_OPCODE("Entering %s\n",__FUNCTION__);
}


/* EOR, CMP */
void Group11(UWORD inst)
{
  UBYTE size = (inst >> 6) & 0x03;
  ULONG val;
  UBYTE reg_field = (inst >> 9) & 0x07;


  switch ((inst >> 8) & 1)
  {
    case 0: /* CMP */
      switch ((inst >> 6) & 0x07)
      {
        case 0x00: /* BYTE */
SHOW_OPCODE("CMP");
          CPU.lastresult = CPU.D[reg_field];
          CPU.oldvalue   = CPU.lastresult;
          CPU.subvalue   = read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 0);
          CPU.lastresult -= CPU.subvalue;
          CPU.lastop = OP_CMP_BYTE;
SHOW_OPCODE(",D%d\n",reg_field);
          return;
        break;
        
        case 0x01: /* WORD */
SHOW_OPCODE("CMP");
          CPU.lastresult = CPU.D[reg_field];
          CPU.oldvalue   = CPU.lastresult;
          CPU.subvalue   = read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 1);
          CPU.lastresult -= CPU.subvalue;
          CPU.lastop = OP_CMP_WORD;
SHOW_OPCODE(",D%d\n",reg_field);
          return;
        break;
        
        case 0x02: /* LONG */
SHOW_OPCODE("CMP");
          CPU.lastresult = CPU.D[reg_field];
          CPU.oldvalue   = CPU.lastresult;
          CPU.subvalue   = read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 2);
          CPU.lastresult -= CPU.subvalue;
          CPU.lastop = OP_CMP_LONG;
SHOW_OPCODE(",D%d\n",reg_field);
          return;
        break;
        
        case 0x03: /* WORD with sign extension */
SHOW_OPCODE("CMPA");
          CPU.lastresult = CPU.A[reg_field];
          CPU.oldvalue   = CPU.lastresult;
          CPU.subvalue   = (LONG)read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 1);
          CPU.lastresult -= CPU.subvalue;
          CPU.lastop = OP_CMP_LONG;
SHOW_OPCODE(",A%d\n",reg_field);
        break;
        
        case 0x07: /* LONG */
SHOW_OPCODE("CMPA");
          CPU.lastresult = CPU.A[reg_field];
          CPU.oldvalue   = CPU.lastresult;
          CPU.subvalue   = read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 2);
          CPU.lastresult -= CPU.subvalue;
          CPU.lastop = OP_CMP_LONG;
SHOW_OPCODE(",A%d\n",reg_field);
        break;
      }
    break;
    
    case 1: /* <ea> ^ Dn -> <ea> || CMPM */
      if (0x8 == (inst & 0x38))
      {
SHOW_OPCODE("CMPM");
        CPU.subvalue    = read_addressing_mode(0x03, inst & 0x07, size);
        CPU.oldvalue    = CPU.lastresult;
SHOW_OPCODE(",");
        CPU.lastresult  = read_addressing_mode(0x03, reg_field, size);
        CPU.oldvalue    = CPU.lastresult;
        CPU.lastresult -= CPU.subvalue;
        CPU.lastop      = OP_TYPE5|size; /* OP_CMP_???? */
SHOW_OPCODE("\n");
      }
      else
      {
SHOW_OPCODE("EOR.%s D%d",oplength[size],reg_field);
        CPU.lastresult = read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, size);
        CPU.lastresult ^= CPU.D[reg_field];
        CPU.lastop = OP_TYPE1 | size;
        write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, size);
SHOW_OPCODE("\n");
      }
    break;
  }
}

/* AND, EXG, MULS, MULU */
void Group12(UWORD inst)
{
  UBYTE size = (inst >> 6) & 0x03;
  ULONG val;
  UBYTE reg_field = (inst >> 9) & 0x07;
  UBYTE reg_field2;
  
  switch ((inst >> 8) & 1)
  {
    case 0: /* <ea> & Dn -> Dn */ 
SHOW_OPCODE("AND");
      val = read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, size);
      switch (size)
      {
        case 0:
          ((UBYTE *)&CPU.D[reg_field])[L_BYTE] &= (UBYTE)val;
        break;
        
        case 1:
          ((UWORD *)&CPU.D[reg_field])[L_WORD] &= (UWORD)val;
        break;
        
        case 2:
          (ULONG)CPU.D[reg_field] &= (ULONG)val;
        break;
      }
      CPU.lastop = OP_TYPE1 | size;
      CPU.lastresult = CPU.D[reg_field];
SHOW_OPCODE(",D%d\n",reg_field);
    break;
    
    case 1: /* <ea> & Dn -> <ea> ||  EXG  (!!!) */
    
      switch ((inst >> 3) & 0x3f)
      {
        case 0x28: /* EXG Dn,Dn */
          reg_field2 = inst & 0x07;
          val = CPU.D[reg_field];
          CPU.D[reg_field]  = CPU.D[reg_field2];
          CPU.D[reg_field2] = val;
SHOW_OPCODE("EXG D%d,D%d\n",reg_field, reg_field2);
          return;
        break;
        
        case 0x29: /* EXG An,An */
          reg_field2 = inst & 0x07;
          val = CPU.A[reg_field];
          CPU.A[reg_field]  = CPU.A[reg_field2];
          CPU.A[reg_field2] = val;
SHOW_OPCODE("EXG A%d,A%d\n",reg_field, reg_field2);
          return;
        break;
        
        case 0x31: /* Exg An,Dn */
          reg_field2 = inst & 0x07;
          val = CPU.D[reg_field];
          CPU.D[reg_field]  = CPU.A[reg_field2];
          CPU.A[reg_field2] = val;
SHOW_OPCODE("EXG D%d,A%d\n",reg_field, reg_field2);
          return;
        break;
        
        default:
          switch ((inst & 0x1c) >> 6)
          {          
            case 0x7:
              CPU.D[reg_field] = (WORD)((UWORD *)&CPU.D[reg_field])[L_WORD] *
                                 (WORD)read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 1);
              CPU.lastop = OP_MULS_LONG;
              return;
            break;
            
            case 0x03:
              CPU.D[reg_field] = (WORD)((UWORD *)&CPU.D[reg_field])[L_WORD] *
                                 (WORD)read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, 1);
              CPU.lastop = OP_MULU_LONG;
              return;
            break;
            
            default:
            
SHOW_OPCODE("AND");
#ifdef DEBUGGING
switch (size)
{
  case 0:
    SHOW_OPCODE("AND.b D%d,",reg_field);
  break;
  case 1:
    SHOW_OPCODE("AND.w D%d,",reg_field);
  break;
  case 2:
    SHOW_OPCODE("AND.l D%d,",reg_field);
  break;
}
#endif
              CPU.lastresult = read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, size);
              CPU.lastresult &= CPU.D[reg_field];
              CPU.lastop = OP_TYPE1|size;
              write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, size);
SHOW_OPCODE("\n");
              return;
            break;
          }
      }
  }
}

/* ADD, ADDA, ADDX */
void Group13(UWORD inst)
{
  UBYTE reg_field = (inst >> 9) & 0x07;
  UBYTE size = (inst >> 6) & 0x03;
  ULONG val;

SHOW_OPCODE("ADD");

  if (0 == (inst & 0x100))
  {
    if (0 == (inst & 0x30))
    {
      /* ADDX */
      if (0 == (inst & 0x8))
      {
        /* Register operation */
        CPU.oldvalue = CPU.D[reg_field];
        switch(size)
        {
          case 0: /* BYTE */
            CPU.addvalue = CPU.D[inst & 0x07];
            if (CPU.SR & CPUFLAG_X)
              CPU.addvalue += 1;
            ((BYTE *)&CPU.D[reg_field])[L_BYTE] += CPU.addvalue;
            CPU.lastresult = CPU.D[reg_field];
            if ((UBYTE)CPU.lastresult < (UBYTE)CPU.oldvalue)
              CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
            else
             CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
            CPU.lastop = OP_ADD_BYTE;
            return;
          break;
        
          case 1: /* WORD */
            CPU.addvalue = CPU.D[inst & 0x07];
            if (CPU.SR & CPUFLAG_X)
              CPU.addvalue += 1;
            ((WORD *)&CPU.D[reg_field])[L_WORD] += CPU.addvalue;
            CPU.lastresult = CPU.D[reg_field];
            if ((UWORD)CPU.lastresult < (UWORD)CPU.oldvalue)
              CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
            else
             CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
            CPU.lastop = OP_ADD_WORD;
            return;
          break;
        
          case 2: /* LONG */
            CPU.addvalue = CPU.D[inst & 0x07];
            if (CPU.SR & CPUFLAG_X)
              CPU.addvalue += 1;
            CPU.D[reg_field] += CPU.addvalue;
            CPU.lastresult = CPU.D[reg_field];
            if ((ULONG)CPU.lastresult < (ULONG)CPU.oldvalue)
              CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
            else
             CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
            CPU.lastop = OP_ADD_LONG;
            return;
          break;
        }
      }
      else
      {
        /* ADDX -(Ax),-(Ay) */  
SHOW_OPCODE("ADDX");
        CPU.oldvalue = read_addressing_mode_nochange(0x4, reg_field, size);
        CPU.lastresult = CPU.oldvalue;
        CPU.addvalue  += read_addressing_mode(0x4, inst & 0x07, size);
        if (CPU.SR & CPUFLAG_X)
          CPU.addvalue++;
        CPU.lastresult += CPU.addvalue;
SHOW_OPCODE(",");
        write_addressing_mode(0x4, reg_field, CPU.lastresult, size);
        switch (size)
        {
          case 0:
            if ((UBYTE)CPU.lastresult < (UBYTE)CPU.oldvalue)
              CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
            else
             CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
          break;
          
          case 1:
            if ((UWORD)CPU.lastresult < (UWORD)CPU.oldvalue)
              CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
            else
             CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
          break;
          
          case 2:
            if ((ULONG)CPU.lastresult < (ULONG)CPU.oldvalue)
              CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
            else
             CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
          break;
        }
        CPU.lastop = OP_TYPE2|size;
SHOW_OPCODE("\n");
      }
    }
    else
    {
      /* <ea> + Dn -> Dn */
      CPU.oldvalue = CPU.D[reg_field];

      switch (size) 
      {
        case 0: /* BYTE */
          CPU.addvalue = (UBYTE)read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, size);
          ((UBYTE *)&CPU.D[reg_field])[L_BYTE] += CPU.addvalue;
          CPU.lastresult = CPU.D[reg_field];
          CPU.lastop = OP_ADD_BYTE;
          if ((UBYTE)CPU.lastresult < (UBYTE)CPU.oldvalue)
            CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
          else
           CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
SHOW_OPCODE(",D%d\n",reg_field);
          return;
        break;
      
        case 1: /* WORD */
          CPU.addvalue = (UWORD)read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, size);
          ((UWORD *)&CPU.D[reg_field])[L_WORD] += CPU.addvalue;
          CPU.lastresult = CPU.D[reg_field];
          CPU.lastop = OP_ADD_WORD;
          if ((UWORD)CPU.lastresult < (UWORD)CPU.oldvalue)
            CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
          else
           CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
SHOW_OPCODE(",D%d\n",reg_field); 
          return;
        break;
      
        case 2: /* LONG */
          CPU.addvalue = read_addressing_mode((inst >> 3) & 0x07, inst & 0x07, size);
          CPU.D[reg_field] += CPU.addvalue;
          CPU.lastresult = CPU.D[reg_field];
          CPU.lastop = OP_ADD_LONG;
          if ((ULONG)CPU.lastresult < (ULONG)CPU.oldvalue)
            CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
          else
           CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
SHOW_OPCODE(",D%d\n",reg_field);
          return;
        break;
      }
    }
SHOW_OPCODE(",D%d\n",reg_field);
  }
  else
  {
    /* <ea> + Dn -> <ea> */
    UWORD flags = 0;
    CPU.lastresult = read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, size);
    CPU.oldvalue   = CPU.lastresult; 
#ifdef DEBUGGING
switch (size)
{
  case 0: /* BYTE */
    SHOW_OPCODE(".b D%d,",reg_field);
  break;

  case 1: /* WORD */
    SHOW_OPCODE(".w D%d,",reg_field);
  break;

  case 2: /* LONG */
    SHOW_OPCODE(".l D%d,",reg_field);
  break;
}  
#endif
    CPU.addvalue = CPU.D[reg_field];
    CPU.lastresult += CPU.addvalue;

    if (0x8 != (inst & 0x38))
    {
      /* not ADDA! ADDA does not affect the ccr */
      CPU.lastop = OP_TYPE2|size;
      switch (size)
      {
          case 0:
            if ((UBYTE)CPU.lastresult < (UBYTE)CPU.oldvalue)
              CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
            else
              CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
          break;
          
          case 1:
            if ((UWORD)CPU.lastresult < (UWORD)CPU.oldvalue)
              CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
            else
              CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
          break;
          
          case 2:
            if ((ULONG)CPU.lastresult < (ULONG)CPU.oldvalue)
              CPU.SR |= (CPUFLAG_C|CPUFLAG_X);
            else
              CPU.SR &= ~(CPUFLAG_C|CPUFLAG_X);
          break;
      }
    }
    
    write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, size);
SHOW_OPCODE("\n");
  }
}


/* ROL, ROXL, ROR, ROXR, LSL, LSR, ASL, ASR */
void Group14(UWORD inst)
{
  ULONG val;
  UBYTE count, reg_field;
  ULONG mask;
  
  switch ((inst >> 6) & 0x03)
  {
    case 0x00: /* BYTE */
      reg_field = inst & 0x07;
      count = (inst >> 9) & 0x07;
      if (inst & 0x20)
      {
        if (0 == count)
          count = 8;
      }
      else
        count = CPU.D[count] & 7;

      switch ((inst >> 3) & 0x03)
      {
        case 0x0: /* ASL, ASR */
          if (inst & 0x100)
          {
            /* ASL */
            mask = 0x80 >> (count-1);
            if (mask & ((BYTE *)&CPU.D[reg_field])[L_BYTE])
              CPU.SR = CPUFLAG_X|CPUFLAG_C;
            else
              CPU.SR = 0;
            CPU.lastop = OP_ASL_BYTE;
            ((BYTE *)&CPU.D[reg_field])[L_BYTE] = ((BYTE *)&CPU.D[reg_field])[L_BYTE] << count;
          }
          else
          {
            /* ASR */
            mask = 0x1 << (count-1);
            if (mask & ((BYTE *)&CPU.D[reg_field])[L_BYTE])
              CPU.SR = CPUFLAG_X|CPUFLAG_C;
            else
              CPU.SR = 0;
            CPU.lastop = OP_ASR_BYTE;
            ((BYTE *)&CPU.D[reg_field])[L_BYTE] = ((BYTE *)&CPU.D[reg_field])[L_BYTE] >> count;
          } 
        break;
          
        case 0x1: /* LSL, LSR */
          if (inst & 0x100)
          {
            /* LSL */
            mask = 0x80 >> (count-1);
            if (mask & ((BYTE *)&CPU.D[reg_field])[L_BYTE])
              CPU.SR = CPUFLAG_X|CPUFLAG_C;
            else
              CPU.SR = 0;
            CPU.lastop = OP_LSL_BYTE;
            ((UBYTE *)&CPU.D[reg_field])[L_BYTE] = ((UBYTE *)&CPU.D[reg_field])[L_BYTE] << count;
          }
          else
          {
            /* LSR */
            mask = 0x1 << (count-1);
            if (mask & ((BYTE *)&CPU.D[reg_field])[L_BYTE])
              CPU.SR = CPUFLAG_X|CPUFLAG_C;
            else
              CPU.SR = 0;
            CPU.lastop = OP_LSR_BYTE;
            ((UBYTE *)&CPU.D[reg_field])[L_BYTE] = ((UBYTE *)&CPU.D[reg_field])[L_BYTE] >> count;
          } 
        break;
          
        case 0x2: /* ROXL, ROXR */
          if (inst & 0x100)
          {
            /* ROXL */
            ((UBYTE *)&CPU.D[reg_field])[L_BYTE] = ((UBYTE *)&CPU.D[reg_field])[L_BYTE] << count | 
                                                   ((UBYTE *)&CPU.D[reg_field])[L_BYTE] >> (9 - count);

            CPU.lastop = OP_ROXL_BYTE;
          }
          else
          {
            /* ROXR */
            ((UBYTE *)&CPU.D[reg_field])[L_BYTE] = ((UBYTE *)&CPU.D[reg_field])[L_BYTE] >> count | 
                                                   ((UBYTE *)&CPU.D[reg_field])[L_BYTE] << (9 - count);
            CPU.lastop = OP_ROXR_BYTE;
          } 
        break;
          
        case 0x3: /* ROL, ROR */
          if (inst & 0x100)
          {
            /* ROL */
            mask = 0x80 >> (count-1);
            if (mask & ((BYTE *)&CPU.D[reg_field])[L_BYTE])
              CPU.SR |= CPUFLAG_C;
            else
              CPU.SR &= CPUFLAG_X;
            CPU.lastop = OP_ROL_BYTE;
            ((UBYTE *)&CPU.D[reg_field])[L_BYTE] = ((UBYTE *)&CPU.D[reg_field])[L_BYTE] << count | ((UBYTE *)&CPU.D[reg_field])[L_BYTE] >> (8 - count);
          }
          else
          {
            /* ROR */
            mask = 0x1 << (count-1);
            if (mask & ((BYTE *)&CPU.D[reg_field])[L_BYTE])
              CPU.SR |= CPUFLAG_C;
            else
              CPU.SR &= CPUFLAG_X;
            CPU.lastop = OP_ROR_BYTE;
            ((UBYTE *)&CPU.D[reg_field])[L_BYTE] = ((UBYTE *)&CPU.D[reg_field])[L_BYTE] >> count | ((UBYTE *)&CPU.D[reg_field])[L_BYTE] << (8 - count);
          } 
        break;
      }
    break;
    
    case 0x01: /* WORD */
      reg_field = inst & 0x07;
      count = (inst >> 9) & 0x07;
      if (inst & 0x20)
      {
        if (0 == count)
          count = 8;
      }
      else
        count = CPU.D[count] & 15;

      switch ((inst >> 3) & 0x03)
      {
        case 0x0: /* ASL, ASR */
          if (inst & 0x100)
          {
            /* ASL */
            mask = 0x8000 >> (count-1);
            if (mask & ((UWORD *)&CPU.D[reg_field])[L_WORD])
              CPU.SR = CPUFLAG_X|CPUFLAG_C;
            else
              CPU.SR = 0;
            CPU.lastop = OP_ASL_WORD;
            ((WORD *)&CPU.D[reg_field])[L_WORD] = ((WORD *)&CPU.D[reg_field])[L_WORD] << count;
          }
          else
          {
            /* ASR */
            mask = 0x1 << (count-1);
            if (mask & ((WORD *)&CPU.D[reg_field])[L_WORD])
              CPU.SR = CPUFLAG_X|CPUFLAG_C;
            else
              CPU.SR = 0;
            CPU.lastop = OP_ASR_WORD;
            ((WORD *)&CPU.D[reg_field])[L_WORD] = ((WORD *)&CPU.D[reg_field])[L_WORD] >> count;
          } 
        break;
          
        case 0x1: /* LSL, LSR */
          if (inst & 0x100)
          {
            /* LSL */
            mask = 0x8000 >> (count-1);
            if (mask & ((UWORD *)&CPU.D[reg_field])[L_WORD])
              CPU.SR = CPUFLAG_X|CPUFLAG_C;
            else
              CPU.SR = 0;
            CPU.lastop = OP_LSL_WORD;
            (UWORD)CPU.D[reg_field] = (UWORD)CPU.D[reg_field] << count;
          }
          else
          {
            /* LSR */
            mask = 0x1 << (count-1);
            if (mask & ((WORD *)&CPU.D[reg_field])[L_WORD])
              CPU.SR = CPUFLAG_X|CPUFLAG_C;
            else
              CPU.SR = 0;
            CPU.lastop = OP_LSR_WORD;
            (UWORD)CPU.D[reg_field] = (UWORD)CPU.D[reg_field] >> count;
          } 
        break;
          
        case 0x2: /* ROXL, ROXR */
          if (inst & 0x100)
          {
            /* ROXL */
            ((UWORD *)&CPU.D[reg_field])[L_WORD] = (UWORD)CPU.D[reg_field] << count |
                                                   (UWORD)CPU.D[reg_field] >> (17 - count);
 
            CPU.lastop = OP_ROXL_WORD;
          }
          else
          {
            /* ROXR */
            ((UWORD *)&CPU.D[reg_field])[L_WORD] = (UWORD)CPU.D[reg_field] >> count |
                                                   (UWORD)CPU.D[reg_field] << (17 - count);
            CPU.lastop = OP_ROXR_WORD;
          } 
        break;
          
        case 0x3: /* ROL, ROR */
          if (inst & 0x100)
          {
            /* ROL */
            mask = 0x8000 >> (count-1);
            if (mask & ((WORD *)&CPU.D[reg_field])[L_WORD])
              CPU.SR |= CPUFLAG_C;
            else
              CPU.SR &= CPUFLAG_X;
            CPU.lastop = OP_ROL_WORD;
            (UWORD)CPU.D[reg_field] = (UWORD)CPU.D[reg_field] << count | (UWORD)CPU.D[reg_field] >> (16 - count);
          }
          else
          {
            /* ROR */
            mask = 0x1 << (count-1);
            if (mask & ((WORD *)&CPU.D[reg_field])[L_WORD])
              CPU.SR |= CPUFLAG_C;
            else
              CPU.SR &= CPUFLAG_X;
            CPU.lastop = OP_ROR_WORD;
            (UWORD)CPU.D[reg_field] = (UWORD)CPU.D[reg_field] >> count | (UWORD)CPU.D[reg_field] << (16 - count);
          } 
        break;
      }
    break;
    
    case 0x02: /* LONG */
      reg_field = inst & 0x07;
      count = (inst >> 9) & 0x07;
      if (inst & 0x20)
      {
        if (0 == count)
          count = 8;
      }
      else
        count = CPU.D[count] & 31;

      switch ((inst >> 3) & 0x03)
      {
        case 0x0: /* ASL, ASR */
          if (inst & 0x100)
          {
            /* ASL */
            mask = 0x80000000 >> (count-1);
            if (mask & CPU.D[reg_field])
              CPU.SR = CPUFLAG_X|CPUFLAG_C;
            else
              CPU.SR = 0;
            CPU.lastop = OP_ASL_LONG;
            (LONG)CPU.D[reg_field] = (LONG)CPU.D[reg_field] << count;
          }
          else
          {
            /* ASR */
            mask = 0x1 << (count-1);
            if (mask & CPU.D[reg_field])
              CPU.SR = CPUFLAG_X|CPUFLAG_C;
            else
              CPU.SR = 0;
            CPU.lastop = OP_ASR_LONG;
            (LONG)CPU.D[reg_field] = (LONG)CPU.D[reg_field] >> count;
          } 
        break;
          
        case 0x1: /* LSL, LSR */
          if (inst & 0x100)
          {
            /* LSL */
            mask = 0x80000000 >> (count-1);
            if (mask & CPU.D[reg_field])
              CPU.SR = CPUFLAG_X|CPUFLAG_C;
            else
              CPU.SR = 0;
            CPU.lastop = OP_LSL_LONG;
            (ULONG)CPU.D[reg_field] = (ULONG)CPU.D[reg_field] << count;
          }
          else
          {
            /* LSR */
            mask = 0x1 << (count-1);
            if (mask & CPU.D[reg_field])
              CPU.SR = CPUFLAG_X|CPUFLAG_C;
            else
              CPU.SR = 0;
            CPU.lastop = OP_LSR_LONG;
            (ULONG)CPU.D[reg_field] = (ULONG)CPU.D[reg_field] >> count;
          } 
        break;
          
        case 0x2: /* ROXL, ROXR */
          if (inst & 0x100)
          {
            /* ROXL */
            (ULONG)CPU.D[reg_field] = (ULONG)CPU.D[reg_field] << count |
                                      (ULONG)CPU.D[reg_field] >> (33 - count);
          }
          else
          {
            /* ROXR */
            (ULONG)CPU.D[reg_field] = (ULONG)CPU.D[reg_field] >> count |
                                      (ULONG)CPU.D[reg_field] << (33 - count);
          } 
        break;
          
        case 0x3: /* ROL, ROR */
          if (inst & 0x100)
          {
            /* ROL */
            mask = 0x80000000 >> (count-1);
            if (mask & CPU.D[reg_field])
              CPU.SR |= CPUFLAG_C;
            else
              CPU.SR &= CPUFLAG_X;
            CPU.lastop = OP_ROL_LONG;
            (ULONG)CPU.D[reg_field] = (ULONG)CPU.D[reg_field] << count | (ULONG)CPU.D[reg_field] >> (32 - count);
          }
          else
          {
            /* ROR */
            mask = 0x1 << (count-1);
            if (mask & CPU.D[reg_field])
              CPU.SR |= CPUFLAG_C;
            else
              CPU.SR &= CPUFLAG_X;
            CPU.lastop = OP_ROR_LONG;
            (ULONG)CPU.D[reg_field] = (ULONG)CPU.D[reg_field] >> count | (ULONG)CPU.D[reg_field] << (32 - count);
          } 
        break;
      }
    break;
    
    case 0x03: /* MEMORY SHIFTS: ASL <ea>, ROL <ea> etc. */
      CPU.lastresult = read_addressing_mode_nochange((inst >> 3) & 0x07, inst & 0x07, 1 /* WORD only !*/);
      switch ((inst >> 9) & 0x03)
      {
        case 0x0: /* ASL, ASR */
          if (inst & 0x100)
          {
            /* ASL */
            if (CPU.lastresult & 0x8000)
              CPU.SR = CPUFLAG_X|CPUFLAG_C;
            else
              CPU.SR = 0;
            CPU.lastop = OP_ASL_WORD;

            (WORD)CPU.lastresult <<= 1;
          }
          else
          {
            /* ASR */
            if (CPU.lastresult & 1)
              CPU.SR = CPUFLAG_X|CPUFLAG_C;
            else
              CPU.SR = 0;
            CPU.lastop = OP_ASR_WORD;

            (WORD)CPU.lastresult >>= 1;
          }
        break;
        
        case 0x1: /* LSL, LSR */
          if (inst & 0x100)
          {
            /* LSL */
            if (CPU.lastresult & 0x8000)
              CPU.SR = CPUFLAG_X|CPUFLAG_C;
            else
              CPU.SR = 0;
            CPU.lastop = OP_LSL_WORD;

            (UWORD) CPU.lastresult <<= 1;
          }
          else
          {
            /* LSR */
            if (CPU.lastresult & 1)
              CPU.SR = CPUFLAG_X|CPUFLAG_C;
            else
              CPU.SR = 0;
            CPU.lastop = OP_LSR_WORD;
            
            (UWORD) CPU.lastresult >>= 1;
          }
        break;

        case 0x2: /* ROXL, ROXR */
          if (inst & 0x100)
          {
            /* ROXL */
            CPU.lastresult = ((UWORD)CPU.lastresult) << 1 |
                             ((UWORD)CPU.lastresult) >> 16;
            CPU.lastop = OP_ROXL_WORD;
          }
          else
          {
            /* ROXR */
            CPU.lastresult = ((UWORD)CPU.lastresult) >> 1 |
                             ((UWORD)CPU.lastresult) << 16;
            CPU.lastop = OP_ROXR_WORD;
          }
        break;
        
        case 0x3: /* ROL, ROR */
          if (inst & 0x100)
          {
            /* ROL */
            if (CPU.lastresult & 0x8000)
              CPU.SR |= CPUFLAG_C;
            else
              CPU.SR &= CPUFLAG_X;
            CPU.lastop = OP_ROL_WORD;
            CPU.lastresult = (((WORD)CPU.lastresult) << 1) | ((CPU.lastresult & 0x8000) >> 15);
          }
          else
          {
            /* ROR */
            if (1 & CPU.lastresult)
              CPU.SR |= CPUFLAG_C;
            else
              CPU.SR &= CPUFLAG_X;
            CPU.lastop = OP_ROR_LONG;
            CPU.lastresult = (((WORD)CPU.lastresult) >> 1) | ((CPU.lastresult & 0x0001) << 15);
          }
        break;
        write_addressing_mode((inst >> 3) & 0x07, inst & 0x07, CPU.lastresult, 1);
      }    
    break;
  }
}


void Group15(UWORD inst)
{
}


static int (* Grouptable[])()=
{
  (void *)&Group0,
  (void *)&Group1,
  (void *)&Group2,
  (void *)&Group3,
  (void *)&Group4,
  (void *)&Group5,
  (void *)&Group6,
  (void *)&Group7,
  (void *)&Group8,
  (void *)&Group9,
  (void *)&Group10,
  (void *)&Group11,
  (void *)&Group12,
  (void *)&Group13,
  (void *)&Group14,
  (void *)&Group15
};

void initialize(UBYTE * stack, ULONG stacksize)
{
  /* reset CPU to a state such that it can execute a program */
  memset(&CPU, 0x0, sizeof(CPU));

  /* the cpu needs to know of a stack... */
  CPU.A[7] = (stack+stacksize-4);

}


void dumpcpu()
{
  calc_ccr();
  SHOW_CPU("D0: %8x  D1: %8x  D2: %8x  D3: %8x\n",(int)CPU.D[0],(int)CPU.D[1],(int)CPU.D[2],(int)CPU.D[3]);
  SHOW_CPU("D4: %8x  D5: %8x  D6: %8x  D7: %8x\n",(int)CPU.D[4],(int)CPU.D[5],(int)CPU.D[6],(int)CPU.D[7]);
  SHOW_CPU("A0: %8x  A1: %8x  A2: %8x  A3: %8x\n",(int)CPU.A[0],(int)CPU.A[1],(int)CPU.A[2],(int)CPU.A[3]);
  SHOW_CPU("A4: %8x  A5: %8x  A6: %8x  A7: %8x\n",(int)CPU.A[4],(int)CPU.A[5],(int)CPU.A[6],(int)CPU.A[7]);
  SHOW_CPU("PC: %8x      ",CPU.PC);
  SHOW_CPU("X=%1d N=%1d Z=%1d V=%1d C=%1d\n",(CPU.SR&CPUFLAG_X)?1:0,
                                             (CPU.SR&CPUFLAG_N)?1:0,
                                             (CPU.SR&CPUFLAG_Z)?1:0,
                                             (CPU.SR&CPUFLAG_V)?1:0,
                                             (CPU.SR&CPUFLAG_C)?1:0);
  SHOW_CPU("\n");
}


VOID Emulate(BPTR program, BOOL verbose, BOOL singlestep)
{
  CPU.PC = (ULONG)BADDR(program)+4;
  if (verbose)
    printf("Emulating program at %x\n",(int)BADDR(program));
  
  while (FALSE == end)
  {
    UWORD inst = *(UWORD *)CPU.PC;

    inst = SWAP_WORD(inst);

    if (args[ARG_DUMPCPU])
      kprintf("Found instruction code word %x at %x\n",inst,CPU.PC);

    CPU.instsize = 2;
    Grouptable[(inst >> 12)](inst);
    
    if (0 == CPU.PC)
      break;
    
    CPU.PC += CPU.instsize;
    
    if (args[ARG_DUMPCPU])
        dumpcpu();
    
    if (singlestep)
    {
      char c;
      scanf("%c",&c);
    }
  }
}



int main (int argc, char **argv)
{
  struct RDArgs *rda;

  rda = ReadArgs(TEMPLATE, args, NULL);
  if (rda)
  {
    struct DosLibrary * DOSBase;

    printf("Welcome to m68kemul. (SysBase at %p)\n",SysBase);

    DOSBase = (struct DosLibrary *)OpenLibrary("dos.library",0);
    if (NULL != DOSBase)
    {
      BPTR program;
      UBYTE * stack = AllocMem(STACKSIZE, MEMF_CLEAR);
      
      if (stack)
      {
        initialize(stack, STACKSIZE);
        program = LoadSeg((STRPTR)args[ARG_FILE]);
      
        if (NULL != program)
        {
          Emulate(program, TRUE, args[ARG_STEP]);
          UnLoadSeg(program);
        }
        else
          printf("Could not load program!\n");
        
        FreeMem(stack, STACKSIZE);
      }
    } 
  }
  
  return 0;
}