#ifndef lint
static char *RCSid = "$Id$";
#endif

/*
 *  The Regina Rexx Interpreter
 *  Copyright (C) 1992-1994  Anders Christensen <anders@pvv.unit.no>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "rexx.h"
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>


#define log_xor(a,b)    (( (a)&&(!(b)) ) || ( (!(a)) && (b) ))
#if !defined(MAX)
# define MAX(a,b) ((a>b)?(a):(b))
#endif
#if !defined(MIN)
# define MIN(a,b) ((a<b)?(a):(b))
#endif
#define IS_AT_LEAST(ptr,now,min) \
   if (now<min) { if (ptr) FreeTSD(ptr); ptr=MallocTSD(now=min) ; } ;


/* hmmm perhaps it should be rebuilt, so that 0 gave: size=0 and num
   and exp undefined, that would be most logical ... */


#define MAX_EXPONENT (999999999)

typedef struct { /* mat_tsd: static variables of this module (thread-safe) */
#ifdef TRACEMEM
   void *    outptr1;
   void *    outptr2;
   void *    outptr3;
   void *    outptr4;
   void *    outptr5;
#endif

   num_descr edescr;
   num_descr fdescr;
   num_descr rdescr;
   num_descr sdescr;



   int       ClassicFormat; /* For testing purpose it is declared here and is
                             * equivalent to NotJ18                         .
                             */
   int       OldRegina; /* For testing purpose only */

   int       add_outsize;   /* This values MAY all become one. CHECK THIS! */
   char *    add_out;
   int       norm_outsize;
   char *    norm_out;
   int       div_outsize;
   char *    div_out;
   int       mul_outsize;
   char *    mul_out;
} mat_tsd_t; /* thread-specific but only needed by this module. see
              * init_math
              */

/* init_math initializes the module.
 * Currently, we set up the thread specific data and check for environment
 * variables to change debugging behaviour.
 * The function returns 1 on success, 0 if memory is short.
 */
int init_math( tsd_t *TSD )
{
   mat_tsd_t *mt;

   if (TSD->mat_tsd != NULL)
      return(1);

   if ((mt = TSD->mat_tsd = MallocTSD(sizeof(mat_tsd_t))) == NULL)
      return(0);
   memset(mt,0,sizeof(mat_tsd_t));

   if (getenv("OLD_REGINA") != NULL)
      mt->OldRegina = 1;
   if (getenv("CLASSIC_REGINA") != NULL)
      mt->ClassicFormat = 1;
   return(1);
}

#ifdef TRACEMEM
void mark_descrs( const tsd_t *TSD )
{
   mat_tsd_t *mt;

   mt = TSD->mat_tsd;
   if (mt->rdescr.num) markmemory( mt->rdescr.num, TRC_MATH ) ;
   if (mt->sdescr.num) markmemory( mt->sdescr.num, TRC_MATH ) ;
   if (mt->fdescr.num) markmemory( mt->fdescr.num, TRC_MATH ) ;
   if (mt->edescr.num) markmemory( mt->edescr.num, TRC_MATH ) ;

   if (mt->outptr1) markmemory( mt->outptr1, TRC_MATH ) ;
   if (mt->outptr2) markmemory( mt->outptr2, TRC_MATH ) ;
   if (mt->outptr3) markmemory( mt->outptr3, TRC_MATH ) ;
   if (mt->outptr4) markmemory( mt->outptr4, TRC_MATH ) ;
   if (mt->outptr5) markmemory( mt->outptr5, TRC_MATH ) ;
}
#endif /* TRACEMEM */


int descr_to_int( const num_descr *input )
{
   /* number must be integer, and must be small enough */
   int result=0, i=0 ;

   if (input->size<input->exp)
       exiterror( ERR_INVALID_INTEGER, 0 )  ;
   else if (input->size>input->exp)
   {
      i = MAX( 0, input->exp ) ;
      for (; i<input->size; i++)
      {
         if (input->num[i]!='0')
            exiterror( ERR_INVALID_INTEGER, 0 )  ;
      }
   }


   for (i=0; i<input->size; i++)
   {
      result = result*10 + (input->num[i] - '0') ;
   }
   if (input->negative)
      result = -result ;

   return result ;
}

void str_strip( num_descr *num )
{
   int i=0, j=0 ;

   if (num->size==1)
   {
      if (num->num[0] == '0')
      {
         num->negative = 0 ;
         num->exp = 1 ;
      }
      return ;
   }

   for (i=0; i<num->size-1 && num->num[i]=='0'; i++ ) ;
   if (i)
   {
      for (j=0; j<(num->size-i); j++)
      {
         num->num[j] = num->num[j+i] ;
      }

      num->exp -= i ;
      num->size -= i ;
      assert( num->size > 0 ) ;
   }

   if ((num->size==1) && (num->num[0]=='0'))
   {
      num->negative = 0 ;
      num->exp = 1 ;
   }
}


#if 1
int getdescr( const tsd_t *TSD, const streng *num, num_descr *descr )
/* converts num into a descr and returns 0 if successfully.
 * returns 1 in case of an error. descr contains nonsense in this case.
 * The newly generated descr is as short as possible: leading and
 * trailing zeros (after a period) will be cut, rounding occurs.
 * We don't use registers and hope the compiler does it better than outselves
 * in the optimization stage, else try in this order: c, inlen, in, out, exp.
 */
{
   const char *in;      /* num->value */
   int   inlen;         /* chars left in "in" */
   char *out;           /* descr->num */
   int   outpos;        /* position where to write */
   int   outmax;        /* descr->max */
   char  c,             /* tmp var */
         lastdigit = 0; /* last digit seen for mantissa, init: error */
   int   pointseen,     /* point in mantissa seen? */
         exp,           /* exp from mantissa */
         exp2,          /* exp from "1E1" */
         expminus;      /* exp in "1E-1" is negative? */

   IS_AT_LEAST( descr->num, descr->max, TSD->currlevel->currnumsize+1 ) ;

   in = num->value;
   inlen = Str_len(num);
   /* skip leading spaces */
   while (inlen && isspace(*in))
   {
      in++;
      inlen--;
   }

   if (!inlen)
      return 1 ;

   c = *in;

   /* check sign */
   if ((c == '-') || (c == '+'))
   {
      descr->negative = (c == '-') ;
      in++; /* c eaten */
      inlen--;
      while (inlen && isspace(*in)) /* skip leading spaces */
      {
         in++;
         inlen--;
      }

      if (!inlen)
         return 1 ;
   }
   else
      descr->negative = 0 ;

   /* cut ending blanks first, a non blank exists (in[0]) at this point */
   while (isspace(in[inlen-1]))
      inlen--;

   while (inlen && (*in == '0')) /* skip leading zeros */
   {
      in++;
      inlen--;
      lastdigit = '0';
   }
   if (!inlen)
   {  /* Fast breakout in case of a plain "0" or an error */

      descr->num[0] = lastdigit;
      descr->exp = 1;
      descr->size = 1;
      if (lastdigit == '0')
      {
         descr->negative = 0;
         return 0 ;
      }
      return 1 ;
   }

   /* Transfer digits and check for points */
   pointseen = 0; /* never seen */
   exp = 0;
   out = descr->num;
   outmax = TSD->currlevel->currnumsize+1;
   outpos = 0;
   while (inlen)
   {
      if ((c = *in) == '.')
      {
         if (pointseen)
            return 1 ;
         pointseen = 1;
         in++;
         inlen--;
         continue;
      }
      if (!isdigit(c))
         break;
      if (outpos < outmax)
      {
         lastdigit = c;
         if ((c=='0') && (outpos==0)) /* skip zeros in "0.0001" */
            exp--;       /* We must be after a point, see zero parsing above */
         else
         {
            out[outpos++] = c;
            if (!pointseen)
               exp++;
         }
      }
      else
      {
         lastdigit = '0';
         if (!pointseen)
            exp++;
      }
      in++;
      inlen--;
   }
   /* the mantissa is correct now, check for ugly "0.0000" later */
   if (inlen)
   {
      /* c is *in at this point, see above */
      expminus = 0;
      if ((c != 'e') && (c != 'E'))
         return 1 ;
      if (--inlen == 0) /* at least one digit must follow */
         return 1 ;
      in++;

      c = *in;
      if ((c == '+') || (c == '-'))
      {
         if (c == '-')
            expminus = 1;
         if (--inlen == 0) /* at least one digit must follow */
            return 1 ;
         in++;
      }
      exp2 = 0;
      while (inlen--)
      {
         c = *in++;
         if (!isdigit(c))
            return 1 ;
         exp2 = exp2*10 + (c - '0'); /* Hmm, no overflow checking? */
      }
      if (expminus)
         exp -= exp2;
        else
         exp += exp2;
   }
   if (outpos == 0) /* no digit or 0.000 with or without exp */
   {
      if (!lastdigit)
         return 1 ;
      out[outpos++] = '0';
      exp = 1;
      descr->negative = 0;
   }
   descr->exp = exp;
   descr->size = outpos;
   assert(descr->size <= TSD->currlevel->currnumsize+1);
   return(0);
}
#else
int getdescr( const tsd_t *TSD, const streng *num, num_descr *descr )
{
   register unsigned char *i=NULL, *k=NULL, *top=NULL, *start=NULL ;
   unsigned char *ktop=NULL, ch=' ' ;
   register int kextra=0, skipped=0 ;
   register int skipzeros=0, decipoint=0, found_digits=0 ;

   IS_AT_LEAST( descr->num, descr->max, TSD->currlevel->currnumsize+1 ) ;

   /* skip leading whitespace */
   i = (unsigned char *)num->value ;
   top = i + Str_len( num ) ;
   for ( ; (i<top  && isspace(ch=*i)); i++ ) ; /* FGC: ordered */

   /* set the sign, and skip more whitespace */
   descr->negative = 0 ;
   if (((ch == '-') || (ch == '+')) && i<top)
   {
      descr->negative = (*(i++) == '-') ;
      for ( ; (i<top) && (isspace(*i)); i++ ) ;
   }

   /* This is slightly confusing ... but the conventions are:
    *
    *  decipoint - number of leading digits in descr->num that are in
    *              front of the decimal point, if any.
    *  skipzeros - number of leading zeros in the output, _after_ the
    *              decimal point, which has been skipped.
    *  examples:
    *      00.0004   ->  decipoint=0   skipzeros=3
    *      123.456   ->  decipoint=3   skipzeros=0
    *      0004000   ->  decipoint=-1  skipzeros=-1
    */

   /* read all digits in number */
   start = i ;
   kextra = 0 ;
   skipped = 0 ;
   skipzeros = -1 ;
   decipoint = -1 ;
   found_digits = 0 ;
   k = (unsigned char*)descr->num ;
   ktop = k + TSD->currlevel->currnumsize + 1 ;
   for (; i<top; i++ )
   {
      if ( isdigit( ch=*i ) )
      {
         if (skipzeros<0)
         {
            found_digits = 1 ;
            if (ch=='0')
            {
               if (decipoint>=0) skipped++ ;
               continue ;
            }
            else
            {
               assert( decipoint <= 0 ) ;
               skipzeros = skipped ;
            }
         }

         if (k < ktop)
            *(k++) = ch ;
         else
            kextra++ ;
      }
      else if (ch=='.')
      {
         if (decipoint!=(-1))
            return 1 ;

         decipoint = (int)( (char*)k - (char*)descr->num ) ;
      }
      else
         break ;
    }

   descr->exp = 0 ;
   if ((i<top) && ((ch=='e') || (ch=='E')))
   {
      int sign=0 ;
      unsigned char *tmp ;

      if ((*(++i) == '-') || (*i == '+'))
         sign = (*(i++) == '-') ;

      for (tmp=i; (i<top) && (isdigit(*i)); i++ )
         descr->exp = descr->exp * 10 + (*i - '0') ;

      if (tmp==i) return 1 ;

      if (sign)
         descr->exp = - descr->exp ;
   }

   /* If we didn't find any non-zero digits */
   descr->size = (int)( (char*)k - (char*)descr->num ) ;
   if (skipzeros<0)
   {
      if (!found_digits)
         return 1 ;

      descr->exp += 1 - skipped ;
      *(k++) = '0' ;
      descr->size++ ;
   }
   else if (decipoint<0)
      descr->exp += descr->size + kextra ;
   else
      descr->exp += decipoint - skipzeros ;

   /* check for non-white-space at the end */
   for (; i<top; i++ )
      if (!isspace(*i))
         return 1 ;

   assert( descr->size <= TSD->currlevel->currnumsize+1 ) ;
   return 0 ;
}
#endif

void str_round( num_descr *descr, int size )
{
   int i=0 ;

   /* we can't round to zero digits */
   if (size==0)
   {
      if (descr->num[0]>='5')
      {
         descr->num[0] = '1' ;
         descr->exp++ ;
         descr->size = 1 ;
      }
      else
      {
         descr->num[0] = '0' ;
         descr->size = 1 ;
         descr->negative = descr->exp = 0 ;
      }
      return ;
   }
   else if (size<0)
   {
      descr->num[0] = '0' ;
      descr->size = 1 ;
      descr->exp = descr->negative = 0 ;
      return ;
   }

   /* increment size by the number of leading zeros existing */
   for (i=0; i<descr->size && descr->num[i]=='0'; i++) ;
   size += i ;

   /* do we have to round? */
   if (descr->size<=size)
      return ;

   /* set the size to the wanted value */
   descr->size = size ;

   /* is it possible just to truncate? */
   if (descr->num[size] < '5')
      return ;

   /* increment next digit, and loop if that is a '9' */
   for (i=size-1;;)
   {
      /* can we get away with inc'ing this digit? */
      if (descr->num[i] != '9')
      {
         descr->num[i]++ ;
         return ;
      }

      /* no, set it to zero, and inc' next digit */
      descr->num[i--] = '0' ;

      /* if there are no more digits, move number one magnitude up */
      if (i==(-1))
      {
#ifndef NDEBUG
         /* Just check a few things ... I don't like surprises */
         for (i=0; i<size; i++)
            assert( descr->num[i] == '0' ) ;
#endif

         /* increase order of magnitude, and set first digit */
         descr->exp++ ;
         descr->num[0] = '1' ;
         return ;
      }
   }
}



void descr_copy( const tsd_t *TSD, const num_descr *f, num_descr *s )
{
   /*
    * Check for the special case that these are identical, then we don't
    * have to do any copying, so just return.
    */
   if (f==s)
      return ;

   s->negative = f->negative ;
   s->exp = f->exp ;
   s->size = f->size ;

   IS_AT_LEAST( s->num, s->max, f->size ) ;
   memcpy( s->num, f->num, f->size ) ;
}



/*
 *
 *
 * So, why don't we just flush the changes into the result string
 * directly, without temporarily storing it in the out string? Well,
 * the answer is that if this function is called like:
 *
 *    string_add( TSD, &descr1, &descr2, &descr1 )
 *
 * then it should be able to produce the correct answer, which is
 * impossible to do without a temporary storage. (Hmmm. No, that is
 * bogos, it just takes a bit of care to not overwrite anything that
 * we might need. Must be rewritten). Another problem, if the result
 * string is to small to hold the answer, we must reallocate space
 * so we might have to live with the out anyway.
 */
void string_add( const tsd_t *TSD, const num_descr *f, const num_descr *s, num_descr *r )
{
   int count1=0, carry=0, tmp=0, sum=0, neg=0 ;
   int lsd=0 ; /* least significant digit */
   int msd=0, loan=0, ccns=0 ;
   int flog=0, fexp=0, fsize=0, slog=0, ssize=0, sexp=0, sdiff=0, fdiff=0 ;
   char *fnum=NULL, *snum=NULL ;
   mat_tsd_t *mt;

   mt = TSD->mat_tsd;

   fexp = f->exp ;
   fsize = f->size ;
   sexp = s->exp ;
   ssize = s->size ;
   flog = f->negative & !s->negative;
   slog = s->negative & !f->negative;
   sdiff = sexp - ssize ;
   fdiff = fexp - fsize ;
   fnum = f->num ;
   snum = s->num ;

   /*
    * Make sure that we have enough space for the internal use.
    */
   ccns = TSD->currlevel->currnumsize ;
   IS_AT_LEAST( mt->add_out, mt->add_outsize, ccns+2 ) ;
#ifdef TRACEMEM
   mt->outptr5 = mt->add_out ;
#endif

   /*
    * If *s is zero compared to *f under NUMERIC DIGITS, set it to zero
    * This also applies if *s is zero. TRL says that in that case, the
    * other number is to be returned.
    */
   if ((ssize==1)&&(snum[0]=='0'))
   {
      descr_copy( TSD, f, r ) ;
      return ;
   }

   /*
    * And do  the same thing for *f
    */
   if (( fsize==1)&&(fnum[0]=='0'))
   {
      descr_copy( TSD, s, r ) ;
      return ;
   }

   if (sexp > fexp)
   {
      if (sexp > fexp + ccns)
      {
         descr_copy( TSD, s, r ) ;
         return ;
      }
   }
   else
   {
      if (fexp > sexp + ccns)
      {
         descr_copy( TSD, f, r ) ;
         return ;
      }
   }

   /*
    * Find the exponent number for the most significant digit and the
    * least significant digit. 'size' is the size of the result, minus
    * any extra carry. 'count1' is the loop variable that iterates
    * through each digit.
    *
    * These initializations may look a bit complex, so there is a
    * description of what they really means, consider the following
    * addition:
    *
    *        xxxxx.xx
    *           yy.yyyy
    *
    * The 'lsd' is the fourth digit after the decimal point, and is
    * therefore set to -3. The 'msd' is the fifth digit before the
    * decimal point, and is therefore set to 5. The size is set to
    * the difference between them, that is 8.
    * The 'carry' and 'loan' are initially
    * cleared.
    *
    * Special consideration is taken, so that 'lsd' will never be more
    * so small that the difference between them are bigger than the
    * current precision.
    */
   msd = MAX( fexp, sexp ) ;
   lsd = MAX( msd-(TSD->currlevel->currnumsize+1), MIN( fdiff, sdiff));
   carry = loan = 0 ;

   /*
    * Loop through the numbers, from the 'lsd' to the 'msd', letting
    * 'count1' have the value of the current digit.
    */

#ifdef CHECK_MEMORY
   /* The faster (and correct) algorithm uses fnum- and snum-pointers which
      are initially set to perhaps illegal values. They become valid by
      an offset. This isn't correctly understood by the bounds checker.
      We use valid base pointers and a complex index here. See below for
      the faster code. WARNING: Changes should be done both here and in the
      '#else' - statement. FGC
    */
   for (count1=lsd; count1<msd; count1++ )
   {
      /*
       * The variable 'sum' collects the sum for the addition of the
       * current digit. This is done, in five steps. First, register
       * any old value stored in 'carry' or 'loan'.
       */
      sum = carry - loan ;

      /*
       * Then, for each of the two numbers, add its digit to 'sum'.
       * There are two considerations to be taken. First, are we
       * within the range of that number. Then what are the sign of
       * the number. The expression of the if statement checks for
       * the validity of the range, and the contents of the if
       * statement adds the digit to 'sum' taking note of the sign.
       */
      if (count1>=fdiff && fexp>count1)
         {
             tmp = fnum[fexp - 1 - count1] - '0';
             if (flog)
                sum -= tmp ;
             else
                sum += tmp ;
         }
/*          else
            fdiff = msd ;
 */
      /*
       * Repeat previous step for the second number
       */
      if (count1>=sdiff && sexp>count1)
         {
            tmp = snum[sexp - 1 - count1] - '0';
            if (slog)
               sum -= tmp ;
            else
               sum += tmp ;
         }
/*         else
            sdiff = msd ; */

      /*
       * If the sum is more than 9, we have a carry, then set 'carry'
       * and subtract 10. And similar, if the sum is less than 0,
       * set 'loan' and add 10.
       */
      if ((carry = ( sum > 9 )))
         sum -= 10 ;

      if ((loan = ( sum < 0 )))
         sum += 10 ;

      /*
       * Flush the resulting digit to the output string.
       */
      mt->add_out[ msd - count1 ] = (char) (sum + '0');
   }
#else
   fnum += fexp - 1 ;
   snum += sexp - 1 ;
   for (count1=lsd; count1<msd; count1++ )
   {
      /*
       * The variable 'sum' collects the sum for the addition of the
       * current digit. This is done, in five steps. First, register
       * any old value stored in 'carry' or 'loan'.
       */
      sum = carry - loan ;

      /*
       * Then, for each of the two numbers, add its digit to 'sum'.
       * There are two considerations to be taken. First, are we
       * within the range of that number. Then what are the sign of
       * the number. The expression of the if statement checks for
       * the validity of the range, and the contents of the if
       * statement adds the digit to 'sum' taking note of the sign.
       */
      if (count1>=fdiff && fexp>count1)
         {
             tmp = fnum[-count1] - '0';
             if (flog)
                sum -= tmp ;
             else
                sum += tmp ;
         }
/*          else
            fdiff = msd ;
 */
      /*
       * Repeat previous step for the second number
       */
      if (count1>=sdiff && sexp>count1)
         {
            tmp = snum[-count1] - '0';
            if (slog)
               sum -= tmp ;
            else
               sum += tmp ;
         }
/*         else
            sdiff = msd ; */

      /*
       * If the sum is more than 9, we have a carry, then set 'carry'
       * and subtract 10. And similar, if the sum is less than 0,
       * set 'loan' and add 10.
       */
      if ((carry = ( sum > 9 )) != 0)
         sum -= 10 ;

      if ((loan = ( sum < 0 )) != 0)
         sum += 10 ;

      /*
       * Flush the resulting digit to the output string.
       */
      mt->add_out[ msd - count1 ] = (char) (sum + '0');
   }
#endif

   neg = ( f->negative && s->negative ) ;
   IS_AT_LEAST( r->num, r->max, /*ccns+2*/ msd-lsd+3 ) ;

   fnum = r->num ;
   if ( carry )
   {
      *(fnum++) = '1' ;
   }
   else if ( loan )
   {
      int i ;
      assert( neg==0 ) ;
      neg = 1 ;
      mt->add_out[0] = '0' ;
      sum = 10 ;
      for ( i=msd-lsd; i>0; i-- )
      {
         if ((mt->add_out[i] = (char) (sum - (mt->add_out[i]-'0') + '0')) > '9')
         {
           mt->add_out[i] = '0' ;
           sum = 10 ;
         }
         else
           sum = 9 ;
      }
      snum = mt->add_out ;
      msd-- ;
   }
   else
   {
      msd-- ;
   }

   r->negative = neg ;
   r->exp = msd + 1 ;
   r->size = r->exp - lsd ;

#if 1
   memcpy( fnum, mt->add_out+1, r->size - ( (carry) ? 1 : 0 ) ) ;
#else
   memcpy( fnum, mt->add_out+1, r->size ) ;
#endif
   str_strip( r ) ;
/*   for (; count1<fsize; count1++)
      fnum[count1] = mt->add_out[count1]  ;
 */
}


streng *str_format_orig( const tsd_t *TSD, const streng *input, int before, int after, int expp, int expt )
{
   static char *out=NULL ;
   static int outsize=0 ;
   int sdigs=0, trigger=0, j=0, size=0, k=0 ;
   int decim, use_exp=0, use_eng=0 ;
   char *tmp_ptr=NULL ;
   streng *result=NULL ;

   char *in_ptr ; /* ptr to current char in input string */
   char *in_end ; /* ptr to end+1 in input string */
   int exponent ; /* the value of the exponent */
   char *out_ptr ; /* ptr to current char in output string */
   mat_tsd_t *mt;

   mt = TSD->mat_tsd;

   if (getdescr(TSD,input,&mt->fdescr))
       exiterror( ERR_BAD_ARITHMETIC, 0 )  ;

   str_round( &mt->fdescr, TSD->currlevel->currnumsize ) ;

   /*
    * Make sure that we have enough space available. If we have memory
    * tracing enabled, remember to save the info about the new memory
    * allocated.
    */
   IS_AT_LEAST( out, outsize, 3*TSD->currlevel->currnumsize+5 ) ;
#ifdef TRACEMEM
   mt->outptr1 = out ;
#endif

   new_round:
   /*
    * Str_ip leading zeros from the descriptor. We could have done this
    * by calling strip(), but it is faster to do it here. Besides doing
    * it this way shortcuts the need to shift (i.e copy) the digits
    */
   str_strip( &mt->fdescr ) ;
   exponent = mt->fdescr.exp ;
   in_end = &mt->fdescr.num[mt->fdescr.size] ;
   in_ptr = mt->fdescr.num ;

   /*
    * Now, let us see if we've got a zero. That one is a special case,
    * and then just return a zero, no more, no less
    */
/*
   if (in_ptr==in_end)
      return Str_creTSD( "0" ) ;
*/
   /*
    * There are certain limits that the .exp must be within, the 32
    * bit integer can hold slightly larger numbers, so if we are outside
    * the legal range, report an error.
    */
   if ((MAX_EXPONENT<exponent-1) || ((-MAX_EXPONENT)>exponent+1))
       exiterror( ERR_ARITH_OVERFLOW, 0 )  ;

   /*
    * Set 'sdigs' to the number of significant digits we want in the
    * answer. Whatever the user wants, we first round off 'in' to
    * the precision before doing anyting.
    */
   sdigs = MIN( in_end-in_ptr, TSD->currlevel->currnumsize ) ;  /* significant digits in answer */

   /* Are we in for exponential form? If the integer part is bigger
    * than a trigger value, or the decimal part bigger than twice that
    * trigger value, use exponential form. The default for trigger is
    * the precision, but it will be expt if that is defined.
    *
    * Problem: will the setting of before and after effect the
    * precision on such a way that it will change the representation
    * between exponential/simple form. (I don't think so?) The code
    * that considers 'after' has therefore been commented out. Consider
    * the problem format('0.00011111',,4,,3). Now, this number needs
    * eight digits after the decimal point in order to be represented.
    * On the other hand, it only need four digits, in the output.
    * Which is the correct???  I don't know, but I think the code should
    * be commented out.
    */
   trigger = (expt!=(-1)) ? expt : TSD->currlevel->currnumsize ;
   decim = MAX( sdigs - exponent, 0 ) ;

   use_exp = ((decim>2*trigger) || (trigger<exponent)) ;
   if (after>(-1))
      decim = after ;

   /* If expp is zero, then we will never use exponential form
    */
   if (expp==0)
      use_exp = 0 ;

   /* Here comes the big question, are we going to use exponential form
    * or simple form, 'use_exp' holds the answer
    */
   if (use_exp)
   {
      /* We are going to use exponential form, now we have to check
       * whether to use scientific or engineering form. In exponential
       * form, there are *always* an integer part, which size is 1 in
       * sci. form and 1-3 in eng. form. Now, there are a total of three
       * ways to do this: sci, eng and custom (i.e. before is set too).
       */
      use_eng = (TSD->currlevel->numform == NUM_FORM_ENG) ;

      /* If number is 99.995, we might have to reconsider the integer
       * part (even the length of the integer part) after rounding the
       * fractional part. So we better round it right away, and do
       * something sensible if order of magnitude changed.
       */
      k = mt->fdescr.exp ;
      if (after>(-1))
      {
         if (use_eng)
            str_round( &mt->fdescr, after + 1 + exponent%2 ) ;
         else
            str_round( &mt->fdescr, after + 1 ) ;

         if (k!=mt->fdescr.exp)
            goto new_round ;
      }

      /* If 'before' was specified, we need to initialize the first
       * chars in out to spaces, and set 'k' so we skip over them.
       */
      if (before!=(-1))
      {
         out_ptr = out + before - (mt->fdescr.negative!=0) - 1 ;
         if (use_eng)
            out_ptr -= (exponent)%3 ;

         /* Now, check that there is enough space, and set the initial
          * characters that we are not going to use to spaces.
          */
         if (out_ptr<out)
             exiterror( ERR_ARITH_OVERFLOW, 0 )  ;

         /* Then set the initial characters to space. When this is done,
          * we are sure (almost, at least) that there we have set k to
          * point to the 'right' char, so that after the sign and the
          * integer part of the matissa has been written out, k will be
          * at the k'th position of the output string.
          */
         for (tmp_ptr=out; tmp_ptr<out_ptr; *(tmp_ptr++)=' ') ;
      }
      else
         out_ptr = out ;

      /*
       * Let's start with the sign, that is not effected by any of the
       * various formats at which this routine can output 'in'
       */
      if (mt->fdescr.negative)
         *(out_ptr++) = '-' ;

      /* Then continue with the first digit, that shall *always* be
       * written out, both in sci and eng form.
       */
      assert( in_ptr < in_end ) ;
      *(out_ptr++) = *(in_ptr++) ;
      exponent-- ;

      /* If we use eng. form, we might have to stuff into it as much
       * as two additional digits. And we have to watch out so we neigher
       * increase the precision nor use more digits than we are allowed
       * to do.
       */
      if (use_eng)
      {
         for (;;)
         {
            /* Break out of this when the exponent is a multiple of three.
             */
            if ((exponent%3)==0)
               break ;

            /* First, check to see if there are more digits in the number
             * that we want to write out. If that is ok, then just write
             * the whole thing out.
             */
            if (in_ptr >= in_end)
                exiterror( ERR_ARITH_OVERFLOW, 0 )  ;
            *(out_ptr++) = *(in_ptr++) ;

            exponent-- ;
         }
      }

      /* OK, now we have written out the integer part of the matissa. Now
       * we must follow with the decimal part. First of all, let us start
       * with the decimal point, which should only be present if the
       * actually are a decimal part.
       */
      if (after==(-1))
         after = in_end - in_ptr ;

      if (after>0)
         *(out_ptr++) = '.' ;

      /* At last we have to to fill in the digits in the decimal part of
       * the matissa. Just loop through it.
       */
      for (; (after) && (in_ptr < in_end); after--)
         *(out_ptr++) = *(in_ptr++) ;

      /* And then we have to add the required number of tailing zeros in
       * the matissa
       */
      for (; after; after--)
         *(out_ptr++) = '0' ;

      /* Then comes the exponent. It should not be written if the
       * exponent is one. On the other hand, if a particular size is
       * requested for the exponent in expp, then the appropriate number
       * of space should be filled in.
       */
      if (exponent)
      {
         *(out_ptr++) = 'E' ;
         *(out_ptr++) = (char) ((exponent>0) ? '+' : '-') ;

         if (exponent<0)
            exponent = -exponent ;

         /* Now, suppose that expp is unspecified, then we would have to
          * find the number of characters needed for the exponent. The
          * following is a kludge to set expp to the 'right' number if
          * it was previously unset.
          */
         if (expp==(-1))
         {
            for (k=exponent,expp=0; k; k/=10, expp++)
               ;
         }

         /* We have to fill in numbers from the end of it, and pad with
          * zeros to the left. First find the end, and then loop backwards
          * for each significant digit in exponent
          */
         for (tmp_ptr=out_ptr+expp-1;;)
         {
            /* First check for overflow, i.e the space reserved in the
             * exponent is not enough to hold it.
             */
            if (tmp_ptr<out_ptr)
                exiterror( ERR_ARITH_OVERFLOW, 0 )  ;

            /* Then extract the decimal digit and put it into the output
             * string, while deviding the exponent by ten.
             */
            *(tmp_ptr--) = (char) (exponent%10 + '0') ;
            exponent /= 10 ;

            /* The only possible way out of this is when the exponent is
             * zero, i.e. it has been written out. (unless an error is
             * trapped in the lines above.)
             */
            if (!exponent)
               break ;
         }

         /* Now there might be some chars left in the exponent that has
          * to be set to leading zeros, check it and do it.
          */
         for (; tmp_ptr>=out_ptr;)
            *(tmp_ptr--) = '0' ;

         /* At last, set k, so that we know the real size of the out
          * string.
          */
         out_ptr += expp ;
      }
      else if (expp!=(-1))
      {
         for (j=(-2); j<expp; j++)
           *(out_ptr++) = ' ' ;
      }
   }
   else
   {
      /* If number is 99.995, we might have to reconsider the integer
       * part (even the length of the integer part) after rounding the
       * fractional part. So we better round it right away, and do
       * something sensible if order of magnitude changed.
       */
      after = decim ;
      k = mt->fdescr.exp ;
      if (after>(-1))
      {
         str_round( &mt->fdescr, after + mt->fdescr.exp ) ;
         if (k!=mt->fdescr.exp)
            goto new_round ;
      }

      out_ptr = out ;
      /* We are not going to use an exponent. Our number will consist of
       * two parts, the integer part, and the fractional part. Let us
       * concentrate on the integer part, which will have a particular
       * length if before is set.
       */
      if (before>(-1))
      {
         /* Since the integer part is going to have a particular length,
          * we better find that lenght, and skip the initial part (or
          * give an error if the length given is too small for this
          * number. Remember that we need at least on digit before the
          * decimal point (the leading zero).
          */
         size = (mt->fdescr.negative) + MAX(exponent, 1) ;

         /* Does the integer part of the output string hold enough chars?
          * If not, report an error. If is does, initialize the first
          * part of it (the unused part) to spaces.
          */
         if (size>before)
             exiterror( ERR_ARITH_OVERFLOW, 0 )  ;
         else
         {
            for (k=0; k<(before-size); k++)
               *(out_ptr++) = ' ' ;
         }
      }

      /* Write out the sign if it is needed, and then loop trough the
       * digits of the integer part of the number. If the integer part
       * in empty, write out a "0" instead.
       */
      if (mt->fdescr.negative)
         *(out_ptr++) = '-' ;

      if (exponent>0)
         for (; (exponent) && (in_ptr<in_end); exponent--)
            *(out_ptr++) = *(in_ptr++) ;
      else
         *(out_ptr++) = '0' ;

      /*
       * If the number doesn't have enough digits to fill the integer
       * part, help it, and fill it with zeros.
       */
      if (exponent>0)
      {
         for (; exponent; exponent--)
            *(out_ptr++) = '0' ;
      }

      /* Now has the time come for the decimal points, which is only
       * to be written out if there actually are decimals, and if
       * the size of the decimal part is non-zero. First find the
       * number of decimals, and stuff it into the 'size' variable
       */
      if (after>(-1))
         after = after ;
      else
         after = in_end - in_ptr ;

      /* If there are decimals, write the decimal point
       */
      if (after>0)
         *(out_ptr++) = '.' ;

      /* Then write the leading zeros after the decimal point, but
       * before the first digit in the number, if there are any
       */
      for (j=0; (j<after) && (exponent<0); exponent++, j++)
         *(out_ptr++) = '0' ;

      /* And then loop through the decimals, and write them out.
       * Remember that size might be larger than the actual number
       * of decimals available.
       */
      for (; (in_ptr<in_end) && (j<after); j++)
         *(out_ptr++) = *(in_ptr++) ;

      /* Then append the right number of zeros, to pad the number
       * to the wanted length. Here k is the length of the number
       */
      for (; j<after; j++)
         *(out_ptr++) = '0' ;
   }

   result = Str_makeTSD( out_ptr - out ) ;
   memcpy( result->value, out, result->len=out_ptr-out ) ;
   return result ;
}

streng *str_format(tsd_t *TSD, const streng *input, int Before,
                                                 int After, int Expp, int Expt)
/* According to ANSI X3J18-199X, 9.4.2, this function performs the BIF "format"
 * with extensions made by Brian.
 * I rewrote the complete function to allow comparing of this function code
 * to that one made in Rexx originally.
 * input is the first arg to "format" and may not be a number.
 * Before, After, Expp and Expt are the other args to this function and are
 * -1 if they are missing value.
 * FGC
 */
{
#define Enlarge(Num,atleast) if (Num.size + (atleast) > Num.max) {            \
                                char *new = MallocTSD(Num.size + (atleast) + 5); \
                                Num.max = Num.size + (atleast) + 5;           \
                                memcpy(new,Num.num,Num.size);                 \
                                FreeTSD(Num.num);                             \
                                Num.num = new;                                \
                             }
   char *buf;
   size_t bufsize;
   size_t bufpos;
   int ShowExp,Exponent,ExponentLen = 0,Afters,Sign,Point,OrigExpp,h;
   streng *retval;
   char ExponentBuffer[80]; /* enough even on a 256-bit-machine for an int */
   mat_tsd_t *mt;

   mt = TSD->mat_tsd;

   if (mt->OldRegina)
      return(str_format_orig(TSD, input, Before, After, Expp, Expt));

   /* Convert the input to a number and check if it is a number at all. */
   if (getdescr(TSD,input,&mt->fdescr))
      exiterror(ERR_INCORRECT_CALL, 11, "FORMAT", 1, tmpstr_of(TSD, input));

   /* Round the number according to NUMERIC DIGITS */
   str_round( &mt->fdescr, TSD->currlevel->currnumsize ) ;

   /* We have done the "call CheckArgs" of the ANSI function. */

   /* In the simplest case the first is the only argument. */
   if ((Before == -1) && (After == -1) && (Expp == -1) && (Expt == -1))
      return str_norm(TSD, &mt->fdescr, NULL);

   if (Expt == -1)
      Expt = TSD->currlevel->currnumsize;

   /* The number is already set up but check twice that we don't have leading
      zeros:
    */
   /*str_strip(&mt->fdescr); FGC: FIX: commented off for testing purpose */

   /* Trailing zeros are confusing, too: */
   while ((mt->fdescr.size > 1) && (mt->fdescr.num[mt->fdescr.size - 1] == '0'))
      mt->fdescr.size--;
   /* The number is normalized, now.
    * Usage of the variables:
    * mt->fdescr.num: Mantissa in the ANSI-standard and defined as usual, zeros
    *                 may be padded at the end but never at the start because:
    * mt->fdescr.exp: true exponent for the mantissa.
    * Exponent:       Used Exponent for the mantissa, e.g.:
    *                 mt->fdescr.num=1,mt->fdescr.exp=2 = 0.1E2 = 10E0
    *                 In this case Exponent may be 0 to reflect the exponent we
    *                 should display.
    * Point:          Defined in the standard but not used. It is obviously
    *                 equal to (mt->fdescr.exp-Exponent)
    * examples with both mt->fdescr.num=Mantissa="101" and mt->fdescr.exp=-2:
    *    Exponent=0:  output may be "0.00101"
    *    Exponent=-3: output may be "1.01E-3"
    */

   Exponent = mt->fdescr.exp;
   Sign = (mt->fdescr.negative) ? 1 : 0;
   /* Sign, Mantissa(mt->fdescr.num) and Exponent now reflect the Number.
      Keep in mind that the Mantissa of a num_descr is always normalized to
      a value smaller than 1. Thus, mt->fdescr(num=1,exp=1) means 0.1E1=1)
    */

   /* Decide whether exponential form to be used, setting ShowExp.
      These tests have to be on the number before any rounding since
      decision on whether to have exponent affects what digits surround
      the decimal point.
      Only after this decision can Before and After arguments be checked.
    */
   ShowExp = 0;
   /* There is a test about maximum number of digits in the part before
      the decimal point, if non-exponential is to be used.
      eg 123.4E+3 becomes 1234E+2 by point removal, 123400 non-floating
    */
   if (Exponent > Expt)
      ShowExp = 1;
   /* Also a test on digits after the point.
      If the Exponent is negative at this point in the calculation there
      is a possibility that the non-exponential form would have too many
      zeros after the decimal point.
      For classic this test is:
    */
   if (mt->ClassicFormat)
   {
      if ((-(Exponent - mt->fdescr.size) > 2*Expt) && (-Exponent >= 0))
         ShowExp = 1;
   }
   else /* For modern it is: */
   {
      /* The non-exponential value needs to be at least a millionth. */
      if (-Exponent >= 6)
         ShowExp = 1;
   }
   /* An over-riding rule for exponential form: */
   if (Expp == 0)
      ShowExp = 0;

   /* ShowExp now indicates whether to show an exponent. */
   if (ShowExp)
   {
      /* Construct the exponential part of the result.
         Exponent at this point in the calculation reflects an integer
         mantissa.  It has to be changed to reflect a decimal point at
         Point from the left. */
      /* set the Point to 1 as required for 'SCIENTIFIC': */
      Exponent--;
      h = Exponent % 3;
      if ((TSD->currlevel->numform == NUM_FORM_ENG) && h)
      {
         if (h < 0) /* integer division may return values < 0 */
            h += 3;
         Enlarge(mt->fdescr,h);
         memset(mt->fdescr.num + mt->fdescr.size,'0',h);
         mt->fdescr.size += h;
         Exponent -= h;
      }
   }
   else /* Force the exponent to zero for non-exponential format. */
   {
      if (Exponent > mt->fdescr.size)
      {
         Enlarge(mt->fdescr,Exponent - mt->fdescr.size);
         memset(mt->fdescr.num + mt->fdescr.size,'0',Exponent - mt->fdescr.size);
         mt->fdescr.size = Exponent; /* mt->fdescr.size += Exponent - mt->fdescr.size; */
      }
      /* else add zeros at the start of the Mantissa in the standard */
      Exponent = 0;
      /* Now Exponent is Zero and Mantissa with Point reflects Number */
   }

   /* Deal with right of decimal point first since that can affect the
      left. Ensure the requested number of digits there.
    */
   Afters = mt->fdescr.size - (mt->fdescr.exp - Exponent);
   assert(Afters >= 0);
   if (After == -1)
      After = Afters;  /* Note default. */
   /* Make Afters match the requested After */
   if (Afters < After)
   {
      h = After - Afters;
      Enlarge(mt->fdescr,h);
      memset(mt->fdescr.num + mt->fdescr.size,'0',h);
      mt->fdescr.size += h;
   }
   else if (Afters > After)
   {
      /* Round by adding 5 at the right place. */
      h = mt->fdescr.exp - Exponent + After;
      /* h == position of overflowed digit or < 0 if it doesn't exist */
      if (h < 0) /* default it to zero, it's a special case of truncating */
      {
         mt->fdescr.size = 1;
         mt->fdescr.num[0] = '0'; /* always enough space for one character */
         Sign = 0;
         mt->fdescr.exp = -After + 1; /* let the mantissa been filled up later */
      }
      else if (mt->fdescr.num[h] >= '5')
      {
         mt->fdescr.size = h;
         /* Just in this case the value may change significantly! We leave
          * the normal transcription of the standard here to do it faster
          * and maybe better. Keep in mind some problems with rounding and
          * the awful ENGINEERING format.
          */
         for (--h;h >= 0;h--)
         {
            if (++mt->fdescr.num[h] <= '9')
               break;
            mt->fdescr.num[h] = '0';
         }
         if (h < 0) /* overflow! we have to add one in the front! */
         {
            Enlarge(mt->fdescr,1);
            memmove(mt->fdescr.num + 1,mt->fdescr.num,mt->fdescr.size);
            mt->fdescr.size++;
            mt->fdescr.num[0] = '1';
            mt->fdescr.exp++;
            if ( (mt->fdescr.exp - Exponent > Expt)
            &&    Expp != 0 ) /* bug 20000727-84858 */
               ShowExp = 1;
            if (ShowExp)
            {
               Exponent = mt->fdescr.exp - 1;
               h = Exponent % 3;
               if ((TSD->currlevel->numform == NUM_FORM_ENG) && h)
               {
                  if (h < 0) /* integer division may return values < 0 */
                     h += 3;
                  Exponent -= h;
               }
            }
         }
         for (h = mt->fdescr.size - 1;h >= 0;h--)
         {
            if (mt->fdescr.num[h] != '0')
               break;
            else if (h == 0) /* completely zero */
               Sign = 0;
         }
         /* The value itself is correct now but check the format: */
      }
      else /* rounding by truncating */
      {
         mt->fdescr.size = h;
         for (--h;h >= 0;h--)
         {
            if (mt->fdescr.num[h] != '0')
               break;
            else if (h == 0) /* completely zero */
               Sign = 0;
         }
      }
   } /* Rounded */
   /* That's all for now with the right part */

   /* Now deal with the part of the result before the decimal point. */
   Point = mt->fdescr.exp - Exponent; /* Point doesn't change never more */
   h = Point;
   if (h <= 0) /* missing front of the number? */
      h = 1; /* assume 1 char for "0" of "0.xxx" */
   if (Before == -1)
      Before = h + Sign; /* Note default. */
   /* Make Point match Before */
   if (h > Before - Sign)
      exiterror(ERR_INCORRECT_CALL, 38, "FORMAT", 2, tmpstr_of(TSD, input));

   /* We check the length of the exponent field, first. This allows to
    * allocate a sufficient string for the complete number.
    */
   OrigExpp = Expp;
   if (ShowExp)
   {
      if ((h = Exponent) < 0)
         h = -h;
      sprintf(ExponentBuffer,"%d",h);
      ExponentLen = strlen(ExponentBuffer);
      if (Expp == -1)
         Expp = ExponentLen;
      if (ExponentLen > Expp)
         exiterror(ERR_INCORRECT_CALL, 38, "FORMAT", 4, tmpstr_of(TSD, input));
   }
   else
      Expp = 0; /* don't format an exponent */

   bufsize = Before + After + Expp + 4; /* Point, "E+", term. zero */
   buf = MallocTSD(bufsize);

   /* Now do the formatting, it's a little bit complicated, since the parts
    * of the number may not exist (partially).
    */
   /* Format the part before the point */
   if (Point <= 0) /* denormalized number */
   {
      memset(buf,' ',Before - 1);
      buf[Before - 1] = '0';
      if (Sign)
         buf[Before - 2] = '-';
   }
   else
   {
      assert(Point <= mt->fdescr.size); /* before part should always fit */
      memset(buf,' ',Before - Point);
      memcpy(buf + Before - Point,mt->fdescr.num,Point);
      if (Sign)
         buf[Before - Point - 1] = '-';
   }
   bufpos = Before;

   /* Process the part after the decimal point */
   if (After > 0)
   {
      buf[bufpos++] = '.';
      if (Point < 0)
      { /* Denormalized mantissa, we must fill up with zeros */
         h = -Point;
         if (h > After)
            h = After; /* beware of an overrun */
         memset(buf + bufpos,'0',h);
         assert(After - h <= mt->fdescr.size);
         memcpy(buf + bufpos + h,mt->fdescr.num,After - h);
      }
      else
      {
         assert(Point + After <= mt->fdescr.size);
         memcpy(buf + bufpos,mt->fdescr.num + Point,After);
      }
      bufpos += After;
   }

   /* Finally process the exponent. ExponentBuffer contents the exponent
    * without the sign.
    */
   if (ShowExp)
   {
      if (Exponent == 0)
      {
         if (OrigExpp != -1)
         {
            memset(buf + bufpos,' ',Expp + 2);
            bufpos += Expp + 2;
         }
      }
      else
      {
         buf[bufpos++] = 'E';
         buf[bufpos++] = (char) ((Exponent < 0) ? '-' : '+');
         memset(buf + bufpos,'0',Expp - ExponentLen);
         memcpy(buf + bufpos + Expp - ExponentLen,ExponentBuffer,ExponentLen);
         bufpos += Expp;
      }
   }

   assert(bufpos < bufsize);
   buf[bufpos] = '\0';

   retval = Str_creTSD(buf);
   FreeTSD(buf);
   return retval;
#undef Enlarge
}


#define OPTIMIZE
#ifdef OPTIMIZE


streng *str_norm( const tsd_t *TSD, num_descr *in, streng *try )
{
   streng *result=NULL ;
   char frst=0 ;
   int i=0, j=0, k=0, p=0 ;
   int sdigs=0, top=0 ;
   int size=0, exp=0 ;
   mat_tsd_t *mt;

   mt = TSD->mat_tsd;

   IS_AT_LEAST( mt->norm_out, mt->norm_outsize, 3*TSD->currlevel->currnumsize+5 ) ;
#ifdef TRACEMEM
   mt->outptr2 = mt->norm_out ;
#endif

   if (in->negative)
      mt->norm_out[k++] = '-' ;

   /* remove effect of leading zeros in the descriptor */
   for ( i=0; i<in->size ; i++)
   {
      if ( in->num[i]!='0' )
         break ;
   }

   exp = in->exp ;
   if (i==in->size) /* mantissa contains zeros only */
   {
      in->size = in->exp = 1 ;
      in->negative = 0 ;
      in->num[0] = '0' ;
      if (try)
      {
         /*
          * Replace assert() with real code - MDW 30012002
         assert( try->max > 0 ) ;
         try->value[0] = '0' ;
         try->len = 1 ;
         */
         if (try->max)
         {
            try->value[0] = '0' ;
            try->len = 1 ;
         }
         else
         {
            Free_stringTSD( try ) ;
            try = Str_creTSD( "0" ) ;
         }
      }
      else
         try = Str_creTSD( "0" ) ;
      return try ;
   }
   else
     size = in->size ;

   if ((MAX_EXPONENT+1<exp) || ((-MAX_EXPONENT-1)>exp)) /* too late here! FGC */
   {
      exiterror( ERR_ARITH_OVERFLOW, 0 )  ;
      return NULL ;
   }

   exp -= top = i ;               /* adjust the exponent */

   /* Again, it is too late for an adjustment to digits() here. The user may
    * have changed digits() in between. The rounding should have taken place
    * after the math operation (which happens) and that's all.
    * Benefit: We don't have to round and therefore can't change the
    * exponent by rounding. FGC
    */
   sdigs = MIN( size-i, TSD->currlevel->currnumsize ) ;  /* significant digits in answer */

   if ((exp <= -6) || ( exp > TSD->currlevel->currnumsize )) /* ANSI */
   {
      /* Too late for rounding, must be fixed! FGC */
      if ((size-i > TSD->currlevel->currnumsize) && (in->num[TSD->currlevel->currnumsize+i] > '4'))
      {
         for (j=TSD->currlevel->currnumsize+i-1; j>=i; j--)
         {
            if (((in->num[j])++)=='9')
               in->num[j] = '0' ;
            else
               break ;
         }
         if (j<i)
         {
            in->num[i] = '1' ;
            exp++ ;
         }
      }

      frst = mt->norm_out[k++] = (char) (( i<size ) ? in->num[i++] : '0') ;
      if (i<size && (1<=TSD->currlevel->currnumsize-(frst!='0')))
         mt->norm_out[k++] = '.' ;

      for (j=1; (i<size) && (j<=TSD->currlevel->currnumsize-(frst!='0')); j++ )
         mt->norm_out[k++] = in->num[i++] ;

      /* avrunding */
      j = exp - 1 ;
      mt->norm_out[k++] = 'E' ;
      sprintf( &mt->norm_out[k], "%+d", j ) ;

      return Str_creTSD( mt->norm_out ) ;
   }

   if (exp <= 0)
   {
      mt->norm_out[k++] = '0' ;
      if (exp<0)
      {
         mt->norm_out[k++] = '.' ;
         for ( j=0; j>exp; j-- )
            mt->norm_out[k++] = '0' ;
      }
   }

   top = MIN( sdigs, TSD->currlevel->currnumsize ) + i ;
   if ((size-i > TSD->currlevel->currnumsize) && (in->num[TSD->currlevel->currnumsize+i] > '4'))
   {
      /* Too late for rounding, must be fixed! FGC */
      for (j=TSD->currlevel->currnumsize+i-1; j>=i; j--)
      {
         if (((in->num[j])++)=='9')
            in->num[j] = '0' ;
         else
            break ;
      }
      if (j<i)
      {
         top-- ;
         p += mt->norm_out[k++] = '1' ;
      }
   }

   j = exp ;
   for (; i<top; i++ )
   {
      if (j--==0)
         mt->norm_out[k++] = '.' ;

      p += (mt->norm_out[k++] = in->num[i]) - '0' ;
   }
   while (j-- > 0)             /* Fill possibly lost zeros */
      mt->norm_out[k++] = '0';

   if (p==0)
      mt->norm_out[(k=1)-1] = '0' ;

   if (try)
   {
      if (try->max>=k)
          result = try ;
      else
      {
          result = Str_makeTSD( k ) ;
          Free_stringTSD( try ) ;
      }
   }
   else
      result = Str_makeTSD( k ) ;

   memcpy( result->value, mt->norm_out, result->len=k ) ;
   return result ;
}

#else


streng *str_norm( const tsd_t *TSD, num_descr *in, streng *try )
{
   streng *result=NULL ;
   char frst=0 ;
   int i=0, j=0, k=0, p=0 ;
   int sdigs=0, top=0 ;
   int exp=0, size=0, neg=0 ;
   mat_tsd_t *mt;

   mt = TSD->mat_tsd;

   IS_AT_LEAST( mt->norm_out, mt->norm_outsize, 3*TSD->currlevel->currnumsize+5 ) ;
#ifdef TRACEMEM
   mt->outptr2 = mt->norm_out ;
#endif

   if (in->negative)
      mt->norm_out[k++] = '-' ;

   /* remove effect of leading zeros in the descriptor */
   for ( i=0; i<in->size ; i++)
   {
      if ( in->num[i]!='0' )
         break ;
   }
   exp = in->exp ;
   size = in->size ;
   neg = in->negative ;
   if (i==size)
     size = exp = neg = 0 ;

   if ((MAX_EXPONENT+1<exp) || ((-MAX_EXPONENT)-1>exp))
       exiterror( ERR_ARITH_OVERFLOW, 0 )  ;

   exp -= top = i ;               /* adjust the exponent */
   sdigs = MIN(size-i,TSD->currlevel->currnumsize); /* significant digits in answer */

   if (( sdigs - exp > 2*TSD->currlevel->currnumsize ) || ( sdigs < exp ))
   {

      if ((size-i > TSD->currlevel->currnumsize) && (in->num[TSD->currlevel->currnumsize+i] > '4'))
      {
         for (j=TSD->currlevel->currnumsize+i-1; j>=i; j--)
         {
            if (((in->num[j])++)=='9')
               in->num[j] = '0' ;
            else
               break ;
         }
         if (j<i)
         {
            in->num[i] = '1' ;
            exp++ ;
         }
      }


      frst = mt->norm_out[k++] = ( i<size ) ? in->num[i++] : '0' ;
      if (i<size)
         mt->norm_out[k++] = '.' ;

      for (j=1; (i<size) && (j<=TSD->currlevel->currnumsize-(frst!='0')); j++ )
         mt->norm_out[k++] = in->num[i++] ;

      /* avrunding */
      j = exp - 1 ;
      mt->norm_out[k++] = 'E' ;
      sprintf( &mt->norm_out[k], "%+d", j ) ;

      return Str_creTSD( mt->norm_out ) ;
   }

   if (exp <= 0)
   {
      mt->norm_out[k++] = '0' ;
      if (exp<0)
      {
         mt->norm_out[k++] = '.' ;
         for ( j=0; j>exp; j-- )
            mt->norm_out[k++] = '0' ;
      }
   }

   top = MIN( sdigs, TSD->currlevel->currnumsize ) + i ;
   if ((size-i > TSD->currlevel->currnumsize) && (in->num[TSD->currlevel->currnumsize+i] > '4'))
   {
      for (j=TSD->currlevel->currnumsize+i-1; j>=i; j--)
      {
         if (((in->num[j])++)=='9')
            in->num[j] = '0' ;
         else
            break ;
      }
      if (j<i)
      {
         top-- ;
         p += mt->norm_out[k++] = '1' ;
      }
   }

   j = exp ;
   for (; i<top; i++ )
   {
      if (j--==0)
         mt->norm_out[k++] = '.' ;

      p += (mt->norm_out[k++] = in->num[i]) - '0' ;
   }
   if (p==0)
      mt->norm_out[(k=1)-1] = '0' ;

   if (try)
   {
      if (try->max>=k)
          result = try ;
      else
      {
          result = Str_makeTSD( k ) ;
          Free_stringTSD( try ) ;
      }
   }
   else
      result = Str_makeTSD( k ) ;

   memcpy( result->value, mt->norm_out, result->len=k ) ;
   return result ;
}


#endif


int string_test( const tsd_t *TSD, const num_descr *first, const num_descr *second )
{
   int i=0, top=0, fnul=0, snul=0 ;
   char fchar=' ', schar=' ' ;

   if ( first->negative != second->negative )  /* have different signs */
      return ( first->negative ? -1 : 1 ) ;

   fnul = ( first->size==1 && first->exp==1 && first->num[0]=='0') ;
   snul = ( second->size==1 && second->exp==1 && second->num[0]=='0') ;
   if (fnul || snul)
   {
      if (fnul && snul) return 0 ;
      if (fnul) return (second->negative ? 1 : -1 ) ;
      else      return (first->negative ? -1 : 1 ) ;
   }

   if ( first->exp != second->exp ) /* have different order */
      return (log_xor( first->negative, first->exp>second->exp ) ? 1 : -1 ) ;

   /* same order and sign, have to compare TSD->currlevel->currnumsize-TSD->currlevel->numfuzz first */
   top = MIN( TSD->currlevel->currnumsize-TSD->currlevel->numfuzz, MAX( first->size, second->size )) ;
   for ( i=0; i<top; i++ )
   {
      fchar = (char) ((first->size > i) ? first->num[i] : '0') ;
      schar = (char) ((second->size > i) ? second->num[i] : '0') ;
      if ( fchar != schar )
         return log_xor( first->negative, fchar>schar ) ? 1 : -1 ;
   }

   /* hmmm, last resort: can the numbers be rounded to make a difference */
   fchar = (char) ((first->size > i) ? first->num[i] : '0') ;
   schar = (char) ((second->size > i) ? second->num[i] : '0') ;
   if (((fchar>'4') && (schar>'4')) || ((fchar<'5') && (schar<'5')))
      return 0 ;  /* equality! */

   /* now, one is rounded upwards, the other downwards */
   return log_xor( first->negative, fchar>'5' ) ? 1 : -1 ;
}



num_descr *string_incr( const tsd_t *TSD, num_descr *input )
{
   int last=0 ;
   char *cptr=NULL ;

   assert( input->size > 0 ) ;

   if (input->size != input->exp || input->exp >= TSD->currlevel->currnumsize)
   {
      static const num_descr one = { "1", 0, 1, 1, 1 } ;

      string_add( TSD, input, &one, input ) ;
      str_round(input,TSD->currlevel->currnumsize) ;
      return input ;
   }

   cptr = input->num ;
   last = input->size-1 ;

   for (;;)
   {
      if (input->negative)
      {
         if (cptr[last] > '1')
         {
            cptr[last]-- ;
            return input ;
         }
         else if (cptr[last]=='1')
         {
            cptr[last]-- ;
            if (last==0)
               str_strip( input ) ;
            return input ;
         }
         else
         {
            assert( cptr[last] == '0' ) ;
            assert( last ) ;
            cptr[last--] = '9' ;
         }
      }
      else
      {
         if (cptr[last] < '9')
         {
            cptr[last]++ ;
            return input ;
         }
         else
         {
            assert( cptr[last] == '9' ) ;
            cptr[last--] = '0' ;
         }
      }

      if (last<0)
      {
         if (input->size >= input->max)
         {
            char *new ;

            assert( input->size == input->max ) ;
            new = MallocTSD( input->max * 2 + 2 ) ;
            memcpy( new+1, input->num, input->size ) ;
            new[0] = '0' ;
            input->size++ ;
            input->exp++ ;
            input->max = input->max*2 + 2 ;
            FreeTSD( input->num ) ;
            cptr = input->num = new ;
         }
         else
         {
            memmove( input->num+1, input->num, input->size ) ;
            input->size++ ;
            input->exp++ ;
            input->num[0] = '0' ;
         }
         last++ ;
      }
   }
}



void string_div( const tsd_t *TSD, const num_descr *f, const num_descr *s, num_descr *r, num_descr *r2, int type )
{
   int ssize=0, tstart=0, tcnt=0, finished=0, tend=0 ;
   int i=0, cont=0, outp=0, test=0, loan=0 ;
   int origneg=0, origexp=0 ;
   mat_tsd_t *mt;

   mt = TSD->mat_tsd;

   IS_AT_LEAST( mt->div_out, mt->div_outsize, (TSD->currlevel->currnumsize+1)*2+1 ) ;
   IS_AT_LEAST( r->num, r->max, TSD->currlevel->currnumsize+1 ) ;
#ifdef TRACEMEM
   mt->outptr3 = mt->div_out ;
#endif

   ssize = MIN( s->size, TSD->currlevel->currnumsize+1 ) ;
   r->exp = 1 + f->exp - s->exp ;
   r->negative = log_xor( f->negative, s->negative ) ;

   /* initialize the pointers */
   tstart = 0 ;
   tend = tstart + MIN( f->size, TSD->currlevel->currnumsize+1 ) ;

   /* first, fill out tmp to the size of ssize */
   for (tcnt=tstart; tcnt< ssize; tcnt++ )
      mt->div_out[tcnt] = (char) ((tcnt<tend) ? f->num[tcnt] : '0') ;

   /* if the ssize first digits in f->num form a number which is smaller */
   /* than s->num, we must add an additional digit to f->num */

   for (i=0; i<ssize; i++)
   {
      if ( mt->div_out[i] > s->num[i] )
         break ;
      else if ( mt->div_out[i] < s->num[i] )
      {
         mt->div_out[tcnt] = (char) ((tcnt<tend) ? f->num[tcnt] : '0') ;
         tcnt++ ;
         r->exp-- ;
         break ;
      }
   }

   /* situation: s->num[1..ssize] contains the devisor, and the array   */
   /* div_out[tstart==0..tcnt-1] hold the (first part of the) devidend. The */
   /* array f->num[tcnt..tend-1] (which may be empty) holds the last    */
   /* part of the devidend                                              */

   /* then foreach resulting digit we want */
   for (outp=0; outp<TSD->currlevel->currnumsize+1 && !finished; outp++)
   {
      r->num[outp] = '0' ;
      if ((tcnt-tstart > ssize) && (mt->div_out[tstart]=='0'))
         tstart++ ;

      /* if this is integer division, and we have hit the decimal point... */
      if ((type!=DIVTYPE_NORMAL) && (outp>=r->exp))
      {
         finished = 1 ;
         continue ;
      }

      /* try to subtract as many times as possible */
      for (cont=1; cont; )
      {
         /* can we subtract one more time? */
         if (tcnt-tstart == ssize)
            for (i=0; i<ssize; i++ )
            {
               test = mt->div_out[tstart+i] - s->num[i] ;
               if (test<0)
                  cont = 0 ;
               if (test!=0)
                  break ;
            }

         /* if we can continue, subtract it */
         loan = 0 ;
         if (cont)
         {
            r->num[outp]++ ;
            for (i=0; i<ssize; i++)
            {
               /* When do M$ ever build usable compilers, sigh: */
               char h = (char) ( s->num[ssize-1-i] - '0' + loan ) ;
               mt->div_out[tcnt-1-i] = (char) (mt->div_out[tcnt-1-i] - h);
               if ((loan = (mt->div_out[tcnt-1-i] < '0'))!=0)
                  mt->div_out[tcnt-1-i] += 10 ;
            }
            if (loan)
            {
               /* decrement it and check for '0' */
               mt->div_out[tstart] -= 1 ;
               if ((tcnt-tstart > ssize) && (mt->div_out[tstart]=='0'))
                  tstart++ ;
            }

         }
      } /* for each possible subtraction */

      if ((tcnt-tstart > ssize) && (mt->div_out[tstart]=='0'))
         tstart++ ;

      /* do we have anything left of the devidend, only meaningful if   */
      /* all digits in original divident have been processed, it is     */
      /* also safe to assume that divident and devisor have equal size  */

      assert( tcnt-tstart == ssize ) ;
      mt->div_out[tcnt] = (char) ((tcnt<tend) ? f->num[tcnt] : '0') ;
      if (++tcnt > tend)
      {
         finished = 1 ;
         for (i=tstart; i<tcnt; i++)
         {
            if (mt->div_out[i]!='0')
            {
               finished = 0 ;
               break ;
            }
         }
      }

   } /* for each digit wanted in the result */

   origexp = f->exp ;
   origneg = f->negative ;

   if (type==DIVTYPE_BOTH) /* return both answers */
   {
      IS_AT_LEAST( r2->num, r2->max, outp ) ;

      memcpy(r2->num, r->num, outp) ;
      r2->negative = r->negative ;
      r2->size = r->size ;
      r2->exp = r->exp ;

      for (r2->size = outp; (r2->size > r2->exp) && (r2->size > 1); r2->size--)
      {
         if (r2->num[r2->size-1]!='0')
            break ;
      }
   }

   if ((type == DIVTYPE_REMINDER) || (type == DIVTYPE_BOTH))
   {
      /* we are really interested in the reminder, so swap things */
      for (i=0; i<MIN(MAX(tend,tcnt)-tstart,TSD->currlevel->currnumsize+1); i++ )
         r->num[i] = (char) (i<tcnt-tstart ? mt->div_out[tstart+i] : f->num[tstart+i]) ;

      r->size = outp = i ;
      r->exp = origexp - tstart ;
      r->negative = origneg ;
   }

   /* then, at the end, we have to strip of trailing zeros that come */
   /* after the decimal point, first do we have any decimals?        */
   for (r->size = outp; (r->size > r->exp) && (r->size > 1); r->size--)
   {
      if (r->num[r->size-1]!='0')
         break ;
   }
}




/* The multiplication table for two single-digits numbers */
static const char mult[10][10][3] = {
   { "00", "00", "00", "00", "00", "00", "00", "00", "00", "00" },
   { "00", "01", "02", "03", "04", "05", "06", "07", "08", "09" },
   { "00", "02", "04", "06", "08", "10", "12", "14", "16", "18" },
   { "00", "03", "06", "09", "12", "15", "18", "21", "24", "27" },
   { "00", "04", "08", "12", "16", "20", "24", "28", "32", "36" },
   { "00", "05", "10", "15", "20", "25", "30", "35", "40", "45" },
   { "00", "06", "12", "18", "24", "30", "36", "42", "48", "54" },
   { "00", "07", "14", "21", "28", "35", "42", "49", "56", "63" },
   { "00", "08", "16", "24", "32", "40", "48", "56", "64", "72" },
   { "00", "09", "18", "27", "36", "45", "54", "63", "72", "81" },
} ;


void string_mul( const tsd_t *TSD, const num_descr *f, const num_descr *s, num_descr *r )
{
   char *outp=NULL;
   const char *answer=NULL ;
   int i=0, sskip=0, fskip=0, sstart=0, fstart=0, base=0, offset=0, carry=0, j=0 ;
   mat_tsd_t *mt;

   mt = TSD->mat_tsd;

   IS_AT_LEAST( mt->mul_out, mt->mul_outsize, 2*(TSD->currlevel->currnumsize+1) ) ;
#ifdef TRACEMEM
   mt->outptr4 = mt->mul_out ;
#endif

   for (i=0; i<2*(TSD->currlevel->currnumsize+1); mt->mul_out[i++]='0') ;
   outp = &mt->mul_out[2*(TSD->currlevel->currnumsize+1)-1] ;

   for (sskip=0; (sskip<s->size) && (s->num[sskip]=='0'); sskip++ ) ;
   sstart = MIN( sskip+TSD->currlevel->currnumsize+1, s->size-1 ) ;

   for (fskip=0; (fskip<f->size) && (f->num[fskip]=='0'); fskip++ ) ;
   fstart = MIN( fskip+TSD->currlevel->currnumsize+1, f->size-1 ) ;

   base = 2*(TSD->currlevel->currnumsize+1)-1 ;
   offset = carry = 0 ;
   for ( i=sstart; i>=sskip; i-- )
   {
      offset = carry = 0 ;
      assert( base >= 0 ) ;
      for ( j=fstart; j>=fskip; j-- )
      {
         answer = mult[f->num[j]-'0'][s->num[i]-'0'] ;
         assert( base-offset >= 0 ) ;
         /* Stupid MSVC likes this only: */
         mt->mul_out[base-offset] = (char) (mt->mul_out[base-offset] +
                                            answer[1] - '0' + carry) ;
         carry = answer[0] - '0' ;
         for (; mt->mul_out[base-offset]>'9'; )
         {
            mt->mul_out[base-offset] -= 10 ;
            carry++ ;
         }
         offset++ ;
      }
      if (base-offset >= 0)
         mt->mul_out[base-offset++] = (char) (carry + '0') ;
      else
         exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__, "" )  ;

      base-- ;
   }

   IS_AT_LEAST( r->num, r->max, /*2*(TSD->currlevel->currnumsize+1)*/
                              outp-mt->mul_out-base+offset  ) ;
   j = 0 ;
   for (i=base-offset+2; (i<=outp-mt->mul_out); i++ )
      r->num[j++] = mt->mul_out[i] ;

   if (j==0)
   {
      r->num[j++] = '0' ;
      r->exp = 1 ;
   }
   else
      r->exp = s->exp + f->exp ;

   r->negative = log_xor( f->negative, s->negative ) ;
   r->size = j ;
   str_round( r, TSD->currlevel->currnumsize ) ;
}


static void descr_strip( const tsd_t *TSD, const num_descr *from, num_descr *to )
{
   int i=0, j=0 ;

   IS_AT_LEAST( to->num, to->max, TSD->currlevel->currnumsize+1 ) ;

   to->negative = from->negative ;
   for (i=0; (i<from->size) && (from->num[i]=='0'); i++ ) ;
   to->exp = from->exp - i ;
   for (j=0; j+i<from->size; j++ )
      to->num[j] = from->num[i+j] ;

   if ((to->exp-1 > MAX_EXPONENT) || ( -MAX_EXPONENT > to->exp+1))
      exiterror( ERR_ARITH_OVERFLOW, 0 )  ;

   to->size = j ;
}



void string_pow( const tsd_t *TSD, const num_descr *num, int power, num_descr *acc, num_descr *res )
{
   static const num_descr one = { "1", 0, 1, 1, 2 } ;
   int ineg=0, pow=0, cnt=0 ;

   IS_AT_LEAST( res->num, res->max, TSD->currlevel->currnumsize+1 ) ;
   IS_AT_LEAST( acc->num, acc->max, TSD->currlevel->currnumsize+1 ) ;

   acc->exp = 1 ;
   acc->size = 1 ;
   acc->negative = 0 ;
   acc->num[0] = '1' ;

   if (power < 0)
   {
      power = -power ;
      ineg  = 1 ;
   }
   pow = power;

   for (cnt=0; pow; cnt++ )
      pow = pow>>1 ;

   for ( ;cnt ; )
   {
      if (power & (1<<(cnt-1)))
      {
         /* multiply acc with *f, and put answer into acc */
         string_mul( TSD, acc, num, res ) ;
         assert( acc->size <= acc->max && res->size <= res->max ) ;
         descr_strip( TSD, res, acc ) ;
         assert( acc->size <= acc->max && res->size <= res->max ) ;
      }

      if ((--cnt)==0)
         break ;   /* horrible example of dataflow */

      /* then, square the contents of acc */
      string_mul( TSD, acc, acc, res ) ;
      assert( acc->size <= acc->max && res->size <= res->max ) ;
      descr_strip( TSD, res, acc ) ;
      assert( acc->size <= acc->max && res->size <= res->max ) ;
   }

   if (ineg)
      /* may hang if acc==zero ? */
      string_div( TSD, &one, acc, res, NULL, DIVTYPE_NORMAL ) ;
   else
      descr_strip( TSD, acc, res ) ;
   assert( acc->size <= acc->max && res->size <= res->max ) ;
}


/* ========= interface routines to the arithmetic routines ========== */

streng *str_add( const tsd_t *TSD, const void *descr, const streng *second )
{
   mat_tsd_t *mt;

   mt = TSD->mat_tsd;
   if (getdescr( TSD, second, &mt->sdescr ))
      exiterror( ERR_BAD_ARITHMETIC, 0 )  ;

   string_add( TSD, descr, &mt->sdescr, &mt->rdescr ) ;
   return str_norm( TSD, &mt->rdescr, NULL ) ;
}

int descr_sign( const void *descr )
{
   return( ((num_descr*)descr)->negative ? -1 : 1 ) ;
}


void free_a_descr( const tsd_t *TSD, num_descr *in )
{
   assert( in->size <= in->max ) ;

   if ( in->num )
      FreeTSD( in->num ) ;

   FreeTSD( in ) ;
}


num_descr *get_a_descr( const tsd_t *TSD, const streng *num )
{
   num_descr *descr=NULL ;

   descr=MallocTSD( sizeof(num_descr)) ;
   descr->max = 0 ;
   descr->num = NULL ;

   if (getdescr( TSD, num, descr ))
       exiterror( ERR_BAD_ARITHMETIC, 0 )  ;

   return (void*)descr ;
}


int str_true( const tsd_t *TSD, const streng *input )
{
   mat_tsd_t *mt;

   mt = TSD->mat_tsd;
   if (input->len != 1)
       exiterror( ERR_UNLOGICAL_VALUE, 0 )  ;

   switch (input->value[0])
   {
      case '1':
         return 1 ;
      case '0':
         return 0 ;
      default:
          exiterror( ERR_UNLOGICAL_VALUE, 0 )  ;
   }

   /* Too keep the compiler happy */
   return 1 ;
}


streng *str_abs( const tsd_t *TSD, const streng *input)
{
   mat_tsd_t *mt;

   mt = TSD->mat_tsd;
   if (getdescr(TSD,input,&mt->fdescr))
       exiterror( ERR_BAD_ARITHMETIC, 0 )  ;

   mt->fdescr.negative = 0 ;
   return str_norm( TSD, &mt->fdescr, NULL ) ;
}


streng *str_trunc( const tsd_t *TSD, const streng *number, int deci )
{
   int i=0, j=0, k=0, size=0, top=0 ;
   streng *result=NULL ;
   mat_tsd_t *mt;

   mt = TSD->mat_tsd;

   /* first, convert number to internal representation */
   if (getdescr( TSD, number, &mt->fdescr ))
       exiterror( ERR_BAD_ARITHMETIC, 0 )  ;

   /* get rid of possible excessive precision */
   if (mt->fdescr.size>TSD->currlevel->currnumsize)
      str_round( &mt->fdescr, TSD->currlevel->currnumsize ) ;

   /* who big must the result string be? */
   if ((i=mt->fdescr.exp) > 0 )
      size = mt->fdescr.exp + deci ;
   else
      size = deci ;

   /*
    * Adrian Sutherland <adrian@dealernet.co.uk>
    * Changed the following line from '+ 2' to '+ 3',
    * because I was getting core dumps ... I think that we need this
    * because negative numbers BIGGER THAN -1 need a sign, a zero and
    * a decimal point ... A.
    */
   result = Str_makeTSD( size + 3 ) ;  /* allow for sign and decimal point */
   j = 0 ;

   if (mt->fdescr.negative)
      result->value[j++] = '-' ;

   /* first fill in the known numerals of the integer part */
   top = MIN( mt->fdescr.exp, mt->fdescr.size ) ;
   for (i=0; i<top; i++)
      result->value[j++] = mt->fdescr.num[i] ;

   /* pad out with '0' in the integer part, if necessary */
   for (k=i; k<mt->fdescr.exp; k++)
      result->value[j++] = '0' ;

   if (k==0)
      result->value[j++] = '0' ;

   k = 0 ;
   if (deci>0)
   {
      result->value[j++] = '.' ;

      /* pad with zeros between decimal point and number */
      for (k=0; k>mt->fdescr.exp; k--)
         result->value[j++] = '0' ;
   }

   /* fill in with the decimals, if any */
   top = MIN( mt->fdescr.size-mt->fdescr.exp, deci ) + i + k ;
   for (; i<top; i++ )
      result->value[j++] = mt->fdescr.num[i] ;

   /* pad with zeros if necessary */
   for (; i<deci+MIN(mt->fdescr.exp,mt->fdescr.size); i++ )
      result->value[j++] = '0' ;

   result->len = j ;
   assert( (result->len <= result->max) && (result->len<=size+2) ) ;
   return( result ) ;
}



/* ------------------------------------------------------------------
 * This function converts a packed binary string to a decimal integer.
 * It is equivalent of interpreting the binary string as a number of
 * base 256, and converting it to base 10 (the actual algorithm uses
 * a number of base 2, padded to a multiple of 8 digits). Negative
 * numbers are interpreted as two's complement.
 *
 * First parameter is the packed binary string; second parameter is
 * the number of initial characters to skip (i.e. the position of the
 * most significant byte in 'input'; the third parameter is a boolean
 * telling if this number is signed or not.
 *
 * The significance of the 'too_large' variable: If the number has
 * leading zeros, that is not an error, so the 'fdescr' might be set
 * to values larger than it can hold. However, the error occurs only
 * if that value is used. Therefore, if 'fdescr' becomes bigger than
 * the max whole number, 'too_large' is set. If attempts are made to
 * use 'fdescr' while 'too_large' is set, an error occurs.
 *
 * Note that this algoritm requires that string_mul and string_add
 * does not change anything in their first two parameters.
 *
 * The 'input' variable is assumed to have at least one digit, so don't
 * call this function with a null string. Maybe the compiler could
 * optimize this function better if [esf]descr were locals?
 */

streng *str_digitize( const tsd_t *TSD, const streng *input, int start, int sign )
{
   int cur_byte=0 ;     /* current byte in 'input' */
   int cur_bit=0 ;      /* current bit in 'input' */
   int too_large=0 ;    /* error flag (see above) */
   mat_tsd_t *mt;

   mt = TSD->mat_tsd;

   /* do we have anything to work on? */
   assert( start < Str_len(input) ) ;

   /* ensure that temporary number descriptors has enough space */
   IS_AT_LEAST( mt->fdescr.num, mt->fdescr.max, TSD->currlevel->currnumsize+2 ) ;
   IS_AT_LEAST( mt->edescr.num, mt->edescr.max, TSD->currlevel->currnumsize+2 ) ;
   IS_AT_LEAST( mt->sdescr.num, mt->sdescr.max, TSD->currlevel->currnumsize+2 ) ;

   /*
    * Initialize the temporary number descriptors: 'fdescr', 'sdescr'
    * and 'edescr'. They will be initialized to 0, 1 and 2 respectively.
    * They are used for:
    *
    *   fdescr: contains the value of the current bit of the current
    *           byte, e.g the third last bit in the last byte will
    *           have the value '0100'b (=4). This value is multiplied
    *           with two at each iteration of the inner loop. Is
    *           initialized to the value '1', and will have the same
    *           sign as 'input'.
    *
    *   sdescr: contains '2', to make doubling of 'fdescr' easy
    *
    *   edescr: contains the answer, initially set to '0' if 'input'
    *           is positive, or '-1' if 'input' is negative. The
    *           descriptor 'fdescr' is added to (or implicitly
    *           subtracted from) this number.
    */
   mt->fdescr.size     = mt->sdescr.size     = mt->edescr.size     = 1 ;
   mt->fdescr.negative = mt->sdescr.negative = mt->edescr.negative = 0 ;
   mt->fdescr.exp      = mt->sdescr.exp      = mt->edescr.exp      = 1 ;

   mt->edescr.num[0] = '0' ;   /* the resulting number */
   mt->fdescr.num[0] = '1' ;   /* the value of each binary digit */
   mt->sdescr.num[0] = '2' ;   /* the number to multiply 'fdescr' in */

   /*
    * If 'input' is signed, but positive, treat as if it was unsigned.
    * 'sign' is then effectively a boolean stating whether 'input' is
    * a negative number. In that case, 'edescr' should be set to '-1'.
    * Also, 'fdescr' is set to negative, so that it is subtracted from
    * 'edescr' when given to string_add().
    */
   if (sign)
   {
      if (input->value[start] & 0x80)
      {
         mt->edescr.num[0] = '1' ;
         mt->edescr.negative = 1 ;
         mt->fdescr.negative = 1 ;
      }
      else
         sign = 0 ;
   }

   /*
    * Each iteration of the outer loop will process a byte in 'input',
    * starting with the last (least significant) byte. Each iteration
    * of the inner loop will process one bit in the byte currently
    * processed by the outer loop.
    */
   for (cur_byte=Str_len(input)-1; cur_byte>=start; cur_byte--)
   {
      for (cur_bit=0; cur_bit<8; cur_bit++)
      {
         /* does the precision hold? if not, set flag */
         if (mt->fdescr.size > TSD->currlevel->currnumsize)
            too_large = 1 ;

         /*
          * If the current bit (the j'th bit in the i'th byte) is set
          * and input is positive; or if current bit is not set and
          * input is negative, then increase the value of the result.
          * This is not really a bitwise xor, but a logical xor, but
          * the values are always 1 or 0, so it doesn't matter.
          */
         if ((sign) ^ ((input->value[cur_byte] >> cur_bit) & 1))
         {
            if (too_large)
                exiterror( ERR_INVALID_INTEGER, 0 )  ;

            string_add( TSD, &mt->edescr, &mt->fdescr, &mt->edescr ) ;
         }

         /*
          * Str_ip away any leading zeros. If this is not done, the
          * accuracy of the operation will deter, since string_add()
          * return answer with leading zero, and the accumulative
          * effect of this would make 'edescr' zero after a few
          * iterations of the loop.
          */
         str_strip( &mt->edescr ) ;

         /*
          * Increase the value of 'fdescr', so that it corresponds with
          * the significance of the current bit in 'input'. But don't
          * do this if 'fdescr' isn't capable of holding that number.
          */
         if (!too_large)
         {
            string_mul( TSD, &mt->fdescr, &mt->sdescr, &mt->fdescr ) ;
            str_strip( &mt->fdescr ) ;
         }
      }
   }

   /* normalize answer and return to caller */
   return str_norm( TSD, &mt->edescr, NULL ) ;
}

streng *str_binerize( const tsd_t *TSD, const streng *number, int length )
{
   int i=0 ;
   streng *result=NULL ;
   char *res_ptr=NULL ;

   /*
    * We are going to need two number in this algoritm, so we can
    * just as well make them right away. We could initialize these on
    * the first invocation of this routine, and thereby saving some
    * space, but that would 1) take CPU on every invocation; 2) it
    * would probably cost just as much space in the text segment.
    * (Would have to set NUMERIC DIGIT to at least 4 before calling
    * getdescr with these.)
    */
   static const num_descr minus_one = {   "1", 1, 1, 1, 2 } ;
   static const num_descr byte      = { "256", 0, 3, 3, 4 } ;

   mat_tsd_t *mt;

   mt = TSD->mat_tsd;

   /*
    * First, let us convert the number into a descriptor. If that is
    * not possible, then we don't have a number, which is an error.
    */
   if (getdescr( TSD, number, &mt->edescr ))
      exiterror( ERR_BAD_ARITHMETIC, 0 )  ;

   /*
    * If the number is negative, then we *must* know how long it is,
    * since we are going to pad with 'ff'x bytes at the left up the
    * the wanted length. If we don't know the length, report an error.
    */
   if ((length==(-1)) && (mt->edescr.negative))
      exiterror( ERR_INCORRECT_CALL, 0 )  ;

   /*
    * Then we have to determine if this actually is a whole number.
    * There are two things that might be wrong: a non-zero fractional
    * part (checked in the 'else' part below; or a insufficient
    * precition (handled in the if part below).
    */
   if (mt->edescr.size < mt->edescr.exp)
   {
      exiterror( ERR_INVALID_INTEGER, 0 )  ;
   }
   else if (mt->edescr.size > mt->edescr.exp)
   {
      /*
       * Make sure that all digits in the fractional part is zero
       */
      for (i=MIN(mt->edescr.exp,0); i<mt->edescr.exp; i++)
      {
         if (mt->edescr.num[i]!='0')
         {
            exiterror( ERR_INVALID_INTEGER, 0 )  ;
         }
      }
   }

   /*
    * If the number is zero, a special case applies, the return value
    * is a nullstring. Same if length is zero. I am not sure if this
    * is needed, or if the rest of the algoritm is general enough to
    * handle these cases too.
    */
   if ((length==0) /* || ((mt->edescr.num[0]=='0') && (mt->edescr.size=1)) */)
      result=nullstringptr() ;

   /*
    * Here comes the real work. To ease the implementation it is
    * devided into two parts based on whether or not length is
    * specified.
    */
   else if (length==(-1))
   {
      /*
       * First, let's estimate the size of the output string that
       * we need. A crude (over)estimate is one char for every second
       * decimal digits. Also set length, just to chache the value.
       * (btw: isn't that MAX( ,0) unneeded? Since number don't have
       * a decimal part, and since it must have a integer part (else
       * it would be zero, and then trapped above.)
       */
      assert(mt->edescr.exp > 0) ;
      result = Str_makeTSD( (length=(MAX(mt->edescr.exp,0))/2) + 1 ) ;
      res_ptr = result->value ;


      /*
       * Let's loop from the least significant part of edescr. For each
       * iteration we devide mt->edescr by 256, stopping when edescr is
       * zero.
       */
      for (i=length; ; i--)
      {
         /*
          * Perform the integer divition, edescr gets the quotient,
          * while fdescr get the reminder. Afterwards, perform some
          * makeup on the numbers (that might not be needed?)
          */
         /* may hang if acc==zero ? */
         string_div( TSD, &mt->edescr, &byte, &mt->fdescr, &mt->edescr, DIVTYPE_BOTH ) ;
         str_strip( &mt->edescr ) ;
         str_strip( &mt->fdescr ) ;

         /*
          * Now, fdescr has the reminder, stuff it into the result string
          * before it escapes :-) (don't we have to cast lvalue here?)
          * Afterwards, check to see if there are more digits to extract.
          */
         result->value[i] = (char) descr_to_int( &mt->fdescr ) ;
         if ((mt->edescr.num[0]=='0') && (mt->edescr.size==1))
            break ;
      }

      /*
       * That's it, now we just have to align the answer and set the
       * correct length. Have to use memmove() since strings may
       * overlap.
       */
      memmove( result->value, &result->value[i], length+1-i ) ;
      result->len = length + 1 - i ;
   }
   else
   {
      /*
       * We do have a specified length for the number. At least that
       * makes it easy to deside how large the result string should be.
       */
      result = Str_makeTSD( length ) ;
      res_ptr = result->value ;

      /*
       * In the loop, iterate once for each divition of 256, but stop
       * only when we have reached the start of the result string.
       * Below, edescr gets the quotient and fdescr gets the reminder.
       */
      for (i=length-1; i>=0; i--)
      {
         /* may hang if acc==zero ? */
         string_div( TSD, &mt->edescr, &byte, &mt->fdescr, &mt->edescr, DIVTYPE_BOTH ) ;
         str_strip( &mt->edescr ) ;
         str_strip( &mt->fdescr ) ;

         /*
          * If the reminder is negative (i.e. quotient is negative too)
          * then add 256 to the reminder, to bring it into the range of
          * an unsigned char. To compensate for that, subtract one from
          * the quotient. Store the reminder.
          */
         if (mt->fdescr.negative)
         {
            /* the following two lines are not needed, but it does not
               work without them. */
            if ((mt->edescr.size==1) && (mt->edescr.num[0]=='0'))
               mt->edescr.exp = 1 ;

            string_add( TSD, &mt->edescr, &minus_one, &mt->edescr ) ;
            str_strip( &mt->edescr ) ;
            string_add( TSD, &mt->fdescr, &byte, &mt->fdescr ) ;
         }
         result->value[i] = (char) descr_to_int( &mt->fdescr ) ;
      }
      /*
       * That's it, store the length
       */
      result->len = length ;
   }

   /*
    * We're finished ... hope it works ...
    */
   return result ;
}


streng *str_normalize( const tsd_t *TSD, const streng *number )
{
   mat_tsd_t *mt;

   mt = TSD->mat_tsd;
   if (getdescr( TSD, number, &mt->fdescr ))
      exiterror( ERR_BAD_ARITHMETIC, 0 )  ;

   return str_norm( TSD, &mt->fdescr, NULL ) ;
}



num_descr *is_a_descr( const tsd_t *TSD, const streng *number )
{
   num_descr *new=NULL ;
   mat_tsd_t *mt;

   mt = TSD->mat_tsd;

   if (getdescr( TSD, number, &mt->fdescr ))
      return NULL ;

   new = MallocTSD( sizeof( num_descr )) ;
   new->max = 0 ;
   new->num = NULL ;

   descr_copy( TSD, &mt->fdescr, new ) ;
   return new ;
}


int myiswnumber( const tsd_t *TSD, const streng *number )
{
/*
 * Checks if number is a whole number according to NUMERIC DIGITS.
 * We DON'T check for NUMERIC FORM if the resulting string would have
 * a decimal point.
 */
   int i=0;
   mat_tsd_t *mt;

   mt = TSD->mat_tsd;

   if (getdescr(TSD, number, &mt->fdescr))
      return 0;
   str_round(&mt->fdescr, TSD->currlevel->currnumsize);
   i = mt->fdescr.exp;
   if (i <= 0)     /* 0.?? */
      return 0;
   for (; i < mt->fdescr.size; i++)
   {
      if (mt->fdescr.num[i] != '0')     /* decimal part not zero */
         return 0;
   }
   return 1;
}


/* Converts number to an integer. Sets *error to 1 on error (0 otherwise) */
int streng_to_int( const tsd_t *TSD, const streng *number, int *error )
{
   int result=0 ;
   int i=0 ;
   mat_tsd_t *mt;

   mt = TSD->mat_tsd;

   if (getdescr( TSD, number, &mt->fdescr ))
      goto errorout ;

   str_round( &mt->fdescr, TSD->currlevel->currnumsize ) ;
   if (mt->fdescr.exp > mt->fdescr.size)  /* precision of less than one */
      goto errorout ;

   i = mt->fdescr.exp ;
   if (i<0)
      i = 0 ;
   for (; i<mt->fdescr.size; i++)
   {
      if (mt->fdescr.num[i] != '0')  /* decimal part not zero */
         goto errorout ;
   }
   if (mt->fdescr.exp>9)
      goto errorout ; /* thus, a 32 bit integer should be sufficient */

   result = 0 ;
   for (i=0; i<mt->fdescr.exp; i++)
      result = result * 10 + (mt->fdescr.num[i]-'0') ;

   if (mt->fdescr.negative)
      result = -result ;

   *error = 0;
   return result ;
errorout:
   *error = 1;
   return(0);
}
