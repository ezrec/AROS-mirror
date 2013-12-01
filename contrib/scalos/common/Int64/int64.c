// int64.c
// $Date$

/***********************************************************************
*
* Function:
*        Operators on Large (64-bit) Integers
*
* Public Routines:
*        MakeU64()
*        MakeS64()
*        Incr64()
*        Decr64()
*        Add64()
*        Sub64()
*        ShiftR64()
*        ShiftL64()
*        Mul64()
*        Div64()
*
***********************************************************************/

/* Standard (ANSI) C library header files. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Application header files. */
#include "int64.h"

/***********************************************************************
*        Public Routines
***********************************************************************/

/*----------------------------------------------------------------------
* Name:    MakeU64()
*
* Action:  Construct a 64-bit unsigned integer from a 32-bit unsigned integer.
*
* Input:   n - 32 bit integer to be used to construct 64-bit unsigned integer.
*
* Return
* Value:   Constructed 64-bit unsigned integer.
*
* Side
* Effects: None.
*
*---------------------------------------------------------------------*/
ULONG64 MakeU64( ULONG n )
{
#ifdef __GNUC__
	return (ULONG64) n;
#else /* __GNUC__ */
	ULONG64 result;

	result.High = 0;
	result.Low = n;

	return ( result );
#endif /* __GNUC__ */
} /* MakeU64() */

/*----------------------------------------------------------------------
* Name:    MakeS64()
*
* Action:  Construct a 64-bit signed integer from a 32-bit signed integer.
*
* Input:   n - 32 bit integer to be used to construct 64-bit signed integer.
*
* Return
* Value:   Constructed 64-bit signed integer.
*
* Side
* Effects: None.
*
*---------------------------------------------------------------------*/
SLONG64 MakeS64( LONG n )
{
#ifdef __GNUC__
	return (SLONG64) n;
#else /* __GNUC__ */
	SLONG64 result;

	result.High = (n < 0) ? -1 : 0;
	result.Low = n;

	return ( result );
#endif /* __GNUC__ */
} /* MakeS64() */

/*----------------------------------------------------------------------
* Name:    Incr64()
*
* Action:  Increment a 64-bit integer by some 32-bit integer.
*
* Input:   x - 64-bit integer to be incremented.
*          n - 32-bit increment.
*
* Return
* Value:   Result of increment operation.
*
* Side
* Effects: None.
*
* Notes:   Assume no overflow of sum.
*          Rewritten for efficiency
*---------------------------------------------------------------------*/
ULONG64 Incr64(ULONG64 x, ULONG n )
{
#ifdef __GNUC__
	return x + n;
#else /* __GNUC__ */
	x.Low += n;			/* add lo order word */

	if ( x.Low < n )			/* did the add cause an overflow? */
		x.High++;			/* yes, adjust hi word for carry */

	return ( x );
#endif /* __GNUC__ */
} /* Incr64() */

/*----------------------------------------------------------------------
* Name:    Decr64()
*
* Action:  Decrement a 64-bit integer by some 32-bit integer.
*
* Input:   x - 64-bit integer to be decremented.
*          n - 32-bit decrement.
*
* Return
* Value:   Result of decrement operation.
*
* Side
* Effects: largeIntP - replaced by the result of the decrement
*                      operation.
*
* Notes:   Assume no underflow of difference.
*          Rewritten for efficiency
*---------------------------------------------------------------------*/
ULONG64 Decr64(ULONG64 x, ULONG n )
{
#ifdef __GNUC__
	return x - n;
#else /* __GNUC__ */
	if ( x.Low < n )		/* is a borrow required? */
		x.High--;		/* yes, borrow from hi word */

	x.Low -= n;		/* subtract lo order word */

	return ( x );
#endif /* __GNUC__ */
} /* Decr64() */

/*----------------------------------------------------------------------
* Name:    Add64()
*
* Action:  Add two 64-bit integers.
*
* Input:   x - first 64-bit integer.
*          y - second 64-bit integer.
*
* Return
* Value:   Result of addition operation.
*
* Side
* Effects: None.
*
* Notes:   Assume no overflow of sum.
*          Rewritten for efficiency
*---------------------------------------------------------------------*/
ULONG64 Add64(ULONG64 x, ULONG64 y )
{
#ifdef __GNUC__
	return x + y;
#else /* __GNUC__ */
	x = Incr64( x, y.Low);
	x.High += y.High;

	return ( x );
#endif /* __GNUC__ */
} /* Add64() */

/*----------------------------------------------------------------------
* Name:    Sub64()
*
* Action:  Subtract two 64-bit integers.
*
* Input:   x - first 64-bit integer.
*          y - second 64-bit integer.
*
* Return
* Value:   Result of subtracting second 64-bit integer from the first.
*
* Side
* Effects: None.
*
* Notes:   Assume no underflow of difference (ie. x >= y)
*          Rewritten for efficiency
*---------------------------------------------------------------------*/
ULONG64 Sub64(ULONG64 x, ULONG64 y )
{
#ifdef __GNUC__
	return x - y;
#else /* __GNUC__ */
	x = Decr64( x, y.Low);
	x.High -= y.High;

	return ( x );
#endif /* __GNUC__ */
} /* Sub64() */

/*----------------------------------------------------------------------
* Name:    ShiftR64()
*
* Action:  Performs a logical shift right on an ULONG64 by b bits.
*
* Input:   x - the address of the 64-bit integer.
*          b - number bits to shift.
*
* Return
* Value:   the value passed in is shifted
*
* Side
* Effects: None.
*
* Notes:   As expected, bits shifted below bit 0 go into the bit bucket.
*---------------------------------------------------------------------*/
void ShiftR64(ULONG64 *x, UBYTE b)
{
#ifdef __GNUC__
	*x >>= b;
#else /* __GNUC__ */
	if (b >= 32)
		{
		x->Low = x->High;
		x->High = 0;
		b &= 0x1F;		/* get rid of all bits above value 32	*/
		}
	x->Low >>= b;			/* shift the low word into position    */
	x->Low |= (x->High << (32-b));	/* OR in the bits remaining of hi word */
	x->High >>= b;			/* shift the hi word by x bits         */
#endif /* __GNUC__ */
}  /* end ShiftR64() */

/*----------------------------------------------------------------------
* Name:    ShiftL64()
*
* Action:  Performs a logical shift left on an ULONG64 by b bits.
*
* Input:   x - the address of the 64-bit integer.
*          b - number of bits to shift.
*
* Return
* Value:   The value passed in is shifted
*
* Side
* Effects: None.
*
* Notes:   As expected, bits shifted above bit 63 go into the bit bucket.
*---------------------------------------------------------------------*/
void ShiftL64(ULONG64 *x, UBYTE b)
{
#ifdef __GNUC__
	*x <<= b;
#else /* __GNUC__ */
	if (b >= 32)
		{
		x->High = x->Low;
		x->Low = 0;
		b &= 0x1F;		/* get rid of all bits above value 32  */
		}
	x->High <<= b;			/* shift the hi word into position     */
	x->High |= (x->Low >> (32-b));	/* OR in the bits remaining of lo word */
	x->Low <<= b;			/* shift the lo word by x bits         */
#endif /* __GNUC__ */
}  /* end ShiftL64() */

/*----------------------------------------------------------------------
* Name:    Mul64()
*
* Action:  Multiply two 64 bit integers.
*
* Input:   x - first 64 bit integer
*          y - second 64 bit integer
*          overflow - address of an integer
*
* Return
* Values:  1) 64 bit Product (x * y) is returned
*          2) overflow set to 0 or 1 to indicate if Product exceeded
*             64 bits (1 indicates overflow)
*
* Side
* Effects: None.
*
* Notes:   Any overflow above 64 bits will be lost.
*---------------------------------------------------------------------*/
ULONG64 Mul64(ULONG64 x, ULONG64 y, BOOL *overflow)
{
#ifdef __GNUC__
	if (overflow)
		*overflow = FALSE;
	return x * y;
#else /* __GNUC__ */
	ULONG64 Prod;
	int bits_lost = 0;			/* 1 = bits have been lost on hi end */

	if (overflow)
		*overflow = 0;			/* start w/ no overflow */
	Prod.High = 0;				/* initialize Product  */
	Prod.Low = 0;

	do	{
		if (y.Low & 0x1)			/* if bit 0 is 1, curr power of 2 will be added */
			{
			/* now that we're adding again, if there have been any bits   */
			/* lost from hi end of x, we know there's an overflow.        */

			if (bits_lost && overflow)
				*overflow = 1;

			Prod = Add64(Prod,x);	/* add shifted x to the product */
			}

		if (x.High & 0x80000000)		/* if bit 63 of x is on before ShiftL, */
			bits_lost = 1;		/* indicate bits have been lost */

		ShiftL64(&x,1);			/* double the value being multiplied   */
		ShiftR64(&y,1);			/* shift next test bit into position   */
		} while (y.High || y.Low);		/* until all y bits are 0 (ie.Mult done) */

	return ( Prod );
#endif /* __GNUC__ */
}  /* end Mul64() */

/*----------------------------------------------------------------------
* Name:    Div64()
*
* Action:  Divide two 64 bit integers.
*
* Input:   x - first 64 bit integer
*          y - second 64 bit integer
*
* Return
* Values:  1) Quotient is returned
*          2) Remainder is stored in specified address address if not NULL
*
* Side
* Effects: None.
*
* Notes:   As usual, bits shifted below bit 0 go into the bit bucket.
*---------------------------------------------------------------------*/
ULONG64 Div64(ULONG64 x, ULONG64 y, ULONG64 *rmdrP)
{
#ifdef __GNUC__
	if (rmdrP)
		*rmdrP = x % y;
	return x / y;
#else /* __GNUC__ */
	ULONG64 Quot,	/* the final answer is accumulated here       */
	tQuot,		/* temp. quotient to find a single power of 2 */
			/*  that fits into the dividend on each pass  */
	divisor;

	Quot.High = 0;					/* initialize Quotient */
	Quot.Low = 0;

	if (y.High==0 && y.Low==0)			/* check for divide by zero */
		{
		y.High = 1/y.Low;			/* force a divide by zero so that error-  */
							/*  handling will be platform-specific    */
		(void) y;
		}
	else	/* do the division */
		{
		while (Cmp64(x,y) >= 0)			/* while x >= y            */
			{
			/* find largest Y (multiplied by powers of 2) that is less than X */
			divisor = y;			/* copy Y so it is unchanged  */
			tQuot.High = 0;			/* init temp quotient (at least 1y goes into x) */
			tQuot.Low = 1;

			while (Cmp64(x,divisor) >= 0 && (divisor.High <= 0x7FFFFFFF))
				{
				ShiftL64(&divisor,1);	/* double divisor (2y 4y 8y...) */
				ShiftL64(&tQuot,1);	/* count number of y's so far   */
				}

			ShiftR64(&divisor,1);		/* back up to a value that's less than X*/
			ShiftR64(&tQuot,1);

			Quot = Add64(Quot,tQuot);	/* calc cumulative quotient    */
			x = Sub64(x,divisor);		/* deduct the multiples so far */
			}

		if (rmdrP)
			*rmdrP = x;			/* save remainder in specified address */
		}
	return ( Quot );
#endif /* __GNUC__ */
}  /* end Div64() */


//----------------------------------------------------------------------
// comparison of a and b
// returns +1 if a > b
//	    0 if a == b
//         -1 if a < b
int Cmp64(ULONG64 x, ULONG64 y)
{
#ifdef __GNUC__
	if (x > y)
		return 1;
	else if (x < y)
		return -1;
#else /* __GNUC__ */
	if (x.High > y.High)
		return 1;
	else if (x.High < y.High)
		return -1;
	else
		{
		if (x.Low > y.Low)
			return 1;
		else if (x.Low < y.Low)
			return -1;
		}
#endif /* __GNUC__ */

	return 0;
}

//----------------------------------------------------------------------
// ASCII conversion of number

void Convert64(const struct Locale *locale, ULONG64 Number, STRPTR Buffer, size_t MaxLen)
{
	char InternalBuffer[40];
	char *lp;
	ULONG64 Ten = MakeU64(10);
	size_t MaxLen2;
	STRPTR Separator = NULL;
	short grp, GroupSize = 0;
	size_t SeparatorLength = 0;
	BOOL NumberIsNotZero;

	if (locale)
		{
		Separator = locale->loc_GroupSeparator;
		GroupSize = locale->loc_Grouping[0];
		SeparatorLength = strlen(Separator);
		}

	MaxLen2 = sizeof(InternalBuffer) - 1;
	lp = InternalBuffer + MaxLen2;
	*lp-- = '\0';
	grp = 0;

	do	{
		ULONG64 Remainder;

		Number = Div64(Number, Ten, &Remainder);

		*lp-- = (char) ULONG64_LOW(Remainder) + '0';
		MaxLen2--;

		NumberIsNotZero = Cmp64(Number, MakeU64(0)) > 0;

		// insert separator
		if (Separator && NumberIsNotZero && ++grp >= GroupSize && MaxLen2 > SeparatorLength)
			{
			lp -= SeparatorLength;
			strncpy(1 + lp, Separator, SeparatorLength);
			grp = 0;
			MaxLen2 -= SeparatorLength;
			}
		} while (NumberIsNotZero && MaxLen2 > 0);
	lp++;

	while (MaxLen > 1 && *lp)
		{
		*Buffer++ = *lp++;
		MaxLen--;
		}
	*Buffer = '\0';
}

//----------------------------------------------------------------------


