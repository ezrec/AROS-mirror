

#ifdef R2PERL

#include "rexx.h"
#include "stdio.h"

static void indent(tsd_t *TSD)
{
   int i ;

   for (i=0; i<TSD->indentsize; i++)
      putchar( ' ' ) ;
}


static void tabin(tsd_t *TSD)
{
   TSD->indentsize += 3 ;
}


static void tabout(tsd_t *TSD)
{
   TSD->indentsize -= 3 ;
}


void preamble()
{
   printf( "require \"r2perl.pl\" ;\n\n" ) ;
}


static void output( const char *cptr )
{
    printf( "%s", cptr ) ;
}


static void outint( int num )
{
    printf( "%d", num ) ;
}


static void outstr( const streng *str )
{
   int i ;

   for (i=0; i<str->len; i++)
      putchar( str->value[i] ) ;
}

static void expression( nodeptr this )
{
   nodeptr tptr ;

   if (!this)
   {
      return ;
   }

   switch (this->type)
   {
      case X_NULL:
         output( "\"\"" ) ;
         return ;

      case X_GT:
      case X_LT:
      case X_EQUAL:
         output( "(" ) ;
         expression( this->p[0] ) ;
         switch (this->type)
         {
            case X_EQUAL: output( "==" ) ; break ;
            case X_GT: output( "==" ) ; break ;
            case X_LT: output( "==" ) ; break ;
         }
         expression( this->p[1] ) ;
         output( ")" ) ;
         break ;

      case X_MODULUS:
      case X_DEVIDE:
      case X_MINUS:
      case X_PLUSS:
      case X_MULT:
         output( "(" ) ;
         expression( this->p[0] ) ;

         switch (this->type)
         {
            case X_MODULUS: output( "%" ) ; break ;
            case X_DEVIDE: output( "/" ) ; break ;
            case X_MULT:  output( "*" ) ; break ;
            case X_MINUS: output( "-" ) ; break ;
            case X_PLUSS: output( "+" ) ; break ;
         }
         expression( this->p[1] ) ;
         output( ")" ) ;
         break ;

      case X_IN_FUNC:
         output( "&" ) ;
         outstr( this->name ) ;
         output( "(" ) ;
         for (tptr=this->p[0]; tptr; tptr=tptr->p[1])
         {
            output( "(" ) ;
            expression( tptr->p[0] ) ;
            output( ")" ) ;
            if (tptr->p[1])
               output( "," ) ;
         }
         output( ")" ) ;
         break ;

      case X_STRING:
      case X_CON_SYMBOL:
         output( "\"" ) ;
         outstr( this->name ) ;
         output( "\"" ) ;
         break ;

      case X_SIM_SYMBOL:
         output( "$" ) ;
         outstr( this->name ) ;
         break ;

      case X_CONCAT:
      case X_SPACE:
         expression( this->p[0] ) ;
         if (this->type == X_SPACE)
            output( ",\" \"," ) ;
         else
            output( "," ) ;
         expression( this->p[1] ) ;
         break ;

      default:
         abort() ;

   }

   return ;
}



void translate( tsd_t *TSD, nodeptr this )
{
   int i ;

start_again:

   if (!this)
      return ;

   switch ( this->type )
   {
      case X_PROGRAM:
      case X_WHENS:
      case X_STATS:
      case X_OTHERWISE:
         translate( TSD, this->p[0] ) ;
         this = this->p[1] ;
         goto start_again ;


      case X_DO:
         indent(TSD) ;
         if ((!this->p[0]) && (!this->p[1]))
         {
            output( "{\n" ) ;
            tabin(TSD) ;
         }
         else if (this->p[0] && !this->p[0]->name)
         {
            output( "for ($loopcnt_" ) ;
            outint( TSD->loopcnt ) ;
            output( "=0; $loopcnt_" ) ;
            outint( TSD->loopcnt ) ;
            output( "<" ) ;
            expression( this->p[0]->p[1]->p[0] ) ;
            output( "; $loopcnt_" ) ;
            outint( TSD->loopcnt++ ) ;
            output( "++)\n" ) ;
            indent(TSD) ;
            output( "{\n" ) ;
            tabin(TSD) ;
         }
         else
         {
            output( "for ($" ) ;
            outstr( this->p[0]->name ) ;
            output( "=" ) ;
            expression( this->p[0]->p[0] ) ;
            output( "; " ) ;

            for (i=1; i<4; i++)
            {
               if (this->p[0]->p[i] && this->p[0]->p[i]->type == X_DO_TO)
               {
                  output( "$" ) ;
                  outstr( this->p[0]->name ) ;
                  output( "<=" ) ;
                  expression( this->p[0]->p[i]->p[0] ) ;
                  break ;
               }
            }
            output( "; " ) ;

            for (i=1; i<4; i++)
            {
               if (this->p[0]->p[i] && this->p[0]->p[i]->type == X_DO_BY)
               {
                  output( "$" ) ;
                  outstr( this->p[0]->name ) ;
                  output( "+=" ) ;
                  expression( this->p[0]->p[i]->p[0] ) ;
                  break ;
               }
            }
            output( ")\n" ) ;
            indent(TSD) ;
            output( "{\n" ) ;
            tabin(TSD) ;
         }

         if (this->p[1] && this->p[1]->type == X_WHILE)
         {
            indent(TSD) ;
            output( "if (" ) ;
            expression( this->p[1]->p[0] ) ;
            output( ") break ;\n" ) ;
         }

         translate( TSD, this->p[2] ) ;

         if (this->p[1] && this->p[1]->type == X_UNTIL)
         {
            indent(TSD) ;
            output( "if (" ) ;
            expression( this->p[1]->p[0] ) ;
            output( ") break ;\n" ) ;
         }

         tabout(TSD) ;
         indent(TSD) ;
         output( "}\n" ) ;
         break ;


       case X_IF:
         indent(TSD) ;
         output( "if (" ) ;
         expression( this->p[0] ) ;
         output( ")\n" ) ;
         indent(TSD) ;
         output( "{\n" ) ;
         tabin(TSD) ;
         translate( TSD, this->p[1] ) ;
         tabout(TSD) ;
         indent(TSD) ;
         output( "}\n" ) ;
         if ( this->p[2] )
         {
             indent(TSD) ;
             output( "else\n" ) ;
             indent(TSD) ;
             output( "{\n" ) ;
             tabin(TSD) ;
             translate( TSD, this->p[2] ) ;
             tabout(TSD) ;
             indent(TSD) ;
             output( "}\n" ) ;
         }
         break ;

      case X_ASSIGN:
      {
         indent(TSD) ;
         output( "$" ) ;
         outstr( this->name ) ;
         output( " = " ) ;
         expression( this->p[0] ) ;
         output( " ;\n" ) ;
         break ;
      }

/*
      case X_IPRET:
      {
         output( "eval( " ) ;
         output( expression( this->p[0] ) ) ;
         output( " )\n" ) ;
         break ;
      }

      case X_NO_OTHERWISE:
      {
         output( "else { die( "WHEN or OTHERWISE expected" ) ; }" ) ;
         break ;
      }

      case X_SELECT:
      {
         first_when = 1 ;
      }
         nstack[nstackptr++] = this->next ;
         nstack[nstackptr++] = this->p[1] ;
         this = this->p[0] ;
         goto fakerecurse ;

      case X_WHEN:
      {
         streng *tptr ;

         if (str_true(TSD,tptr=evaluate(TSD,this->p[0])))
         {
            Free_stringTSD( tptr ) ;
            nstackptr-- ; / * kill the OTHERWISE on the stack * /
            this = this->p[1] ;
            goto fakerecurse ;
         }
         Free_stringTSD( tptr ) ;
         break ;
      }
*/

      case X_SAY:
      {
         indent(TSD) ;
         output( "print " ) ;
         if (this->p[0])
         {
            expression( this->p[0] ) ;
            output( "," ) ;
         }
         output( "\"\\n\" ;\n" ) ;
         break ;
      }

      case X_TRACE:
      {
         break ;
      }

      case X_EXIT:
      {
         indent(TSD) ;
         output( "exit(" ) ;
         expression( this->p[0] ) ;
         output( " ) ;\n" ) ;
         break ;
      }

      case X_COMMAND:
      {
         indent(TSD) ;
         output( "system(" ) ;
         expression( this->p[0] ) ;
         output( " ) ;\n" ) ;
      }
/*
      case X_ADDR_N:   / * ADDRESS environment [expr] * /
      {
         streng *envir, *tmp, *rc ;
         int rc ;

         envir = this->name ;
         if (this->p[0])
         {
            tmp = evaluate(TSD, this->p[0]);
            rc = perform(tmp, envir, this->lineno);
            Free_stringTSD( tmp ) ;
            setvalue( TSD, &RC_name, rc ) ;
         }
         else
         {
            Free_stringTSD( TSD->currlevel->prev_env ) ;
            TSD->currlevel->prev_env = TSD->currlevel->environment ;
            TSD->currlevel->environment = Str_dupTSD(envir) ;
         }
         break ;
      }


      case X_ADDR_V:   / * ADDRESS [VALUE] expr * /
      {
         streng *cptr ;

         cptr = evaluate(TSD, this->p[0]) ;
         Free_stringTSD( TSD->currlevel->prev_env ) ;
         TSD->currlevel->prev_env = TSD->currlevel->environment ;
         TSD->currlevel->environment = cptr ;
         break ;
      }


      case X_ADDR_S:   / * ADDRESS * /
      {
         streng *tptr ;

         tptr = TSD->currlevel->environment ;
         TSD->currlevel->environment = TSD->currlevel->prev_env ;
         TSD->currlevel->prev_env = tptr ;
         break ;
      }


      case X_DROP:
      {
         nodeptr nptr ;
         for (nptr=this->p[0]; nptr; nptr=nptr->p[0] )
            if (nptr->name)
               if (nptr->type == X_SIM_SYMBOL)
                  drop_var( TSD, nptr->name ) ;
               else if (nptr->type == X_IND_SYMBOL)
               {
                  int begin,end;
                  streng *name,*value = shortcut(TSD,nptr) ;

                  /* Chop space separated words and drop them one by one */
                  for (end = 0;;)
                  {
                     begin = end; /* end of last word processed + 1 */
                     while ((begin < Str_len(value)) &&
                            isspace(value->value[begin]))
                        begin++;
                     if (begin == Str_len(value))
                        break;
                     end = begin + 1; /* find next separator */
                     while ((end < Str_len(value)) &&
                            !isspace(value->value[end]))
                        end++;
                     /* end now on space after word or past end of string */

                     name = Str_makeTSD(end - begin);
                     name->len = end - begin;
                     memcpy(name->value, value->value + begin, Str_len(name));

                     Str_upper(name);

                     drop_var( TSD, name ) ;
                     Free_stringTSD( name ) ;
                  }
               }
         break ;
      }

      case X_SIG_SET:
      case X_CALL_SET:
      {
         int type ;
         trap *traps = gettraps( TSD, TSD->currlevel ) ;

         / * which kind of condition is this? * /
         type = identify_trap( this->p[1]->type ) ;

         / * We always set this * /
         traps[type].invoked = (this->type == X_SIG_SET) ;
         traps[type].delayed = 0 ;
         traps[type].on_off = (this->p[0]->type == X_ON ) ;

         / * set the name of the variable to work on * /
         FREE_IF_DEFINED( traps[type].name ) ;
         if (this->name)
            traps[type].name = Str_dupTSD( this->name ) ;
         else if (this->p[0]->type == X_ON)
            traps[type].name = Str_creTSD( signalnames[type] ) ;

         break ;
      }

      case X_SIG_VAL:
      case X_SIG_LAB:
      {
         streng *cptr ;

         cptr = (this->name) ? Str_dupTSD(this->name) : evaluate( TSD, this->p[0] ) ;
         nstackptr = 0 ;
         for (;stackptr>0;stackptr--)
         {
            if (stack[stackptr-1].increment)
            {
               free_a_descr(TSD,stack[stackptr-1].increment) ;
               stack[stackptr-1].increment = NULL ;
            }

            if (stack[stackptr-1].stopval)
            {
               free_a_descr(TSD, stack[stackptr-1].stopval) ;
               stack[stackptr-1].stopval = NULL ;
            }
         }

         setvalue( TSD, var_sigl, int_to_streng( TSD, this->lineno )) ;
         entry = getlabel( TSD, cptr ) ;

         if (!this->name)
            Free_stringTSD( cptr ) ;

         if ((entry)==NULL) exiterror(16) ;
         this = entry->next ;
         goto fakerecurse ;
         break ;
      }
      case X_PROC:
         if (TSD->currlevel->varflag)
            exiterror( ERR_UNEXPECTED_PROC, 0 )  ;

         for (ptr=this->p[0];(ptr);ptr=ptr->p[0])
         if (ptr->name)
            expose_var(TSD,ptr->name) ;
         else
            exiterror( ERR_INTERPRETER_FAILURE, 0 )  ;

         expose_var(TSD,NULL) ;
         break ;

      case X_CALL:
      {
         this->u.node = getlabel(TSD,this->name) ;
         this->type = (this->u.node) ? X_IS_INTERNAL : X_IS_BUILTIN ;
      }

      case X_IS_INTERNAL:
      {
         paramboxptr targs ;

         if ( this->u.node )
         {
            setvalue( TSD, var_sigl, int_to_streng( TSD, this->lineno )) ;

            no_next_interactive = 1 ;
            targs = initplist( TSD, this ) ;
            oldlevel = TSD->currlevel ;
            TSD->currlevel = newlevel( TSD, TSD->currlevel ) ;
            TSD->currlevel->args = targs ;
            stackmark = pushcallstack( this ) ;

            result = interpret( TSD, this->u.node ) ;

            popcallstack( stackmark ) ;
            removelevel( TSD, TSD->currlevel ) ;
            TSD->currlevel = oldlevel ;
            TSD->currlevel->next = NULL ;

            if (result)
               setvalue( TSD, RESULT_name, result ) ;
            else
               drop_var( TSD, RESULT_name ) ;

            break ;
        }
     }

     case X_IS_BUILTIN:
     {
        if (!(result = buildtinfunc( TSD, this )))
            exiterror( ERR_ROUTINE_NOT_FOUND, 0 )  ;

        if (result)
           setvalue( TSD, RESULT_name, result ) ;
        else
           drop_var( TSD, RESULT_name ) ;

        break ;
      }

      case X_PARSE_ARG:
      case X_PARSE_ARG_U:
        args = TSD->currlevel->args->next ;
        (void)parseargtree( TSD, this, args, this->type!=X_PARSE_ARG ) ;
        break ;

      case X_PARSE_U:
      case X_PARSE:
         source = NULL ;
         switch (this->p[0]->type) {
             case X_PARSE_VAR:
                source = Str_dupTSD(shortcut( TSD, this->p[0] )) ;
/ *              source = Str_dupTSD(getvalue( TSD, this->p[0]->name, 1 )) ; * /
                break ;

             case X_PARSE_VAL:
                source = evaluate(TSD, this->p[0]->p[0]);
                break ;

             case X_PARSE_PULL:
                source = popline( TSD ) ;
                break ;

             case X_PARSE_VER:
                source = Str_creTSD(PARSE_VERSION_STRING) ;
                break ;

             case X_PARSE_EXT:
                source = readkbdline( TSD ) ;
                break ;

             case X_PARSE_SRC:
                origfile = TSD->systeminfo->called_as ;
                inpfile = TSD->systeminfo->input_file ;
                source = Str_makeTSD(15+Str_len(origfile)+Str_len(inpfile)) ;
                source->len = 0 ;
                Str_catstrTSD(source,"UNIX COMMAND ") ;
                Str_catTSD(source,inpfile) ;
                Str_catstrTSD(source," ") ;
                Str_catTSD(source,origfile) ;
                break ;
             }

        if (this->type==X_PARSE_U)
           (void)upcase(source) ;

        doparse( TSD, source, this->p[1] ) ;
        Free_stringTSD( source ) ;
        break ;
*/

      case X_PUSH:
        indent(TSD) ;
        output( "push( @rx_array, " ) ;
        expression( this->p[0] ) ;
        output( ") ;\n" ) ;
        break ;

      case X_PULL:
        indent(TSD) ;
        output( "pop( @rx_array, " ) ;
        expression( this->p[0] ) ;
        output( ") ;\n" ) ;
        break ;

      case X_QUEUE:
        indent(TSD) ;
        output( "unshift( @rx_array, " ) ;
        expression( this->p[0] ) ;
        output( ") ;\n" ) ;
        break ;

      case X_RETURN:
      {
         indent(TSD) ;
         output( "return( " ) ;
         if (this->p[0])
            expression( this->p[0] ) ;
         else
            output( "\"\"" ) ;
         output( " ) ;\n" ) ;
         break ;
      }

      case X_LEAVE:
      {
         if (this->name)
            output( "warn( \"symbolname ignored in LEAVE\n\") ;\n" ) ;

         indent(TSD) ;
         output( "break ;\n" ) ;
         break ;
      }

      case X_ITERATE:
      {
         if (this->name)
            output( "warn( \"symbolname ignored in ITERATE\n\") ;\n" ) ;

         indent(TSD) ;
         output( "continue ;\n" ) ;
         break ;
      }
/*
      case X_NUM_D:
      {
         streng *cptr = evaluate( TSD, this->p[0] ) ;
         TSD->currlevel->currnumsize = atopos( TSD, cptr ) ;
         Free_stringTSD( cptr ) ;
         break ;
      }

      case X_NUM_FUZZ:
      {
         streng *cptr = evaluate( TSD, this->p[0] ) ;
         TSD->currlevel->numfuzz = atozpos( TSD, cptr ) ;
         Free_stringTSD( cptr ) ;
         break ;
      }

      case X_NUM_F:
      {
         if (this->p[0]->type == X_NUM_SCI)
            TSD->currlevel->numform = NUM_FORM_SCI ;
         else if (this->p[0]->type == X_NUM_ENG)
            TSD->currlevel->numform = NUM_FORM_ENG ;
         else
             exiterror( ERR_INTERPRETER_FAILURE, 0 )  ;
         break ;
      }


      case X_LABEL:
      case X_NULL:
         break ;
*/
   }

}



#endif /* R2PERL */
