/*****************************************************************************
 * Modul                : main.c                                             *
 * Zweck                : Simplexalgorithmus                                 *
 * Autor                : Stefan Förster                                     *
 *                                                                           *
 * Datum      | Version | Bemerkung                                          *
 * -----------|---------|--------------------------------------------------- *
 * 14.03.1989 | 1.0     |                                                    *
 * 15.03.1989 |         | Zeitmessung mittels CurrentTime()                  *
 * 17.03.1989 | 1.1     | SetTaskPri() jetzt korrekt                         *
 *            |         | min cTx zugelassen                                 *
 * 18.03.1989 | 1.2     | Optional Ausgabe an ein File möglich (-f)          *
 *****************************************************************************/

#include <aros/oldprograms.h>
#include "simplex.h"

IMPORT USHORT       PhaseI(), PhaseII();
IMPORT BOOL         MPSX(), SearchExpr();
IMPORT INT          GetInput();
IMPORT SHORT        GetExpr();
IMPORT VOID         GiveMemBack(), GetRidOfLists(), PrintError(), Cap();


struct IntuitionBase *IntuitionBase = NULL;

DOUBLE	INFINITE = 1.0/0.0;  /* IEEE: NAN (Not A Number) */

FILE    *file[2] = { NULL };

TEXT    symbols[NUM_SYMBOLS][MAX_STRLEN+1];
BOOL    minimize = _FALSE; /* _TRUE, falls Zielfunktion minimiert wird */

ITEMPTR list[NUM_LISTS];
TEXT    args[BUFFER], line[BUFFER];

SHORT   m, n, *B = NULL, *Nq = NULL, *Nminus = NULL;
DOUBLE  c0, c0start;
DOUBLE  *A = NULL, *AB1 = NULL, *b = NULL, *b2q = NULL;
DOUBLE  *c = NULL, *c2 = NULL, *upper = NULL, *lower = NULL;
DOUBLE  *x = NULL, *cq = NULL, *pi = NULL, *dq = NULL, *help = NULL;


STRPTR errors[] = {
  (STRPTR)"Invalid arguments for \"load\"",
  (STRPTR)"Identifier too long",
  (STRPTR)"Identifier declared twice",
  (STRPTR)"Unknown Identifier",
  (STRPTR)"Sections have to start in column 1",
  (STRPTR)"Section defined twice",
  (STRPTR)"Unknown section",
  (STRPTR)"Illegal order of sections",
  (STRPTR)"Missing NAME-section (1st section) or NAME-section empty",
  (STRPTR)"Missing ROWS-section (2nd section) or ROWS-section empty",
  (STRPTR)"Missing goal or goal not found",
  (STRPTR)"Missing COLUMNS-section (3rd section) or COLUMNS-section empty",
  (STRPTR)"Missing RHS-section (4th section)",
  (STRPTR)"Missing ENDATA-section (last section)",
  (STRPTR)"Constraint must be of type N, E, L or G",
  (STRPTR)"Bound must be of type UP or LO",
  (STRPTR)"Lower bound > upper bound",
  (STRPTR)"Expression in RANGES belongs to a \"E\"-constraint",
  (STRPTR)"Can\'t find necessary expression in this line",
  (STRPTR)"File-name too long",
  (STRPTR)"Can\'t open file for read access",
  (STRPTR)"Can\'t open file for write access",
  (STRPTR)"End-of-file reached",
  (STRPTR)"Not enough memory",
  (STRPTR)"FATAL ERROR (This error should not occur, reboot system!)"
};





void main(void)
{
  INT     error;
  SHORT   start, stop, length, i;
  BOOL    end = _FALSE;
  USHORT  verbose = 0, result;
  TEXT    ch;
  STRPTR  sptr;
  ULONG   iter1, iter2, sec1, sec2, micro1, micro2;
  LONG    h, min, sec, sec100, pri = 0L;
  /*SHORT   atoi();*/
  VOID    PrintSolution(), Leave();

  /* Zuerst Intuition-Bibliothek öffnen */

  if(!(IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0))) {
    Leave("Can't find \"intuition.library\"!");
    return;
  }

  puts("\f\033[1mASimplex Version 1.2");
  puts("THE Amiga Simplex Program");
  puts("(c) 18.03.1989 Stefan Förster\033[0m\n\n");

  FOREVER {

    printf(">> ");

    if(NULL != (error = GetInput(args))) {
      PrintError(error,NULL);
      Leave("");
    }
    Cap(args,0,BUFFER);

    if(SearchExpr(args,&start,&stop)) {
      length = GetExpr(line,args,start,stop);

      if(strcmp(line,"HELP") == 0) {
        puts("   HELP");
        puts("   LOAD <file> [-cGOAL] [-bRHS] [-rRANGE] [-uBOUND] [-m] \
[-fFILE]");
        puts("   VERBOSE");
        puts("   PRIORITY [N]");
        puts("   QUIT");
      }

      else if(strcmp(line,"QUIT") == 0) {
        do {
          printf("?? Really [Y,N] ? ");
          if(NULL != (error = GetInput(args))) {
            PrintError(error,NULL);
            Leave("");
          }
          Cap(args,0,BUFFER);
          if(!SearchExpr(args,&start,&stop)) start = 0;
        } while((ch = *(args+start))!='N' && ch!='Y');
        if(ch == 'Y') Leave("");
      }

      else if(strcmp(line,"VERBOSE") == 0) {
        verbose = verbose ? 0 : VERBOSE;
        if(verbose) puts("   VERBOSE ON");
        else        puts("   VERBOSE OFF");
      }

      else if(strcmp(line,"PRIORITY") == 0) {
        sptr = args+stop+1;
        if(SearchExpr(sptr,&start,&stop)) {
          length = GetExpr(line,sptr,start,stop);
          pri = (LONG)atoi(line);
          if(pri<0L) pri = 0L;
          if(pri>20L) pri = 20L;
          SetTaskPri(FindTask((STRPTR)0),pri);
          printf("   Changed task priority to %ld.\n",pri);
        }
        else printf("   Current task priority is %ld.\n",pri);
      }

      else if(strcmp(line,"LOAD") == 0) {
        CurrentTime(&sec1,&micro1);
        if(MPSX(args+stop+1)) {
          if(!verbose) puts("@@ Please wait - calculation.");
          iter1 = iter2 = 0L;
          result = PhaseI(&m,&n,B,Nq,A,AB1,b,b2q,c,c2,&c0,c0start,verbose,
                          upper,x,cq,pi,dq,Nminus,help,&iter1);
          if(result == NOT_INVERTABLE) {
            puts("   Matrix AB is not invertable.");
            if(file[1]) fputs("   Matrix AB is not invertable.\n",file[1]);
          }

          else if(result == UNBOUNDED) {
            puts("   PHASE I IS UNBOUNDED. THIS SHOULD NOT OCCUR!!");
            if(file[1])
              fputs("   PHASE I IS UNBOUNDED. THIS SHOULD NOT OCCUR!!\n",file[1]);
          }

          else if(result == EMPTY) {
            puts("   This problem is not solveable.");
            if(file[1]) fputs("   This problem is not solveable.\n",file[1]);
          }

          else if(result == CLEAR_CUT) PrintSolution();

          else {
            result = PhaseII(m,n,B,Nq,A,AB1,b,b2q,c,&c0,c0start,
                    verbose|PHASE_II,upper,lower,x,cq,pi,dq,Nminus,help,&iter2);
            if(result == NOT_INVERTABLE) {
              puts("   Matrix AB is not invertable.");
              if(file[1]) fputs("   Matrix AB is not invertable.\n",file[1]);
            }
            else if(result == UNBOUNDED) {
              puts("   This problem is unbounded.");
              if(file[1]) fputs("   This problem is unbounded.\n",file[1]);
            }
            else PrintSolution();
          }

          printf("-> %ld iterations needed.\n",iter1+iter2);
          if(file[1]) fprintf(file[1],"-> %ld iterations needed.\n",iter1+iter2);
          GiveMemBack();
          GetRidOfLists();
          minimize = _FALSE;
        }
        CurrentTime(&sec2,&micro2);
        sec100 = micro2-micro1;
        sec = sec2-sec1;
        if(sec100 < 0L) {
          sec100 += 1000000L;
          --sec;
        }
        sec100 /= 10000L;
        h = sec/3600L;
        min = (sec-3600L*h)/60;
        sec -= 3600L*h+60*min;
        printf("-> Used time: %3ld : %02ld : %02ld,%02ld\n",h,min,sec,sec100);
        if(file[1]) {
          fprintf(file[1],"-> Used time: %3ld : %02ld : %02ld,%02ld\f",h,min,
                  sec,sec100);
          fclose(file[1]);
          file[1] = NULL;
        }
        if(file[0]) {
          fclose(file[0]);
          file[0] = NULL;
        }
      }

      else printf("   Unknown command \"%s\".\n",line);

    }

  } /* FOREVER */

}



/*****************************************************************************
 * VOID PrintSolution()                                                      *
 * Ausgabe der Lösung                                                        *
 *****************************************************************************/

VOID    PrintSolution()

{
  VOID PrintX();

  puts("@@ Solution:\n");
  printf("   %-9s = %14.10lg\n",symbols[GOAL],minimize ? -c0 : c0);
  PrintX(list[VAR_LIST],stdout);
  puts("");

  if(file[1]) {
    fputs("@@ Solution:\n\n",file[1]);
    fprintf(file[1],"   %-9s = %14.10lg\n",symbols[GOAL],minimize ? -c0 : c0);
    PrintX(list[VAR_LIST],file[1]);
    fputs("\n",file[1]);
  }
}



/*****************************************************************************
 * VOID PrintX()                                                             *
 * Gibt das Ergebnis der Berechnungen aus.                                   *
 *****************************************************************************/

VOID    PrintX(ptr,fptr)

ITEMPTR ptr;
FILE    *fptr;

{
  if(ptr) {
    PrintX(ptr->next,fptr);
    fprintf(fptr,"   %-9s = %14.10lg\n",ptr->string,x[ptr->nr-1]);
  }
}



/*****************************************************************************
 * VOID Leave()                                                              *
 * Verlassen des Programms                                                   *
 *****************************************************************************/

VOID    Leave(str)

STRPTR  str;

{
  if(IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);

  fprintf(stderr,"\n%s\n\n",str);
  exit(0);
}
