#include <exec/types.h>
#include <aros/debug.h>

void hearconfigdefault()
{
D(bug("function not implemented:  hearconfigdefault "));
}

ULONG checksum(UBYTE *mem, LONG size)
{
    LONG result = 0;
    ULONG *ulp = (ULONG *)mem;
    
    size /= 4;
    
    while(size-- >= 0)
    	result += *ulp++;
	
    return result;
}

WORD hexconvert(UBYTE *src, UBYTE *dest, WORD count1, WORD count2)
{
    UBYTE *ubp;
    WORD i;
    
    ubp = dest;
    
    for(i = 0; i < count2; i++)
    {
    	*ubp++ = ' ';
    	*ubp++ = ' ';
    	*ubp++ = ' ';
    }
    *ubp = ' ';
    
    i = count2 * 3 + 1;
    
    ubp = dest + count2 * 2 + 1;
    
    while(count1--)
    {
    	UBYTE c, c2;
	
	c = *src++;
	c2 = c;
	if (c == ' ') c2 = '.';
	
	*ubp++ = c2;
	
	c2 = c;
	c = (c >> 4) & 0xF;
	
	if (c >= 10) c += 7;
	c += '0';
	*dest++ = c;
	
	c2 &= 0xF;
	if (c2 >= 10) c2 += 7;
	c2 += '0';
	*dest++ = c2;	
    }
    
    return i;
	
}

void modplay()
{
D(bug("function not implemented:  modplay "));   
}

void playmod()
{
D(bug("function not implemented:  hearconfigdefault "));   
}

void modinfo()
{
D(bug("function not implemented:  modinfo "));   
}

void hearconfig()
{
D(bug("function not implemented:  hearconfig "));   
}

void fmname()
{
D(bug("function not implemented:  fmname "));   
}

void fmexee()  
{
D(bug("function not implemented:  fmexee "));   
}

void fmexe()
{
D(bug("function not implemented:  fmexe "));   
}

void bootsum(UBYTE *mem)
{
    ULONG sum = 0, sumold;
    ULONG *ulp = (ULONG *)mem;
    WORD i;
    
    for(i = 0; i < 1024/4; i++)
    {
    	sumold = sum;
	sum += *ulp++;
	if (sum < sumold)
	{
	    /* overflow */
	    sum++;
	}
    }
    
    ulp = (ULONG *)mem;
    ulp[1] = sum;
}

#if 0

	;a0=reqscroll

_RealTimeScroll
l3C00	MOVEM.L	D0-D7/A0-A6,-(SP)
	MOVE.L	A0,A3
	MOVE.L	(A3),D0         d0 = topentrynumber
	MOVE.W	8(A3),D1        d1 = linespacing
	EXT.L	D1
	BSR.W	l3B94                mul64(topentrynumber*linespacing)
	MOVE.L	D0,D7           d0 = d7 =
l3C14	MOVE.L	D7,D6           d6 =
	MOVE.L	4(A3),D0        d0 = numentries -
	SUB.L	10(A3),D0            numlines
	BLE.W	l3D42           if (d0 <= 0) goto l3d42
	MOVE.L	14(A3),A0       a0 = propgadget
	MOVE.L	$22(A0),A0      a0 = propgadget->propinfo
	MOVEQ	#0,D1
	move.w	2(a0),d1        d1 = horizpot
	tst.w	48(a3)	    	test reqscrollstruct->flags
	bne.s	jaska           if flags 1= 0 goto jaska
	MOVE.W	4(A0),D1        d1 = vertpot
jaska	BSR.W	l3BCE                     specialmul64(pot,numentries-numlines)
	MOVE.L	D0,D2           d0 = d2 = 
	MOVE.W	8(A3),D1        d1 = vertbody
	EXT.L	D1
	BSR.W	l3B94           d0 = vertbody * result of speicalmul64 from above
	MOVE.L	D7,D1
	SUB.L	D0,D1
	MOVE.L	D1,D3
	BEQ.W	l3D42
	NEG.L	D1
	BMI.S	l3C50
	ADDQ.L	#8,D1
l3C50	SUBQ.L	#4,D1
	MOVE.L	D1,D0
	BGE.S	l3C58
	NEG.L	D0
l3C58	MOVE.L	D1,D5
	MOVE.W	#5,D1
	BSR.W	l3BE0
	MOVE.L	D0,D1
	TST.L	D5
	BGE.S	l3C6A
	NEG.L	D1
l3C6A	MOVE.W	8(A3),D4
	MULU	12(A3),D4
	MOVE.L	D4,D5
	LSR.L	#2,D5
	SUB.L	D5,D4
	MOVE.W	8(A3),D5
	EXT.L	D5
	SUB.L	D5,D4
	CMP.L	D4,D1
	BGT.S	l3C8A
	NEG.L	D1
	CMP.L	D4,D1
	BLE.S	l3CA6
l3C8A	MOVE.L	D2,(A3)
	SUB.L	D3,D7
	MOVE.L	$12(A3),A0
	MOVEM.L	D0-D7/A0-A6,-(SP)
	EXG	A0,A3
	MOVE.L	A0,-(SP)
	JSR	(A3)
	ADDQ.W	#4,SP
	MOVEM.L	(SP)+,D0-D7/A0-A6
	BRA.W	l3C14

l3CA6	NEG.L	D1
	ADD.L	D1,D7
	MOVE.L	D1,-(SP)
	MOVE.L	D7,D2
	MOVE.L	D6,D3
	TST.L	(SP)
	BPL.S	l3CC0
	MOVE.W	8(A3),D1
	SUBQ.W	#1,D1
	EXT.L	D1
	ADD.L	D1,D2
	ADD.L	D1,D3
l3CC0	MOVE.L	D2,D0
	MOVE.W	8(A3),D1
	BSR.W	l3BE0
	MOVE.L	D0,D1
	MOVE.L	D0,-(SP)
	MOVE.W	8(A3),D0
	EXT.L	D0
	BSR.W	l3B94
	MOVE.L	D0,D1
	MOVE.L	(SP)+,D0
	SUB.L	D7,D1
	MOVEM.L	D0/D1,-(SP)
	MOVE.L	D2,D0
	MOVE.W	8(A3),D1
	BSR.W	l3BE0
	MOVE.L	D0,D2
	MOVE.L	D3,D0
	BSR.W	l3BE0
	MOVE.L	D0,D3
	MOVEM.L	(SP)+,D0/D1
	SUB.L	D2,D3
	BGE.S	l3D20
	NEG.L	D3
	MOVE.L	10(A3),D2
	SUB.L	D3,D2
	ADD.L	D2,D0
	MOVEM.L	D0/D1,-(SP)
	MOVE.L	D2,D0
	MOVE.W	8(A3),D1
	EXT.L	D1
	BSR.B	l3B94
	MOVE.L	D0,D2
	MOVEM.L	(SP)+,D0/D1
	ADD.L	D2,D1
l3D20	MOVE.L	(SP)+,D2
	MOVEM.L	D0-D7/A0-A6,-(SP)
	;MOVE.L	$1A(A3),-(SP)
	move.l	a3,-(sp)
	MOVEM.L	D0-D3,-(SP)
	MOVE.L	$1A(A3),A0
	EXG	A0,A3
	JSR	(A3)
	ADD.W	#$14,SP
	MOVEM.L	(SP)+,D0-D7/A0-A6
	BRA.W	l3C14

l3D42	MOVE.L	D7,D0
	MOVE.W	8(A3),D1
	BSR.W	l3BE0
	MOVE.L	D0,(A3)
	MOVE.L	$16(A3),A0
	MOVE.L	A0,D2
	BEQ.S	l3D66
	MOVEM.L	D0-D7/A0-A6,-(SP)
	EXG	A0,A3
	MOVE.L	A0,-(SP)
	JSR	(A3)
	ADDQ.W	#4,SP
	MOVEM.L	(SP)+,D0-D7/A0-A6
l3D66	MOVE.L	A6,-(SP)
	move.l	40(a3),a6
	JSR	-$10E(A6)
	MOVE.L	(SP)+,A6
	MOVE.L	14(A3),A0
	TST.B	13(A0)
	BMI.W	l3C14
	MOVEM.L	(SP)+,D0-D7/A0-A6
	RTS

l3B94	MOVEM.L	D2-D7/A0-A6,-(SP) ; 64 bit mul: d0.32 * d1.32 = d0.lower32 d1.upper32
	MOVE.W	D1,D2
	MULU	D0,D2
	MOVE.L	D0,D3
	SWAP	D3
	MOVE.L	D1,D4
	SWAP	D4
	MULU	D4,D0
	MULU	D3,D1
	MULU	D4,D3
	SWAP	D0
	SWAP	D1
	MOVE.L	D3,A0
	ADD.W	D0,A0
	ADD.W	D1,A0
	CLR.W	D0
	CLR.W	D1
	ADD.L	D0,D2
	BCC.S	l3BBE
	ADDQ.L	#1,A0
l3BBE	ADD.L	D1,D2
	BCC.S	l3BC4
	ADDQ.L	#1,A0
l3BC4	MOVE.L	A0,D1
	MOVE.L	D2,D0
	MOVEM.L	(SP)+,D2-D7/A0-A6
	RTS

l3BCE	BSR.B	l3B94
	MOVE.W	D0,-(SP)
	MOVE.W	D1,D0
	SWAP	D0
	TST.W	(SP)+
	BPL.S	l3BDE
	ADDQ.L	#1,D0
l3BDE	RTS

l3BE0	MOVEM.L	D1-D7/A0-A6,-(SP)  ; 32 bit division: d0.l = d0.l / d1.l
	MOVE.L	D0,D2	               
	CLR.W	D2                     
	SWAP	D2                     
	DIVU	D1,D2                  
	SWAP	D0                    
	SWAP	D2                     
	MOVE.W	D2,D0                  
	SWAP	D0
	DIVU	D1,D0
	MOVE.W	D0,D2
	MOVE.L	D2,D0
	MOVEM.L	(SP)+,D1-D7/A0-A6
	RTS
	
#endif

void RealTimeScroll()
{
D(bug("function not implemented:  hearconfigdefault "));   
}

void playsample()
{
D(bug("function not implemented:  hearconfigdefault "));   
}

void playsamplefile()
{
D(bug("function not implemented:  hearconfigdefault "));   
}

void playdatatypesample()
{
D(bug("function not implemented:  hearconfigdefault "));   
}

void rawdo() 
{
D(bug("function not implemented:  hearconfigdefault "));   
}

void* rleunpack(void *source, void *dest, LONG size)
{
    UBYTE *s = (UBYTE *)source;
    UBYTE *d = (UBYTE *)dest;
    BYTE c;
    WORD i;
    
    for(;;)
    {
	c = *s++;
	if (c < 0)
	{
    	    i = -c;
	    c = *s++;

	    while(i-- >= 0)
	    {
		*d++ = c;
		size--;
		if (size == 0) goto done;
	    }
	}
	else
	{
    	    while(c-- >= 0)
	    {
		*d++ = *s++;
		size--;
		if (size == 0) goto done;	    
	    }
	}
    }
 
done:
    return (void *)s;   
}

void rlepack()
{
D(bug("function not implemented:  hearconfigdefault "));   
}

void stopoldmod()
{
D(bug("function not implemented:  hearconfigdefault "));   
}

void unpack()
{
D(bug("function not implemented:  hearconfigdefault "));   
}

void unpacknp()
{
D(bug("function not implemented:  hearconfigdefault "));   
}
