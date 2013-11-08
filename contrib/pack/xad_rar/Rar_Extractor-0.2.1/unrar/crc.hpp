
// Rar_Extractor 0.2.1. See unrar_license.txt.

#ifndef _RAR_CRC_
#define _RAR_CRC_

uint CRC(uint StartCRC,const void *Addr,uint Size);
ushort OldCRC(ushort StartCRC,const void *Addr,uint Size);

#endif
