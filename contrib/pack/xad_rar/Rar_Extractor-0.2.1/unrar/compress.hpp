
// Rar_Extractor 0.2.1. See unrar_license.txt.

#ifndef _RAR_COMPRESS_
#define _RAR_COMPRESS_

class ComprDataIO;
class PackingFileTable;

#define CODEBUFSIZE     0x4000
#define MAXWINSIZE      0x400000
#define MAXWINMASK      (MAXWINSIZE-1)

#define LOW_DIST_REP_COUNT 16

#define NC 299  /* alphabet = {0, 1, 2, ..., NC - 1} */
#define DC  60
#define LDC 17
#define RC  28
#define HUFF_TABLE_SIZE (NC+DC+RC+LDC)
#define BC  20

#define NC20 298  /* alphabet = {0, 1, 2, ..., NC - 1} */
#define DC20 48
#define RC20 28
#define BC20 19
#define MC20 257

enum {CODE_HUFFMAN,CODE_LZ,CODE_LZ2,CODE_REPEATLZ,CODE_CACHELZ,
      CODE_STARTFILE,CODE_ENDFILE,CODE_VM,CODE_VMDATA};

#endif
