#define MAXWINSIZE      0x100000
#define MAXWINMASK      (MAXWINSIZE-1)
#define UNP_MEMORY      MAXWINSIZE

void OldUnpack(UBYTE *UnpAddr,int Solid);

extern long DestUnpSize;
extern int Suspend;

