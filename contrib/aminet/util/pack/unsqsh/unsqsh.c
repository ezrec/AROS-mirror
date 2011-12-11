/*

  portable code to decrunch XPK-SQSH files
  written from "xpksqsh.library 1.10 (6.3.94)"

  (24.12.97) jah@fh-zwickau.de
  tested with sas6.51(m68k) gcc2.7.2(i386) gcc2.7.2(sparc)

  (23.01.99) h430827@stud.u-szeged.hu
  fixed problem with raw chunks, added checksum verification
  tested with egcs-2.91.57(i386)

  (31.05.99) wepl@kagi.com
  complete reworked, header chksum check added, optimized
  tested with xlC3.1.3(ppc) gcc2.7.2(i486) gcc2.8.1(sparc)

*/

/*
#define debug
*/
#include <exec/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define XPKERR_NOXPK 1
#define XPKERR_UNKNOWNCOMP 2
#define XPKERR_FORMAT 3
#define XPKERR_CHKSUM 4
#define XPKERR_UNKNOWNCHUNK 5

int xpkerrno;

static const char *xpkerrtext[] = {
  NULL,
  "not a XPK file",
  "unknown compressor type",
  "data format error",
  "checksum failed",
  "unknown chunk type"
};

int  xpk_check  (UBYTE* src, int* srclen, int* dstlen);
int  xpk_unpack (UBYTE* src, UBYTE* dst);
void sqsh       (UBYTE* src, UBYTE* dst);

int main(int ac,char *av[]) {
  UBYTE *src;
  UBYTE *dst;
  int srclen;
  int dstlen;
  int rc=1;

  if (! (src = (UBYTE*) malloc(16))) {
    fprintf(stderr,"no memory\n");
  } else {
    if (1 != fread(src,16,1,stdin)) {
      perror("error reading source file-header");
    } else {
      if (! xpk_check(src,&srclen,&dstlen)) {
	fprintf(stderr,"%s\n",xpkerrtext[xpkerrno]);
      } else {
	if (! (src = realloc(src,srclen))) {
	  fprintf(stderr,"no memory\n");
	} else {
	  if (! (dst = malloc(dstlen))) {
	    fprintf(stderr,"no memory\n");
	  } else {
	    if (1 != fread(src+16,srclen-16,1,stdin)) {
	      perror("error reading source file");
	    } else {
	      if (! xpk_unpack(src,dst)) {
		fprintf(stderr,"%s\n",xpkerrtext[xpkerrno]);
	      } else {
		if (1 != fwrite(dst,dstlen,1,stdout)) {
		  perror("error writing destination file");
		} else {
		  rc = 0;
		}
	      }
	    }
	    free(dst);
	  }
	}
      }
    }
    free(src);
  }
  return rc;
}

/*
 * check the given 16 byte array if it is a valid XPK-SQSH header
 * and returns source and destination legth
 *
 */
int xpk_check  (UBYTE* src, int* srclen, int* dstlen) {

  if ('X' != *src++ ||
      'P' != *src++ ||
      'K' != *src++ ||
      'F' != *src++) {
    xpkerrno = XPKERR_NOXPK;
    return 0;
  }

  *srclen  = *src++ << 24;
  *srclen |= *src++ << 16;
  *srclen |= *src++ << 8;
  *srclen |= *src++;

  if ('S' != *src++ ||
      'Q' != *src++ ||
      'S' != *src++ ||
      'H' != *src++) {
    xpkerrno = XPKERR_UNKNOWNCOMP;
    return 0;
  }

  *dstlen  = *src++ << 24;
  *dstlen |= *src++ << 16;
  *dstlen |= *src++ << 8;
  *dstlen |= *src++;

  xpkerrno = 0;
  return 1;
}


/*
; file header:
;    0	LONG	"XPKF"
;    4	LONG	file size - 8
;    8	LONG	"SQSH"
;   12	LONG	unpacked size
;  $10	QUAD	original file header
;  $20	WORD
;  $22	WORD
;  $24  first chunk
;
; chunk header:
;    0  BYTE	type
;    1  BYTE	checksum
;    2	WORD	checksum
;    4	WORD	packed chunk size starting from offset 8, long aligned
;    6	WORD	unpacked chunk size
;    8	LABEL	bitstream...
*/

int xpk_unpack (UBYTE* src, UBYTE* dst) {
  int len;		/* unpacked size */
  UBYTE type;		/* type of chunk */
  UBYTE hchk;		/* chunk header chksum */
  UWORD chk;		/* chunk data chksum */
  UWORD cp;		/* chunk packed length */
  UWORD cu;		/* chunk unpacked length */
  ULONG l, *lp;

#ifdef debug
  UBYTE* t=src;		/* used for offset calculation */
#endif

  src += 12;		/* skip id's and file size */
  len  = *src++ << 24;
  len |= *src++ << 16;
  len |= *src++ << 8;
  len |= *src++;
  src += 20;		/* skip original file header and ?? */

  /* do for each chunk */
  while (len) {
    type = *src++;		/* type of chunk */
    hchk = *src++;		/* chunk header chksum */
    chk  = *(UWORD*)src;	/* chunk data chksum */
    src += 2;
    cp   = *src++ << 8;
    cp  |= *src++;		/* chunk packed length */
    cu   = *src++ << 8;
    cu  |= *src++;		/* chunk unpacked length */

#ifdef debug
    fprintf(stderr,"offset=%6x type=%02x chk=%04x cp=%04x cu=%04x",src-t,type,chk,cp,cu);
#endif

    /* check chunk header chksum */
    hchk ^= type;
    hchk ^= chk ^ (chk >> 8);
    hchk ^= cp  ^ (cp >> 8);
    hchk ^= cu  ^ (cu >> 8);
    if (hchk) {
      xpkerrno=XPKERR_CHKSUM;
      return 0;
    }

    /* make packed chunk size long aligned */
    cp   = (cp + 3) & 0xfffc;
    
    /* check chunk data chksum */
    for (l=0,lp=(ULONG*)(src+cp); lp!=(ULONG*)src; ) l ^= *--lp;
    chk ^= l & 0xffff;
    chk ^= l >> 16;
    if (chk) {
      xpkerrno=XPKERR_CHKSUM;
      return 0;
    }
    
    /* check chunk type */
    switch (type) {
    case 0:
      /* unpacked */
      memcpy(dst,src,cu);
      dst += cu;
      src += cp;
      len -= cu;
      continue;
    case 1:
      /* SQSH packed */
      sqsh(src,dst);
      dst += cu;
      src += cp;
      len -= cu;
      continue;
    default:
      /* unknown type */
      xpkerrno=XPKERR_UNKNOWNCHUNK;
      return 0;
    }
  }
  xpkerrno = 0;
  return 1;
}

int bfextu(UBYTE *p,int bo,int bc) {
  int r;

  p += bo / 8;
  r = *(p++);
  r <<= 8;
  r |= *(p++);
  r <<= 8;
  r |= *p;
  r <<= bo % 8;
  r &= 0xffffff;
  r >>= 24 - bc;

  return r;
}

#define bfextu1 ((*(src + d0 / 8) >> (7 - (d0 % 8))) & 1)

int bfextu3(UBYTE *p,int bo) {
  int r;

  p += bo / 8;
  r = *(p++);
  r <<= 8;
  r |= *p;
  r >>= 13 - (bo % 8);
  r &= 7;

  return r;
}

int bfexts(UBYTE *p,int bo,int bc) {
  int r;

  p += bo / 8;
  r = *(p++);
  r <<= 8;
  r |= *(p++);
  r <<= 8;
  r |= *p;
  r <<= (bo % 8) + 8;
  r >>= 32 - bc;

  return r;
}

void sqsh(UBYTE *src, UBYTE *dst) {
  int d0,d1,d2,d3,d4,d5,d6,a2,a5;
  UBYTE *a4,*a6;
  UBYTE a3[] = { 2,3,4,5,6,7,8,0,3,2,4,5,6,7,8,0,4,3,5,2,6,7,8,0,5,4,
	6,2,3,7,8,0,6,5,7,2,3,4,8,0,7,6,8,2,3,4,5,0,8,7,6,2,3,4,5,0 };

#ifdef debug
	fprintf(stderr,"=%02x%02x dst=%x",*src,*(src+1),dst);
#endif

	a6 = dst;
	a6 += *src++ << 8;
	a6 += *src++;
	d0 = d1 = d2 = d3 = a2 = 0;
 
	d3 = *(src++);
	*(dst++) = d3;

l6c6:	if (d1 >= 8) goto l6dc;
	if (bfextu1) goto l75a;
	d0 ++;
	d5 = 0;
	d6 = 8;
	goto l734;

l6dc:	if (bfextu1) goto l726;
	d0 ++;
	if (! bfextu1) goto l75a;
	d0 ++;
	if (bfextu1) goto l6f6;
	d6 = 2;
	goto l708;

l6f6:	d0 ++;
	if (! bfextu1) goto l706;
	d6 = bfextu3(src,d0);
	d0 += 3;
	goto l70a;

l706:	d6 = 3;
l708:	d0 ++;
l70a:	d6 = *(a3 + (8*a2) + d6 - 17);
	if (d6 != 8) goto l730;
l718:	if (d2 < 20) goto l722;
	d5 = 1;
	goto l732;

l722:	d5 = 0;
	goto l734;

l726:	d0 += 1;
	d6 = 8;
	if (d6 == a2) goto l718;
	d6 = a2;
l730:	d5 = 4;
l732:	d2 += 8;
l734:	d4 = bfexts(src,d0,d6);
	d0 += d6;
	d3 -= d4;
	*dst++ = d3;
	d5--;
	if (d5 != -1) goto l734;
	if (d1 == 31) goto l74a;
	d1 += 1;
l74a:	a2 = d6;
l74c:	d6 = d2;
	d6 >>= 3;
	d2 -= d6;
	if (dst < a6) goto l6c6;

#ifdef debug
	fprintf(stderr," dst=%p a6=%p d0=8*%x+%d->%x\n",dst,a6,d0/8,d0%8,(d0+7)/8+3);
/*	fprintf(stderr,"d1=%x d2=%x d3=%x d4=%x d5=%x d6=%x a2=%x a4=%x a5=%x\n",d1,d2,d3,d4,d5,d6,a2,a4,a5); */
#endif

	return;

l75a:	d0++;
	if (bfextu1) goto l766;
	d4 = 2;
	goto l79e;

l766:	d0++;
	if (bfextu1) goto l772;
	d4 = 4;
	goto l79e;

l772:	d0++;
	if (bfextu1) goto l77e;
	d4 = 6;
	goto l79e;

l77e:	d0++;
	if (bfextu1) goto l792;
	d0++;
	d6 = bfextu3(src,d0);
	d0 += 3;
	d6 += 8;
	goto l7a8;

l792:	d0++;
	d6 = bfextu(src,d0,5);
	d0 += 5;
	d4 = 16;
	goto l7a6;

l79e:	d0++;
	d6 = bfextu1;
	d0 ++;
l7a6:	d6 += d4;
l7a8:	if (bfextu1) goto l7c4;
	d0 ++;
	if (bfextu1) goto l7bc;
	d5 = 8;
	a5 = 0;
	goto l7ca;

l7bc:	d5 = 14;
	a5 = -0x1100;
	goto l7ca;

l7c4:	d5 = 12;
	a5 = -0x100;
l7ca:	d0++;
	d4 = bfextu(src,d0,d5);
	d0 += d5;
	d6 -= 3;
	if (d6 < 0) goto l7e0;
	if (d6 == 0) goto l7da;
	d1 -= 1;
l7da:	d1 -= 1;
	if (d1 >= 0) goto l7e0;
	d1 = 0;
l7e0:	d6 += 2;
	a4 = -1 + dst + a5 - d4;
l7ex:	*dst++ = *a4++;
	d6--;
	if (d6 != -1) goto l7ex;
	d3 = *(--a4);
	goto l74c;
}

/*
Unpack	move.l	a6,-(sp)
	movea.l	a0,a6
	movea.l	(a6),a0		xsp_InBuf
	movea.l	(8,a6),a1	xsp_OutBuf
	moveq	#0,d7
	move.w	(a0)+,d7
	move.l	d7,($10,a6)
	bsr.b	lbc0006b0
	moveq	#0,d0
	movea.l	(sp)+,a6
	rts

lbc0006b0	movea.l	a1,a6
	adda.l	d7,a6
	moveq	#0,d0
	moveq	#0,d1
	moveq	#0,d2
	moveq	#0,d3
	suba.w	a2,a2
	lea	(data_a3,pc),a3
	move.b	(a0)+,d3
	move.b	d3,(a1)+
lbc0006c6	cmp.w	#8,d1
	bcc.b	lbc0006dc
	bftst	(a0){d0:1}
	bne.w	lbc00075a
	addq.l	#1,d0
	moveq	#0,d5
	moveq	#8,d6
	bra.b	lbc000734

lbc0006dc	bftst	(a0){d0:1}
	bne.b	lbc000726
	addq.l	#1,d0
	bftst	(a0){d0:1}
	beq.b	lbc00075a
	addq.l	#1,d0
	bftst	(a0){d0:1}
	bne.b	lbc0006f6
	moveq	#2,d6
	bra.b	lbc000708

lbc0006f6	addq.l	#1,d0
	bftst	(a0){d0:1}
	beq.b	lbc000706
	bfextu	(a0){d0:3},d6
	addq.l	#3,d0
	bra.b	lbc00070a

lbc000706	moveq	#3,d6
lbc000708	addq.l	#1,d0
lbc00070a	lea	(-$10,a3,a2.w*8),a4
	move.b	(-1,a4,d6.w),d6
	cmp.w	#8,d6
	bne.b	lbc000730
lbc000718	cmp.w	#$14,d2
	bcs.b	lbc000722
	moveq	#1,d5
	bra.b	lbc000732

lbc000722	moveq	#0,d5
	bra.b	lbc000734

lbc000726	addq.l	#1,d0
	moveq	#8,d6
	cmpa.w	d6,a2
	beq.b	lbc000718
	move.l	a2,d6
lbc000730	moveq	#4,d5
lbc000732	addq.w	#8,d2
lbc000734	bfexts	(a0){d0:d6},d4
	add.l	d6,d0
	sub.b	d4,d3
	move.b	d3,(a1)+
	dbra	d5,lbc000734
	cmp.w	#$1f,d1
	beq.b	lbc00074a
	addq.w	#1,d1
lbc00074a	movea.w	d6,a2
lbc00074c	move.w	d2,d6
	lsr.w	#3,d6
	sub.w	d6,d2
	cmpa.l	a6,a1
	blt.w	lbc0006c6
	rts

lbc00075a	addq.l	#1,d0
	bftst	(a0){d0:1}
	bne.b	lbc000766
	moveq	#2,d4
	bra.b	lbc00079e

lbc000766	addq.l	#1,d0
	bftst	(a0){d0:1}
	bne.b	lbc000772
	moveq	#4,d4
	bra.b	lbc00079e

lbc000772	addq.l	#1,d0
	bftst	(a0){d0:1}
	bne.b	lbc00077e
	moveq	#6,d4
	bra.b	lbc00079e

lbc00077e	addq.l	#1,d0
	bftst	(a0){d0:1}
	bne.b	lbc000792
	addq.l	#1,d0
	bfextu	(a0){d0:3},d6
	addq.l	#3,d0
	addq.l	#8,d6
	bra.b	lbc0007a8

lbc000792	addq.l	#1,d0
	bfextu	(a0){d0:5},d6
	addq.l	#5,d0
	moveq	#$10,d4
	bra.b	lbc0007a6

lbc00079e	addq.l	#1,d0
	bfextu	(a0){d0:1},d6
	addq.l	#1,d0
lbc0007a6	add.w	d4,d6
lbc0007a8	bftst	(a0){d0:1}
	bne.b	lbc0007c4
	addq.l	#1,d0
	bftst	(a0){d0:1}
	bne.b	lbc0007bc
	moveq	#8,d5
	suba.w	a5,a5
	bra.b	lbc0007ca

lbc0007bc	moveq	#14,d5
	movea.w	#$ef00,a5
	bra.b	lbc0007ca

lbc0007c4	moveq	#12,d5
	movea.w	#$ff00,a5
lbc0007ca	addq.l	#1,d0
	bfextu	(a0){d0:d5},d4
	add.l	d5,d0
	subq.w	#3,d6
	bcs.b	lbc0007e0
	beq.b	lbc0007da
	subq.w	#1,d1
lbc0007da	subq.w	#1,d1
	bge.b	lbc0007e0
	moveq	#0,d1
lbc0007e0	addq.w	#2,d6
	lea	(-1,a1,a5.w),a4
	suba.w	d4,a4
	lsr.w	#1,d6
	bcc.b	lbc0007ee
lbc0007ec	move.b	(a4)+,(a1)+
lbc0007ee	move.b	(a4)+,(a1)+
	dbra	d6,lbc0007ec
	move.b	-(a4),d3
	bra.w	lbc00074c

data_a3	dc.l	$2030405
	dc.l	$6070800
	dc.l	$3020405
	dc.l	$6070800
	dc.l	$4030502
	dc.l	$6070800
	dc.l	$5040602
	dc.l	$3070800
	dc.l	$6050702
	dc.l	$3040800
	dc.l	$7060802
	dc.l	$3040500
	dc.l	$8070602
	dc.l	$3040500
	dc.b	0
	dc.b	0
*/
