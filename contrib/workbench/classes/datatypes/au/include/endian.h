#ifndef ENDIAN_H
#define ENDIAN_H

#if defined(__X86__)
#define LITTLE_ENDIAN 1
#endif

// generic C routines

#ifdef MAKE_ID
#undef MAKE_ID
#endif

#if defined(LITTLE_ENDIAN)
#define MAKE_ID(a,b,c,d)	\
	((uint32) (d)<<24 | (uint32) (c)<<16 | (uint32) (b)<<8 | (uint32) (a))
#else
#define MAKE_ID(a,b,c,d)	\
	((uint32) (a)<<24 | (uint32) (b)<<16 | (uint32) (c)<<8 | (uint32) (d))
#endif

#define read_le16(P) le2nat16(*(uint16 *)(P))
#define read_le32(P) le2nat32(*(uint32 *)(P))
#define read_le64(P) le2nat64(*(uint64 *)(P))

#define write_le16(P,V) *(uint16 *)(P)=nat2le16(V)
#define write_le32(P,V) *(uint32 *)(P)=nat2le32(V)
#define write_le64(P,V) *(uint64 *)(P)=nat2le64(V)

#define read_be16(P) be2nat16(*(uint16 *)(P))
#define read_be32(P) be2nat32(*(uint32 *)(P))
#define read_be64(P) be2nat64(*(uint64 *)(P))

#define write_be16(P,V) *(uint16 *)(P)=nat2be16(V)
#define write_be32(P,V) *(uint32 *)(P)=nat2be32(V)
#define write_be64(P,V) *(uint64 *)(P)=nat2be64(V)

#define endian64(A) \
endian32((uint64)(A) >> 32)+(endian32(A) << 32)

#define endian32(A) \
( ( ((uint32)(A)&0xFF000000)>>24 )+( ((uint32)(A)&0xFF0000)>>8 )+( ((uint32)(A)&0xFF00)<<8 ) \
+( ((uint32)(A)&0xFF)<<24 ) )

#define endian16(A) \
( ( ((uint16)(A)&0xFF00)>>8 )+( ((uint16)(A)&0xFF)<<8 ) )

#if defined(__M68K__) && defined(__VBCC__) // use VBCC 68k inline asm

#undef endian32
#undef endian16

int32 endian32(__reg("d0") int32) =
	"\trol.w\t#8,d0\n"
	"\tswap\td0\n"
	"\trol.w\t#8,d0";

int16 endian16(__reg("d0") int16) =
	"\trol.w\t#8,d0";

#elif defined(__PPC__) && defined(__VBCC__) // use VBCC PPC inline asm

#undef read_le64
#undef read_le32
#undef read_le16

int64 read_le64(__reg("r4") void *) =
	"\taddi\t3,4,4"
	"\tlwbrx\t4,0,4"
	"\tlwbrx\t3,0,3";

int32 read_le32(__reg("r3") void *) =
	"\tlwbrx\t3,0,3";

int16 read_le16(__reg("r3") void *) =
	"\tlhbrx\t3,0,3";

#undef write_le64
#undef write_le32
#undef write_le16

void write_le64(__reg("r4") void *, __reg("r5/r6") int64) =
	"\taddi\t3,4,4"
	"\tstwbrx\t4,0,6"
	"\tstwbrx\t3,0,5";

void write_le32(__reg("r3") void *, __reg("r4") int32) =
	"\tstwbrx\t4,0,3";

void write_le16(__reg("r3") void *, __reg("r4") int16) =
	"\tsthbrx\t4,0,3";

/*
#undef endian32
#undef endian16

int32 endian32(__reg("r3") int32) =

int16 endian16(__reg("r3") int16) =
*/

/*
#elif defined(__PPC__) && defined(__GNU_C__) // use GCC PPC asm

#undef endian32
#undef endian16
*/

#endif

#if defined(LITTLE_ENDIAN)

#define be2nat16(A) endian16(A)
#define be2nat32(A) endian32(A)
#define be2nat64(A) endian64(A)

#define le2nat16(A) (A)
#define le2nat32(A) (A)
#define le2nat64(A) (A)

#define nat2be16(A) endian16(A)
#define nat2be32(A) endian32(A)
#define nat2be64(A) endian64(A)

#define nat2le16(A) (A)
#define nat2le32(A) (A)
#define nat2le64(A) (A)

#else

#define be2nat16(A) (A)
#define be2nat32(A) (A)
#define be2nat64(A) (A)

#define le2nat16(A) endian16(A)
#define le2nat32(A) endian32(A)
#define le2nat64(A) endian64(A)

#define nat2be16(A) (A)
#define nat2be32(A) (A)
#define nat2be64(A) (A)

#define nat2le16(A) endian16(A)
#define nat2le32(A) endian32(A)
#define nat2le64(A) endian64(A)

#endif

#endif /* ENDIAN_H */
