/*
**  LZX Extract in (supposedly) portable C.
**
**  Based on unlzx 1.0 by David Tritscher.
**  Rewritten by Oliver Gantert <lucyg@t-online.de>
**
**  Compiled with vbcc/Amiga and lcc/Win32
*/

#ifndef unlzx_unlzx_h
#define UNLZX_VERSION "2.16"
#define UNLZX_VERDATE "14.11.2000"
/*
#define UNLZX_DEBUG
#define UNLZX_TIME
*/

#ifndef AMIGA
# include <windows.h>
#endif
#ifdef __AMIGAOS__
#include <exec/types.h>
LONG mkdir(STRPTR path, UWORD perm);
#else
#include <sys/stat.h>
#endif
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef UNLZX_TIME
#include <time.h>
#endif

struct filename_node
{
  struct filename_node * next;
  unsigned long length;
  unsigned long crc;
  unsigned char filename[256];
};

struct UnLZX
{
  unsigned char match_pattern[256];
  signed long use_outdir;
  unsigned char output_dir[768];
  unsigned char work_buffer[1024];

  signed long mode;

  unsigned char info_header[10];
  unsigned char archive_header[32];
  unsigned char header_filename[256];
  unsigned char header_comment[256];

  unsigned long pack_size;
  unsigned long unpack_size;

  unsigned long crc;
  unsigned long year;
  unsigned long month;
  unsigned long day;
  unsigned long hour;
  unsigned long minute;
  unsigned long second;
  unsigned char attributes;
  unsigned char pack_mode;

  struct filename_node *filename_list;

  unsigned char read_buffer[16384];
  unsigned char decrunch_buffer[66560];

  unsigned char *source;
  unsigned char *destination;
  unsigned char *source_end;
  unsigned char *destination_end;

  unsigned long decrunch_method;
  unsigned long decrunch_length;
  unsigned long last_offset;
  unsigned long global_control;
  signed long global_shift;

  unsigned char offset_len[8];
  unsigned short offset_table[128];
  unsigned char huffman20_len[20];
  unsigned short huffman20_table[96];
  unsigned char literal_len[768];
  unsigned short literal_table[5120];

  unsigned long sum;
};

#define PMATCH_MAXSTRLEN  512    /*  max string length  */

#define make_percent(p,m)   ((p*100)/m)

#endif /* unlzx_unlzx_h */

