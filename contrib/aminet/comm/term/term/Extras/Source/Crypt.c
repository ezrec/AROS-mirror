/*
**	Crypt.c
**
**	Data encryption routines.
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* The width of the cell ring is defined here. A word of warning: never
	 * encrypt a file and forget the access password, since it will be
	 * extremely hard to break the code (with 30 cells there are 256^30
	 * possible values each cell may be initialized to which means that
	 * you could have to try more than 1.7e+72 combinations for each single
	 * password character before actually finding the matching password).
	 * See `Random sequence generation by cellular automata' by Steven
	 * Wolfram (Institute for Advanced Study; Advances in Applied
	 * Mathematics) for more information.
	 */

#define CELL_WIDTH 32

	/* The cell ring and the ring index pointers. */

STATIC UBYTE Cell[2][CELL_WIDTH],*Src,*Dst;

	/* The pattern the cell ring will be filled with. The pattern
	 * may seem familiar: these are the first 32 digits of Pi.
	 */

STATIC UBYTE Pi[32] = { 3,1,4,1,5,9,2,6,5,3,5,8,9,7,9,3,2,3,8,4,6,2,6,4,3,3,8,3,2,7,9,5 };

	/* Automaton():
	 *
	 *	A cellular automaton working on a ring of cells, producing
	 *	random data in each single cell.
	 */

STATIC LONG
Automaton(VOID)
{
	UBYTE *A,*B,*Swap;
	LONG i;

	A = Src;
	B = Dst;

		/* Operate on the cell ring... */

	for(i = 1 ; i < CELL_WIDTH - 1 ; i++)
		B[i] = A[i - 1] ^ (A[i] | A[i + 1]);

		/* Operate on first and last element. */

	B[0]			= A[CELL_WIDTH - 1] ^ (A[0]              | A[1]);
	B[CELL_WIDTH - 1]	= A[CELL_WIDTH - 2] ^ (A[CELL_WIDTH - 1] | A[0]);

		/* Swap cell rings. */

	Swap	= Src;
	Src	= Dst;
	Dst	= Swap;

		/* Return contents of first cell. */

	return(A[0]);
}

	/* Encrypt(UBYTE *Source,UBYTE *Destination,UBYTE *Key):
	 *
	 *	Encrypt data using cellular automaton as a random number generator.
	 */

VOID
Encrypt(UBYTE *Source,LONG SourceLen,UBYTE *Destination,UBYTE *Key,LONG KeyLen)
{
	LONG i;

		/* Set up cell ring index pointers. */

	Src = Cell[0];
	Dst = Cell[1];

	if(KeyLen < 32)
		CopyMem(Pi,&Src[KeyLen],32 - KeyLen);

	CopyMem(Key,Src,KeyLen);

		/* Encrypt the source data. */

	for(i = 0 ; i < SourceLen ; i++)
		*Destination++ = ((LONG)Source[i] + Automaton()) % 256;
}

	/* Decrypt(UBYTE *Source,UBYTE *Destination,UBYTE *Key):
	 *
	 *	Decrypt data using cellular automaton as a random number generator.
	 */

VOID
Decrypt(UBYTE *Source,LONG SourceLen,UBYTE *Destination,UBYTE *Key,LONG KeyLen)
{
	LONG i,Code;

		/* Set up cell ring index pointers. */

	Src = Cell[0];
	Dst = Cell[1];

	if(KeyLen < 32)
		CopyMem(Pi,&Src[KeyLen],32 - KeyLen);

	CopyMem(Key,Src,KeyLen);

		/* Decrypt the source data. */

	for(i = 0 ; i < SourceLen ; i++)
	{
		Code = ((LONG)Source[i]) - Automaton();

		if(Code < 0)
			Code += 256;

		*Destination++ = Code;
	}
}
