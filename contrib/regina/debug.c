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
#include <string.h>
#include <ctype.h>
#include <assert.h>

#ifndef NDEBUG

void dumpvars( const tsd_t *TSD, cvariableptr *hashptr )
{
/* change by bja: dumpvars to use stderr, as trace or listleaked do */
/* modified my mh: respect stdout_for_stderr OPTION also */
/* change by bja: display variable's length in front of value       */
   cvariableptr ptr=NULL, tptr=NULL ;
   int i=0, j=0, k=0 ;
   FILE *fp=stderr;

   if ( get_options_flag( TSD->currlevel, EXT_STDOUT_FOR_STDERR ) )
      fp = stdout;
   if (hashptr==NULL)
      hashptr = (cvariableptr *) TSD->currlevel->vars ;

   fprintf(fp,"\nDumping variables to <stdout>\n") ;
   for (i=0;i!=HASHTABLENGTH;i++)
   {
      if (hashptr[i]!=NULL)
         fprintf(fp,"   Variables from bin no %d\n",i) ;
      for (ptr=hashptr[i];ptr!=NULL;ptr=ptr->next)
      {
         if ((ptr->name->value)[ptr->name->len-1]=='.')
         {
#if 0
            fprintf(fp,"   >>> Stem    : //%s// Default: //%s//  Values:\n",
                  ptr->name->value,(ptr->value)?(ptr->value->value):"<none>") ;
#else
            fprintf(fp,"   >>> Stem    : ");
            fprintf(fp,"(%d) :",ptr->name->len);
            for (k=0;k<ptr->name->len;k++)
               putc(ptr->name->value[k],fp);
 /* avoid problems with exposed variables                                              bja */
            if (ptr->realbox)
            {
               fprintf(fp," Exposed!\n");
               continue;
            }
 /* end of avoid problems with exposed variables                                       bja */
            fprintf(fp, " Default: [");
            if (ptr->value)
            {
               fprintf(fp,"(%d) :",ptr->value->len);
               for (k=0;k<ptr->value->len;k++)
                  putc(ptr->value->value[k],fp);
            }
            else
               fprintf(fp, "<none>");
            fprintf(fp, "]  Values:\n");
#endif
            for (j=0;j<HASHTABLENGTH;j++)
            {
               if ((tptr=((ptr->index))[j])!=NULL)
               {
                  fprintf(fp, "      Sub-bin no %d\n",j) ;
                  for (;tptr;tptr=tptr->next)
                  {
                     if (tptr->name)
                     {
#if 0
                        fprintf(fp, "      >>> Variable: //%s// Value: //%s//\n",
                              tptr->name->value,tptr->value->value) ;
#else
                        fprintf(fp, "      >>> Tail: ");
                        fprintf(fp, "(%d) :",tptr->name->len);
                        for (k=0;k<tptr->name->len;k++)
                           putc(tptr->name->value[k],fp);
                        if ( tptr->num ) /* variable is a number */
                        {
/* try printing numbers a bit better...                                                bja
                           fprintf(fp, " (number: %s) ",ptr->num->num );
 */                                                                                  /* bja */
                           fprintf(fp, " (number: ");                             /* bja */
                           if (tptr->num->negative) fprintf(fp,"-");              /* bja */
                           fprintf(fp, "%.*s) ",tptr->num->size, tptr->num->num );/* bja */
                        }
                        fprintf(fp," Flag: %d hwired: %ld valid: %ld ",ptr->flag, ptr->hwired, ptr->valid );
                        fprintf(fp," Value: [");
                        if (tptr->value)
                        {
                           fprintf(fp,"(%d) :",tptr->value->len);
                           for (k=0;k<tptr->value->len;k++)
                              putc(tptr->value->value[k],fp);
                        }
                        else
                        {                                               /* added bja */
                           fprintf(fp,"<none>");                           /* added bja */
                        }
                        fprintf(fp,"]\n");
#endif
                     }
                  }
               }
            }
         }
         else
         {
#if 0
            fprintf(fp,"   >>> Variable: //%s// Value: //%s//\n",
                                     ptr->name->value,ptr->value->value) ;
#else
            fprintf(fp,"   >>> Variable: ");
            fprintf(fp,"(%d) :",ptr->name->len);
            for (k=0;k<ptr->name->len;k++)
               putc(ptr->name->value[k],fp);
 /* avoid problems with exposed variables                                              bja */
            if (ptr->realbox)
            {
               fprintf(fp," Exposed\n");
               continue;
            }
 /* end of avoid problems with exposed variables                                       bja */
            fprintf(fp," Value: [");
            if (ptr->value)
            {
               fprintf(fp,"(%d) :",ptr->value->len);
               for (k=0;k<ptr->value->len;k++)
                  putc(ptr->value->value[k],fp);
            }
            fprintf(fp,"]\n");
#endif
         }
      }
   }

   return ;
}


void dumptree(const tsd_t *TSD, const treenode *this, int level, int newline)
{
   int i=0, j=0 ;
   streng *ptr=NULL;
   FILE *fp=stderr;

   if ( get_options_flag( TSD->currlevel, EXT_STDOUT_FOR_STDERR ) )
      fp = stdout;
   if ((this->charnr)!=0
   &&  (this->charnr)!=(-1))
   {
      if (newline)
         putc('\n',fp);
      for (i=0;i!=level;i++) fprintf(fp,"  ") ;
      fprintf(fp,"Lineno: %d   Charno: %d", this->lineno, this->charnr) ;
      if (newline)
      {
         ptr = getsourceline(TSD, this->lineno, this->charnr,
                             &TSD->systeminfo->tree) ;
         fprintf(fp," Sourceline: [");
         for(i=0;i<ptr->len;i++)
            putc(ptr->value[i],fp);
         putc(']',fp);
      }
      putc('\n',fp);
   }

   for (i=0;i!=level;i++)
      fprintf(fp,"  ") ;
   fprintf(fp,">>>  in type=%d == %s\n", this->type, getsym(this->type)) ;

   for (i=0;i!=level;i++) fprintf(fp,"  ") ;
   fprintf(fp,"Flags: lnum %d rnum %d lsvar %d rsvar %d lcvar %d rcvar %d\n",
      this->u.flags.lnum,
      this->u.flags.rnum,
      this->u.flags.lsvar,
      this->u.flags.rsvar,
      this->u.flags.lcvar,
      this->u.flags.rcvar );

   if ((this->name)!=NULL)
   {
      for (i=0;i!=level;i++) fprintf(fp,"  ") ;
      fprintf(fp,"Name: [");
      for (i=0;i<this->name->len;i++)
         putc(this->name->value[i],fp);
      fprintf(fp,"]\n") ;
   }

   for (j=0;j<sizeof(this->p)/sizeof(this->p[0]);j++)
      if (this->p[j]!=NULL)
      {
         for (i=0;i!=level;i++)
            fprintf(fp,"  ") ;
         fprintf(fp,"==> (%d) going down in branch %d, type %d = %s\n",
                this->type,j+1,this->p[j]->type,getsym(this->p[j]->type)) ;
         dumptree( TSD, this->p[j], level+1, 0 ) ;
      }

   for (i=0;i!=level;i++)
      fprintf(fp,"  ") ;
   fprintf(fp,"<<< out type=%d == %s\n", this->type,getsym(this->type)) ;

   if (this->next)
      dumptree( TSD, this->next, level, 1 ) ;

}

#endif /* !NDEBUG */


#ifdef TRACEMEM
void marksource( clineboxptr ptr )
{
   for (;ptr;ptr=ptr->next) {
      markmemory( ptr->line,TRC_SOURCEL ) ;
      markmemory( (char *)ptr, TRC_SOURCE ) ; }
}
#endif


static const char *sourceline( int line, const internal_parser_type *ipt, unsigned *size)
{
   clineboxptr first;
   const otree *otp;

   if (ipt->first_source_line == NULL)
   { /* must be incore_source but that value may be NULL because of a failed
      * instore[0] of RexxStart!
      */
      otp = ipt->srclines; /* NULL if incore_source==NULL */
      while (otp && (otp->num < line)) {
         line -= otp->num;
         otp = otp->next;
      }
      if (otp == NULL)
      {
         *size = 0 ;
         return NULL ;
      }
      line--;
      *size = otp->elems[line].length ;
      return ipt->incore_source + otp->elems[line].offset ;
   }
   first = ipt->first_source_line;
   for (;first;)
   {
      if (first->lineno==line)
      {
         *size = first->line->len ;
         return first->line->value ;
      }
      else
         first = (first->lineno<line) ? first->next : first->prev ;
   }

   *size = 0 ;
   return NULL ;
}



streng *getsourceline( const tsd_t *TSD, int line, int charnr, const internal_parser_type *ipt )
{
   int dquote=0, squote=0 ;
   unsigned len ;
   streng *string ;
   const char *ptr, *chptr, *chend, *tmptr ;
   char *outptr ;

   assert( charnr>=0 ) ;
   if (!charnr)
     charnr++ ;

   ptr = sourceline(line,ipt,&len) ;
/*   assert( ptr ) ; */
   if (!ptr || (charnr >= len))
      return nullstringptr() ;

   chptr = ptr + --charnr ;
   chend = ptr + len ;
   for (; (chptr < chend) && isspace(*chptr); chptr++) ;
   string = Str_makeTSD(BUFFERSIZE+1) ;
   outptr = string->value ;

   for (;;)
   {
restart:
      if (chptr>=chend || outptr >= string->value + BUFFERSIZE)
         break ;

      if (!squote && *chptr=='\"')
         dquote = !dquote ;

      else if (!dquote && *chptr=='\'')
         squote = !squote ;

      else if (!(dquote || squote))
      {
         switch (*chptr)
         {
            case ',':
               for(tmptr=chptr+1; tmptr<chend && isspace(*tmptr); tmptr++ ) ;
               assert( tmptr<=chend ) ;
               if (tmptr==chend)
               {
                  *(outptr++) = ' ' ;
                  chptr = sourceline(++line,ipt,&len) ;
                  chend = chptr + len ;
                  for(; chptr<chend && isspace(*chptr); chptr++) ;
                  goto restart;
               }
               break ;

            case ':':
               *(outptr++) = *chptr ;

            case ';':
               goto endloop ;

         }
      }

      *(outptr++) = *(chptr++) ;
   }

endloop:
   assert( outptr - string->value <= BUFFERSIZE ) ;
   *outptr = '\0'; /* needs to be 0-terminated */
   string->len = outptr - string->value ;
   return string ;
}

