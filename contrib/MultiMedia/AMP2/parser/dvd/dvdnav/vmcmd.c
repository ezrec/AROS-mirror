/*
 * Copyright (C) 2000, 2001 Martin Norbäck, Håkan Hjort
 * 
 * This file is part of libdvdnav, a DVD navigation library. It is modified
 * from a file originally part of the Ogle DVD player.
 * 
 * libdvdnav is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * libdvdnav is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
 *
 * $Id$
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>
#include <assert.h>

#include "vmcmd.h"


/*  freebsd compatibility */
#ifndef PRIu8
#define PRIu8 "d"
#endif

/*  freebsd compatibility */
#ifndef PRIu16
#define PRIu16 "d"
#endif

static const char *cmp_op_table[] = {
  NULL, "&", "==", "!=", ">=", ">", "<=", "<"
};
static const char *set_op_table[] = {
  NULL, "=", "<->", "+=", "-=", "*=", "/=", "%=", "rnd", "&=", "|=", "^="
};

static const char *link_table[] = {
  "LinkNoLink",  "LinkTopC",    "LinkNextC",   "LinkPrevC",
  NULL,          "LinkTopPG",   "LinkNextPG",  "LinkPrevPG",
  NULL,          "LinkTopPGC",  "LinkNextPGC", "LinkPrevPGC",
  "LinkGoUpPGC", "LinkTailPGC", NULL,          NULL,
  "RSM"
};

static const char *system_reg_table[] = {
  "Menu Description Language Code",
  "Audio Stream Number",
  "Sub-picture Stream Number",
  "Angle Number",
  "Title Track Number",
  "VTS Title Track Number",
  "VTS PGC Number",
  "PTT Number for One_Sequential_PGC_Title",
  "Highlighted Button Number",
  "Navigation Timer",
  "Title PGC Number for Navigation Timer",
  "Audio Mixing Mode for Karaoke",
  "Country Code for Parental Management",
  "Parental Level",
  "Player Configurations for Video",
  "Player Configurations for Audio",
  "Initial Language Code for Audio",
  "Initial Language Code Extension for Audio",
  "Initial Language Code for Sub-picture",
  "Initial Language Code Extension for Sub-picture",
  "Player Regional Code",
  "Reserved 21",
  "Reserved 22",
  "Reserved 23"
};

static const char *system_reg_abbr_table[] = {
  NULL,
  "ASTN",
  "SPSTN",
  "AGLN",
  "TTN",
  "VTS_TTN",
  "TT_PGCN",
  "PTTN",
  "HL_BTNN",
  "NVTMR",
  "NV_PGCN",
  NULL,
  "CC_PLT",
  "PLT",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};
    
static void print_system_reg(uint16_t reg) {
  if(reg < sizeof(system_reg_abbr_table) / sizeof(char *))
    fprintf(stderr, "%s (SRPM:%d)", system_reg_table[reg], reg);
  else
    fprintf(stderr, " WARNING: Unknown system register ( reg=%d ) ", reg);
}

static void print_reg(uint8_t reg) {
  if(reg & 0x80)
    print_system_reg(reg & 0x7f);
  else
    if(reg < 16)
      fprintf(stderr, "g[%" PRIu8 "]", reg);
    else
      fprintf(stderr, " WARNING: Unknown general register ");
}

static void print_cmp_op(uint8_t op) {
  if(op < sizeof(cmp_op_table) / sizeof(char *) && cmp_op_table[op] != NULL)
    fprintf(stderr, " %s ", cmp_op_table[op]);
  else
    fprintf(stderr, " WARNING: Unknown compare op ");
}

static void print_set_op(uint8_t op) {
  if(op < sizeof(set_op_table) / sizeof(char *) && set_op_table[op] != NULL)
    fprintf(stderr, " %s ", set_op_table[op]);
  else
    fprintf(stderr, " WARNING: Unknown set op ");
}

static void print_reg_or_data(command_t* command, int immediate, int byte) {
  if(immediate) {
    int i = vm_getbits(command, (byte*8), 16);
    
    fprintf(stderr, "0x%x", i);
    if(isprint(i & 0xff) && isprint((i>>8) & 0xff))
      fprintf(stderr, " (\"%c%c\")", (char)((i>>8) & 0xff), (char)(i & 0xff));
  } else {
    print_reg(vm_getbits(command, ((byte + 1)*8), 8));
  }
}

static void print_reg_or_data_2(command_t* command, int immediate, int byte) {
  if(immediate)
    fprintf(stderr, "0x%x", vm_getbits(command, ((byte*8)+1), 7));
  else
    fprintf(stderr, "g[%" PRIu8 "]", vm_getbits(command, ((byte*8)+4), 4));
}

static void print_if_version_1(command_t* command) {
  uint8_t op = vm_getbits(command, 9, 3);
  
  if(op) {
    fprintf(stderr, "if (");
    print_reg(vm_getbits(command,24,8));
    print_cmp_op(op);
    print_reg_or_data(command, vm_getbits(command, 8,1), 4);
    fprintf(stderr, ") ");
  }
}

static void print_if_version_2(command_t* command) {
  uint8_t op = vm_getbits(command, 9, 3);
  
  if(op) {
    fprintf(stderr, "if (");
    print_reg(vm_getbits(command, 48, 8));
    print_cmp_op(op);
    print_reg(vm_getbits(command, 56, 8));
    fprintf(stderr, ") ");
  }
}

static void print_if_version_3(command_t* command) {
  uint8_t op = vm_getbits(command, 9, 3);
  
  if(op) {
    fprintf(stderr, "if (");
    print_reg(vm_getbits(command, 20, 4));
    print_cmp_op(op);
    print_reg_or_data(command, vm_getbits(command, 8, 1), 6);
    fprintf(stderr, ") ");
  }
}

static void print_if_version_4(command_t* command) {
  uint8_t op = vm_getbits(command, 9, 3);
  
  if(op) {
    fprintf(stderr, "if (");
    print_reg(vm_getbits(command, 12, 4));
    print_cmp_op(op);
    print_reg_or_data(command, vm_getbits(command, 8, 1), 4);
    fprintf(stderr, ") ");
  }
}

static void print_special_instruction(command_t* command) {
  uint8_t op = vm_getbits(command, 12, 4);
  
  switch(op) {
    case 0: /*  NOP */
      fprintf(stderr, "Nop");
      break;
    case 1: /*  Goto line */
      fprintf(stderr, "Goto %" PRIu8, vm_getbits(command, 56, 8));
      break;
    case 2: /*  Break */
      fprintf(stderr, "Break");
      break;
    case 3: /*  Parental level */
      fprintf(stderr, "SetTmpPML %" PRIu8 ", Goto %" PRIu8, 
	      vm_getbits(command, 52, 4), vm_getbits(command, 56, 8));
      break;
    default:
      fprintf(stderr, "WARNING: Unknown special instruction (%i)", 
	      vm_getbits(command, 12, 4));
  }
}

static void print_linksub_instruction(command_t* command) {
  int linkop = vm_getbits(command, 59, 5);
  int button = vm_getbits(command, 48, 6);
  
  if(linkop < sizeof(link_table)/sizeof(char *) && link_table[linkop] != NULL)
    fprintf(stderr, "%s (button %" PRIu8 ")", link_table[linkop], button);
  else
    fprintf(stderr, "WARNING: Unknown linksub instruction (%i)", linkop);
}

static void print_link_instruction(command_t* command, int optional) {
  uint8_t op = vm_getbits(command, 12, 4);
  
  if(optional && op)
    fprintf(stderr, ", ");
  
  switch(op) {
    case 0:
      if(!optional)
      fprintf(stderr, "WARNING: NOP (link)!");
      break;
    case 1:
      print_linksub_instruction(command);
      break;
    case 4:
      fprintf(stderr, "LinkPGCN %" PRIu16, vm_getbits(command, 49, 15));
      break;
    case 5:
      fprintf(stderr, "LinkPTT %" PRIu16 " (button %" PRIu8 ")", 
	      vm_getbits(command, 54, 10), vm_getbits(command, 48, 6));
      break;
    case 6:
      fprintf(stderr, "LinkPGN %" PRIu8 " (button %" PRIu8 ")", 
	      vm_getbits(command, 57, 7), vm_getbits(command, 48, 6));
      break;
    case 7:
      fprintf(stderr, "LinkCN %" PRIu8 " (button %" PRIu8 ")", 
	      vm_getbits(command, 56, 8), vm_getbits(command, 48, 6));
      break;
    default:
      fprintf(stderr, "WARNING: Unknown link instruction");
  }
}

static void print_jump_instruction(command_t* command) {
  switch(vm_getbits(command, 12, 4)) {
    case 1:
      fprintf(stderr, "Exit");
      break;
    case 2:
      fprintf(stderr, "JumpTT %" PRIu8, vm_getbits(command, 41, 7));
      break;
    case 3:
      fprintf(stderr, "JumpVTS_TT %" PRIu8, vm_getbits(command, 41, 7));
      break;
    case 5:
      fprintf(stderr, "JumpVTS_PTT %" PRIu8 ":%" PRIu16, 
	      vm_getbits(command, 41, 7), vm_getbits(command, 22, 10));
      break;
    case 6:
      switch(vm_getbits(command, 40, 2)) {
        case 0:
          fprintf(stderr, "JumpSS FP");
          break;
        case 1:
          fprintf(stderr, "JumpSS VMGM (menu %" PRIu8 ")", vm_getbits(command, 44, 4));
          break;
        case 2:
          fprintf(stderr, "JumpSS VTSM (vts %" PRIu8 ", title %" PRIu8 
		  ", menu %" PRIu8 ")", vm_getbits(command, 32, 8), vm_getbits(command, 24, 8), vm_getbits(command, 44, 4));
          break;
        case 3:
          fprintf(stderr, "JumpSS VMGM (pgc %" PRIu8 ")", vm_getbits(command, 17, 15));
          break;
        }
      break;
    case 8:
      switch(vm_getbits(command, 40, 2)) {
        case 0:
          fprintf(stderr, "CallSS FP (rsm_cell %" PRIu8 ")",
              vm_getbits(command, 32, 8));
          break;
        case 1:
          fprintf(stderr, "CallSS VMGM (menu %" PRIu8 
		  ", rsm_cell %" PRIu8 ")", vm_getbits(command, 44, 4), vm_getbits(command, 32, 8));
          break;
        case 2:
          fprintf(stderr, "CallSS VTSM (menu %" PRIu8 
		  ", rsm_cell %" PRIu8 ")", vm_getbits(command, 44, 4), vm_getbits(command, 32, 8));
          break;
        case 3:
          fprintf(stderr, "CallSS VMGM (pgc %" PRIu8 ", rsm_cell %" PRIu8 ")", 
		  vm_getbits(command, 17, 15), vm_getbits(command, 32, 8));
          break;
      }
      break;
    default:
      fprintf(stderr, "WARNING: Unknown Jump/Call instruction");
  }
}

static void print_system_set(command_t* command) {
  int i;
  
  switch(vm_getbits(command, 4, 4)) {
    case 1: /*  Set system reg 1 &| 2 &| 3 (Audio, Subp. Angle) */
      for(i = 1; i <= 3; i++) {
        if(vm_getbits(command, ((2+i)*8), 1)) {
          print_system_reg(i);
          fprintf(stderr, " = ");
          print_reg_or_data_2(command, vm_getbits(command, 3, 1), 2 + i);
          fprintf(stderr, " ");
        }
      }
      break;
    case 2: /*  Set system reg 9 & 10 (Navigation timer, Title PGC number) */
      print_system_reg(9);
      fprintf(stderr, " = ");
      print_reg_or_data(command, vm_getbits(command, 3, 1), 2);
      fprintf(stderr, " ");
      print_system_reg(10);
      fprintf(stderr, " = %" PRIu8, vm_getbits(command, 40, 8)); /*  ?? */
      break;
    case 3: /*  Mode: Counter / Register + Set */
      fprintf(stderr, "SetMode ");
      if(vm_getbits(command, 40, 1))
	fprintf(stderr, "Counter ");
      else
	fprintf(stderr, "Register ");
      print_reg(vm_getbits(command, 44, 4));
      print_set_op(0x1); /*  '=' */
      print_reg_or_data(command, vm_getbits(command, 3, 1), 2);
      break;
    case 6: /*  Set system reg 8 (Highlighted button) */
      print_system_reg(8);
      if(vm_getbits(command, 3, 1)) /*  immediate */
        fprintf(stderr, " = 0x%x (button no %d)", vm_getbits(command, 32, 16), vm_getbits(command, 32, 6));
      else
        fprintf(stderr, " = g[%" PRIu8 "]", vm_getbits(command, 44, 4));
      break;
    default:
      fprintf(stderr, "WARNING: Unknown system set instruction (%i)", 
	      vm_getbits(command, 4, 4));
  }
}

static void print_set_version_1(command_t* command) {
  uint8_t set_op = vm_getbits(command, 4, 4);
  
  if(set_op) {
    print_reg(vm_getbits(command, 24, 8));  /* FIXME: This is different from decoder.c!!!  */
    print_set_op(set_op);
    print_reg_or_data(command, vm_getbits(command, 3, 1), 4);
  } else {
    fprintf(stderr, "NOP");
  }
}

static void print_set_version_2(command_t* command) {
  uint8_t set_op = vm_getbits(command, 4, 4);
  
  if(set_op) {
    print_reg(vm_getbits(command, 12, 4));
    print_set_op(set_op);
    print_reg_or_data(command, vm_getbits(command, 3, 1), 2);
  } else {
    fprintf(stderr, "NOP");
  }
}

void vmPrint_mnemonic(vm_cmd_t *vm_command)  {
  command_t command;
  command.instruction =( (uint64_t) vm_command->bytes[0] << 56 ) |
        ( (uint64_t) vm_command->bytes[1] << 48 ) |
        ( (uint64_t) vm_command->bytes[2] << 40 ) |
        ( (uint64_t) vm_command->bytes[3] << 32 ) |
        ( (uint64_t) vm_command->bytes[4] << 24 ) |
        ( (uint64_t) vm_command->bytes[5] << 16 ) |
        ( (uint64_t) vm_command->bytes[6] <<  8 ) |
          (uint64_t) vm_command->bytes[7] ;
  command.examined = 0; 

  switch(vm_getbits(&command,0,3)) { /* three first bits */
    case 0: /*  Special instructions */
      print_if_version_1(&command);
      print_special_instruction(&command);
      break;
    case 1: /*  Jump/Call or Link instructions */
      if(vm_getbits(&command,3,1)) {
        print_if_version_2(&command);
        print_jump_instruction(&command);
      } else {
        print_if_version_1(&command);
        print_link_instruction(&command, 0); /*  must be pressent */
      }
      break;
    case 2: /*  Set System Parameters instructions */
      print_if_version_2(&command);
      print_system_set(&command);
      print_link_instruction(&command, 1); /*  either 'if' or 'link' */
      break;
    case 3: /*  Set General Parameters instructions */
      print_if_version_3(&command);
      print_set_version_1(&command);
      print_link_instruction(&command, 1); /*  either 'if' or 'link' */
      break;
    case 4: /*  Set, Compare -> LinkSub instructions */
      print_set_version_2(&command);
      fprintf(stderr, ", ");
      print_if_version_4(&command);
      print_linksub_instruction(&command);
      break;
    case 5: /*  Compare -> (Set and LinkSub) instructions */
      print_if_version_4(&command);
      fprintf(stderr, "{ ");
      print_set_version_2(&command);
      fprintf(stderr, ", ");
      print_linksub_instruction(&command);
      fprintf(stderr, " }");
      break;
    case 6: /*  Compare -> Set, always LinkSub instructions */
      print_if_version_4(&command);
      fprintf(stderr, "{ ");
      print_set_version_2(&command);
      fprintf(stderr, " } ");
      print_linksub_instruction(&command);
      break;
    default:
      fprintf(stderr, "WARNING: Unknown instruction type (%i)", vm_getbits(&command, 0, 3));
  }
  /*  Check if there still are bits set that were not examined */
  
  if(command.instruction & ~ command.examined) {
    fprintf(stderr, " libdvdnav: vmcmd.c: [WARNING, unknown bits:");
    fprintf(stderr, " %08llx", (command.instruction & ~ command.examined) );
    fprintf(stderr, "]");
  }
}

void vmPrint_CMD(int row, vm_cmd_t *vm_command) {
  int i;

  fprintf(stderr, "(%03d) ", row + 1);
  for(i = 0; i < 8; i++)
    fprintf(stderr, "%02x ", vm_command->bytes[i]);
  fprintf(stderr, "| ");

  vmPrint_mnemonic(vm_command);
  fprintf(stderr, "\n");
}


