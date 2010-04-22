#define BUFFER_SIZE 64

struct StringBuffer
{
    int pos;
    char buf[BUFFER_SIZE];
};

struct DisassemblerObject
{
    disassembler_ftype disasm;
    disassemble_info dinfo;
    struct StringBuffer sbuf;
    struct LibcData libc_data;
};
