#ifndef INLINES_H
#define INLINES_H

#ifdef __STORM__
__inline unsigned char leadz(BitBoard b)
#else
static inline unsigned char leadz(BitBoard b)
#endif
{
#ifdef HAVE64BIT
int i=b>>48;
if(i) return lzArray[i];
i=b>>32;
if(i) return lzArray[i]+16;
i=b>>16;
if(i) return lzArray[i]+32;
return lzArray[int(b)]+48;
#else
int i=b.hi>>16;
if(i) return lzArray[i];
if(b.hi) return lzArray[b.hi]+16;
i=b.lo>>16;
if(i) return lzArray[i]+32;
return lzArray[b.lo]+48;
#endif
}

#ifdef __STORM__
__inline unsigned char nbits(BitBoard b)
#else
static inline unsigned char nbits(BitBoard b)
#endif
{
return BitCount[int(b>>48)]+BitCount[int(b>>32)&0xffff]+BitCount[int(b>>16)&0xffff]+BitCount[int(b)&0xffff];
}

#endif
