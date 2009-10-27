#include "system_headers.h"

#if defined(__amigaos4__)
  #include <proto/exec.h>
  #ifdef __USE_INLINE__
    #ifdef DebugPrintF
      #undef DebugPrintF
    #endif
  #endif
  #ifndef kprintf
    #define kprintf(format, args...)  ((struct ExecIFace *)((*(struct ExecBase **)4)->MainInterface))->DebugPrintF(format, ## args)
  #endif
#elif defined(__MORPHOS__)
  #include <exec/rawfmt.h>
  #include <proto/exec.h>
  #define KPutFmt(format, args)  VNewRawDoFmt(format, (APTR)RAWFMTFUNC_SERIAL, NULL, args)
  void kprintf(const char *formatString,...);
#else
  void kprintf(const char *formatString,...);
#endif

/* /// "khexdump" */
void khexdump( void *buffer,
               LONG size )
{
    ULONG cnt;
    UBYTE c;
    ULONG addr;

    addr = 0;
    while (size >= 16) {
        UWORD bw[8];
        UBYTE *bb;

        memcpy(&bw[0], buffer, sizeof(UWORD) * 8);
        bb = (UBYTE *)&bw[0];
        kprintf("%08lx: %04lx %04lx %04lx %04lx %04lx %04lx %04lx %04lx  ", addr, bw[0], bw[1], bw[2], bw[3], bw[4], bw[5], bw[6], bw[7]);
        for (cnt = 0; cnt < 16; cnt++) {
            c = bb[cnt];
            kprintf("%lc", isprint(c) ? c : '.');
        }
        kprintf("\n");
        size -= 16;
        addr += 16;
        buffer = (void *)((UBYTE *)buffer + 16);
    }
    if (size >= 14) {
        UWORD bw[8];
        UBYTE *bb;

        memcpy(&bw[0], buffer, sizeof(UWORD) * 8);
        bb = (UBYTE *)&bw[0];
		kprintf("%08lx: %04lx %04lx %04lx %04lx %04lx %04lx %04lx ", addr, bw[0], bw[1], bw[2], bw[3], bw[4], bw[5], bw[6]);
        for (cnt = 14; cnt < size; cnt++) kprintf("%02lx", bb[cnt]);
        for (cnt = size; cnt < 16; cnt++) kprintf("  ");
        kprintf("  ");
        for (cnt = 0; cnt < size; cnt++) {
            c = bb[cnt];
            kprintf("%lc", isprint(c) ? c : '.');
        }
        kprintf("\n");
    } else if (size >= 12) {
        UWORD bw[7];
        UBYTE *bb;

        memcpy(&bw[0], buffer, sizeof(UWORD) * 7);
        bb = (UBYTE *)&bw[0];
		kprintf("%08lx: %04lx %04lx %04lx %04lx %04lx %04lx ", addr, bw[0], bw[1], bw[2], bw[3], bw[4], bw[5]);
        for (cnt = 12; cnt < size; cnt++) kprintf("%02lx", bb[cnt]);
        for (cnt = size; cnt < 16; cnt++) kprintf("  ");
        for (cnt = size/4; cnt < 16/4; cnt++) kprintf(" ");
        kprintf("  ");
        for (cnt = 0; cnt < size; cnt++) {
            c = bb[cnt];
            kprintf("%lc", isprint(c) ? c : '.');
        }
        kprintf("\n");
    } else if (size >= 10) {
        UWORD bw[6];
        UBYTE *bb;

        memcpy(&bw[0], buffer, sizeof(UWORD) * 6);
        bb = (UBYTE *)&bw[0];
		kprintf("%08lx: %04lx %04lx %04lx %04lx %04lx ", addr, bw[0], bw[1], bw[2], bw[3], bw[4]);
        for (cnt = 10; cnt < size; cnt++) kprintf("%02lx", bb[cnt]);
        for (cnt = size; cnt < 16; cnt++) kprintf("  ");
        for (cnt = size/4; cnt < 16/4; cnt++) kprintf(" ");
        kprintf("  ");
        for (cnt = 0; cnt < size; cnt++) {
            c = bb[cnt];
            kprintf("%lc", isprint(c) ? c : '.');
        }
        kprintf("\n");
    } else if (size >= 8) {
        UWORD bw[5];
        UBYTE *bb;

        memcpy(&bw[0], buffer, sizeof(UWORD) * 5);
        bb = (UBYTE *)&bw[0];
        kprintf("%08lx: %04lx %04lx %04lx %04lx ", addr, bw[0], bw[1], bw[2], bw[3]);
        for (cnt = 8; cnt < size; cnt++) kprintf("%02lx", bb[cnt]);
        for (cnt = size; cnt < 12; cnt++) kprintf("  ");
        for (cnt = size/4; cnt < 16/4; cnt++) kprintf(" ");
        kprintf("           ");
        for (cnt = 0; cnt < size; cnt++) {
            c = bb[cnt];
            kprintf("%lc", isprint(c) ? c : '.');
        }
        kprintf("\n");
    } else if (size >= 6) {
        UWORD bw[4];
        UBYTE *bb;

        memcpy(&bw[0], buffer, sizeof(UWORD) * 4);
        bb = (UBYTE *)&bw[0];
        kprintf("%08lx: %04lx %04lx %04lx ", addr, bw[0], bw[1], bw[2]);
        for (cnt = 6; cnt < size; cnt++) kprintf("%02lx", bb[cnt]);
        for (cnt = size; cnt < 8; cnt++) kprintf("  ");
        for (cnt = size/4; cnt < 16/4; cnt++) kprintf(" ");
        kprintf("                   ");
        for (cnt = 0; cnt < size; cnt++) {
            c = bb[cnt];
            kprintf("%lc", isprint(c) ? c : '.');
        }
        kprintf("\n");
    } else if (size >= 4) {
        UWORD bw[3];
        UBYTE *bb;

        memcpy(&bw[0], buffer, sizeof(UWORD) * 3);
        bb = (UBYTE *)&bw[0];
        kprintf("%08lx: %04lx %04lx ", addr, bw[0], bw[1]);
        for (cnt = 4; cnt < size; cnt++) kprintf("%02lx", bb[cnt]);
        for (cnt = size; cnt < 8; cnt++) kprintf("  ");
        for (cnt = size/4; cnt < 16/4; cnt++) kprintf(" ");
        kprintf("                    ");
        for (cnt = 0; cnt < size; cnt++) {
            c = bb[cnt];
            kprintf("%lc", isprint(c) ? c : '.');
        }
        kprintf("\n");
    } else if (size >= 2) {
        UWORD bw[2];
        UBYTE *bb;

        memcpy(&bw[0], buffer, sizeof(UWORD) * 2);
        bb = (UBYTE *)&bw[0];
        kprintf("%08lx: %04lx ", addr, bw[0]);
        for (cnt = 2; cnt < size; cnt++) kprintf("%02lx", bb[cnt]);
        for (cnt = size; cnt < 8; cnt++) kprintf("  ");
        for (cnt = size/4; cnt < 16/4; cnt++) kprintf(" ");
        kprintf("                    ");
        for (cnt = 0; cnt < size; cnt++) {
            c = bb[cnt];
            kprintf("%lc", IsPrint(NULL, c) ? c : '.');
        }
        kprintf("\n");
    } else if (size > 0) {
        UWORD bw[1];
        UBYTE *bb;

        memcpy(&bw[0], buffer, sizeof(UWORD) * 1);
        bb = (UBYTE *)&bw[0];
        kprintf("%08lx: ", addr);
        for (cnt = 0; cnt < size; cnt++) kprintf("%02lx", bb[cnt]);
        for (cnt = size; cnt < 4; cnt++) kprintf("  ");
        for (cnt = size/4; cnt < 16/4; cnt++) kprintf(" ");
        kprintf("                             ");
        for (cnt = 0; cnt < size; cnt++) {
            c = bb[cnt];
            kprintf("%lc", isprint(c) ? c : '.');
        }
        kprintf("\n");
    }
}
/* \\\ */

/* /// "fhexdump" */
void fhexdump( BPTR fh,
               void *buffer,
               LONG size )
{
    ULONG cnt;
    UBYTE c;
    ULONG addr;

    addr = 0;
    while (size >= 16) {
        UWORD bw[8];
        UBYTE *bb;

        memcpy(&bw[0], buffer, sizeof(UWORD) * 8);
        bb = (UBYTE *)&bw[0];
        FPrintf(fh, "%08lx: %04lx %04lx %04lx %04lx %04lx %04lx %04lx %04lx  ", addr, bw[0], bw[1], bw[2], bw[3], bw[4], bw[5], bw[6], bw[7]);
        for (cnt = 0; cnt < 16; cnt++) {
            c = bb[cnt];
            FPrintf(fh, "%lc", isprint(c) ? c : '.');
        }
        FPrintf(fh, "\n");
        size -= 16;
        addr += 16;
        buffer = (void *)((UBYTE *)buffer + 16);
    }
    if (size >= 14) {
        UWORD bw[8];
        UBYTE *bb;

        memcpy(&bw[0], buffer, sizeof(UWORD) * 8);
        bb = (UBYTE *)&bw[0];
		FPrintf(fh, "%08lx: %04lx %04lx %04lx %04lx %04lx %04lx %04lx ", addr, bw[0], bw[1], bw[2], bw[3], bw[4], bw[5], bw[6]);
        for (cnt = 14; cnt < size; cnt++) FPrintf(fh, "%02lx", bb[cnt]);
        for (cnt = size; cnt < 16; cnt++) FPrintf(fh, "  ");
        FPrintf(fh, "  ");
        for (cnt = 0; cnt < size; cnt++) {
            c = bb[cnt];
            FPrintf(fh, "%lc", isprint(c) ? c : '.');
        }
        FPrintf(fh, "\n");
    } else if (size >= 12) {
        UWORD bw[7];
        UBYTE *bb;

        memcpy(&bw[0], buffer, sizeof(UWORD) * 7);
        bb = (UBYTE *)&bw[0];
		FPrintf(fh, "%08lx: %04lx %04lx %04lx %04lx %04lx %04lx ", addr, bw[0], bw[1], bw[2], bw[3], bw[4], bw[5]);
        for (cnt = 12; cnt < size; cnt++) FPrintf(fh, "%02lx", bb[cnt]);
        for (cnt = size; cnt < 16; cnt++) FPrintf(fh, "  ");
        for (cnt = size/4; cnt < 16/4; cnt++) FPrintf(fh, " ");
        FPrintf(fh, "  ");
        for (cnt = 0; cnt < size; cnt++) {
            c = bb[cnt];
            FPrintf(fh, "%lc", isprint(c) ? c : '.');
        }
        FPrintf(fh, "\n");
    } else if (size >= 10) {
        UWORD bw[6];
        UBYTE *bb;

        memcpy(&bw[0], buffer, sizeof(UWORD) * 6);
        bb = (UBYTE *)&bw[0];
		FPrintf(fh, "%08lx: %04lx %04lx %04lx %04lx %04lx ", addr, bw[0], bw[1], bw[2], bw[3], bw[4]);
        for (cnt = 10; cnt < size; cnt++) FPrintf(fh, "%02lx", bb[cnt]);
        for (cnt = size; cnt < 16; cnt++) FPrintf(fh, "  ");
        for (cnt = size/4; cnt < 16/4; cnt++) FPrintf(fh, " ");
        FPrintf(fh, "  ");
        for (cnt = 0; cnt < size; cnt++) {
            c = bb[cnt];
            FPrintf(fh, "%lc", isprint(c) ? c : '.');
        }
        FPrintf(fh, "\n");
    } else if (size >= 8) {
        UWORD bw[5];
        UBYTE *bb;

        memcpy(&bw[0], buffer, sizeof(UWORD) * 5);
        bb = (UBYTE *)&bw[0];
        FPrintf(fh, "%08lx: %04lx %04lx %04lx %04lx ", addr, bw[0], bw[1], bw[2], bw[3]);
        for (cnt = 8; cnt < size; cnt++) FPrintf(fh, "%02lx", bb[cnt]);
        for (cnt = size; cnt < 12; cnt++) FPrintf(fh, "  ");
        for (cnt = size/4; cnt < 16/4; cnt++) FPrintf(fh, " ");
        FPrintf(fh, "           ");
        for (cnt = 0; cnt < size; cnt++) {
            c = bb[cnt];
            FPrintf(fh, "%lc", isprint(c) ? c : '.');
        }
        FPrintf(fh, "\n");
    } else if (size >= 6) {
        UWORD bw[4];
        UBYTE *bb;

        memcpy(&bw[0], buffer, sizeof(UWORD) * 4);
        bb = (UBYTE *)&bw[0];
        FPrintf(fh, "%08lx: %04lx %04lx %04lx ", addr, bw[0], bw[1], bw[2]);
        for (cnt = 6; cnt < size; cnt++) FPrintf(fh, "%02lx", bb[cnt]);
        for (cnt = size; cnt < 8; cnt++) FPrintf(fh, "  ");
        for (cnt = size/4; cnt < 16/4; cnt++) FPrintf(fh, " ");
        FPrintf(fh, "                   ");
        for (cnt = 0; cnt < size; cnt++) {
            c = bb[cnt];
            FPrintf(fh, "%lc", isprint(c) ? c : '.');
        }
        FPrintf(fh, "\n");
    } else if (size >= 4) {
        UWORD bw[3];
        UBYTE *bb;

        memcpy(&bw[0], buffer, sizeof(UWORD) * 3);
        bb = (UBYTE *)&bw[0];
        FPrintf(fh, "%08lx: %04lx %04lx ", addr, bw[0], bw[1]);
        for (cnt = 4; cnt < size; cnt++) FPrintf(fh, "%02lx", bb[cnt]);
        for (cnt = size; cnt < 8; cnt++) FPrintf(fh, "  ");
        for (cnt = size/4; cnt < 16/4; cnt++) FPrintf(fh, " ");
        FPrintf(fh, "                    ");
        for (cnt = 0; cnt < size; cnt++) {
            c = bb[cnt];
            FPrintf(fh, "%lc", isprint(c) ? c : '.');
        }
        FPrintf(fh, "\n");
    } else if (size >= 2) {
        UWORD bw[2];
        UBYTE *bb;

        memcpy(&bw[0], buffer, sizeof(UWORD) * 2);
        bb = (UBYTE *)&bw[0];
        FPrintf(fh, "%08lx: %04lx ", addr, bw[0]);
        for (cnt = 2; cnt < size; cnt++) FPrintf(fh, "%02lx", bb[cnt]);
        for (cnt = size; cnt < 8; cnt++) FPrintf(fh, "  ");
        for (cnt = size/4; cnt < 16/4; cnt++) FPrintf(fh, " ");
        FPrintf(fh, "                    ");
        for (cnt = 0; cnt < size; cnt++) {
            c = bb[cnt];
            FPrintf(fh, "%lc", isprint(c) ? c : '.');
        }
        FPrintf(fh, "\n");
    } else if (size > 0) {
        UWORD bw[1];
        UBYTE *bb;

        memcpy(&bw[0], buffer, sizeof(UWORD) * 1);
        bb = (UBYTE *)&bw[0];
        FPrintf(fh, "%08lx: ", addr);
        for (cnt = 0; cnt < size; cnt++) FPrintf(fh, "%02lx", bb[cnt]);
        for (cnt = size; cnt < 4; cnt++) FPrintf(fh, "  ");
        for (cnt = size/4; cnt < 16/4; cnt++) FPrintf(fh, " ");
        FPrintf(fh, "                             ");
        for (cnt = 0; cnt < size; cnt++) {
            c = bb[cnt];
            FPrintf(fh, "%lc", isprint(c) ? c : '.');
        }
        FPrintf(fh, "\n");
    }
}
/* \\\ */

