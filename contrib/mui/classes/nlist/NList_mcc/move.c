#define NO_PROTOS
#include "private.h"

/* attention :  le nombre d'octets doit etre multiple de 4 !
 * et que ca soit efficace src et dest doivent etre multiples de 4 aussi.
 *
 * extern void  NL_Move(void *dest,void *src,LONG len);
 *
 */

void  NL_Move( long *dest, long *src, long len, long newpos )
{
	if ( len > 0 )
	{
		len /= 4;
		len--;

		do
		{
			(*(struct TypeEntry **)src)->entpos = newpos++;
			*(dest++) = *(src++);
		}
		while ( --len >= 0 );
	}
}


void  NL_MoveD( long *dest, long *src, long len, long newpos )
{
	if ( len > 0 )
	{
		len /= 4;
		len--;

		do
		{
			*(--dest) = *(--src);
			(*(struct TypeEntry **)dest)->entpos = --newpos;
		}
		while ( --len >= 0 );
	}
}



/*
void  NL_Move(long *dest,long *src,long len)
{
  if (len > 0)
  {
    len /= 4;
    len--;
    do {
      *(dest++) = *(src++);
    } while (--len >= 0);
  }
}


void  NL_MoveD(long *dest,long *src,long len)
{
  if (len > 0)
  {
    len /= 4;
    len--;
    do {
      *(--dest) = *(--src);
    } while (--len >= 0);
  }
}
*/

/*
gcc -S -O4 Move.c -o Move.s
gcc -s -c Move.s -o Move.o



#NO_APP
gcc2_compiled.:
___gnu_compiled_c:
.text
	.even
.globl _NL_Move
_NL_Move:
	link a5,#0
	movel a5@(8),a1
	movel a5@(12),a0
	movel a5@(16),d0
	jle L9
	asrl #2,d0
	subql #1,d0
	movel d0,d1
	swapw d1
L11:
	movel a0@+,a1@+
	dbraw d0,L11
	dbraw d1,L11
L9:
	unlk a5
	rts
	.even
.globl _NL_MoveD
_NL_MoveD:
	link a5,#0
	movel a5@(8),a1
	movel a5@(12),a0
	movel a5@(16),d0
	jle L23
	asrl #2,d0
	subql #1,d0
	movel d0,d1
	swapw d1
L25:
	movel a0@-,a1@-
	dbraw d0,L25
	dbraw d1,L25
L23:
	unlk a5
	rts






	subql #1,d0
	jpl Lxx

est remplacable par (attention a dbra qui n'utilise que 16 bits de d0 !) :

	movel d0,d1
	swapw d1
...
	dbraw d0,Lxx
	dbraw d1,Lxx


*/
