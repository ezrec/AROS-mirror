/*****************************************************************************
 * Modul                : phase1.c                                           *
 * Zweck                : Phase I des Simplexalgorithmus                     *
 * Autor                : Stefan Förster                                     *
 *                                                                           *
 * Datum      | Version | Bemerkung                                          *
 * -----------|---------|--------------------------------------------------- *
 * 01.03.1989 | 0.0     |                                                    *
 * 02.03.1989 | 0.1     | TryPivot(), Calc1(), Calc2()                       *
 * 03.03.1989 | 0.2     | Bug in TryPivot(): sum = eta[k] statt sum = eta[i] *
 *            |         | Abfrage auf Invertierbarkeit von AB                *
 * 04.03.1989 | 1.0     | Bug in Calc2(): zusätzl.: b[i] = b[j]              *
 * 06.03.1989 | 1.1     | Bug in TryPivot(): else ptr1+=mm; bei AB1-Update   *
 * 07.03.1989 |         | Ein Aufruf von Mult() gespart                      *
 * 10.03.1989 | 1.2     | Änd. in Calc1(): *c0 = dummy + c0start;            *
 * 14.03.1989 | 1.3     | BUG in PhaseI(): c0 neuberechnen, falls CLEAR_CUT  *
 *****************************************************************************/

#include <aros/oldprograms.h>
#include "simplex.h"


#define PIVOT     (USHORT)1
#define NO_PIVOT  (USHORT)0

IMPORT VOID   Mult();
IMPORT USHORT Invers();
IMPORT DOUBLE M();
IMPORT VOID   SetM();
IMPORT USHORT PhaseII();

GLOBAL DOUBLE INFINITE;
GLOBAL BOOL   minimize;


/*****************************************************************************
 * USHORT PhaseI()                                                           *
 * - EMPTY, falls das Polyeder leer ist                                      *
 * - CLEAR_CUT, falls das Polyeder einelementig ist                          *
 * - OPTIMAL, falls eine zulässige Basis gefunden wurde                      *
 * - UNBOUNDED, falls (unzulässigerweise !) das Hilfsprogramm unbeschr. ist  *
 * - NOT_INVERTABLE, falls AB (unzulässigerweise!) nicht invertierbar ist    *
 *                                                                           *
 * Input:     m, n, A, b, c, c0start, upper  (b>=0)                          *
 *                                                                           *
 * Output:    EMPTY          : -                                             *
 *            CLEAR_CUT      : x                                             *
 *            OPTIMAL        : m, A, b, B, Nq, AB1, b2q                      *
 *            UNBOUNDED      : -                                             *
 *            NOT_INVERTABLE : -                                             *
 *****************************************************************************/



USHORT PhaseI(m,n,B,Nq,A,AB1,b,b2q,c,c2,c0,c0start,flags,upper,x,cq,pi,dq,
              Nminus,help,iter)

SHORT   *m;         /* Zeiger auf m           */
SHORT   *n;         /* Zeiger auf n           */
SHORT   *B;         /* (m)-Vektor             */
SHORT   *Nq;        /* (n)-Vektor: (n-m)+m    */
DOUBLE  *A;         /* (m,n)-Matrix           */
DOUBLE  *AB1;       /* (m,m)-Matrix           */
DOUBLE  *b;         /* (m)-Vektor             */
DOUBLE  *b2q;       /* (m)-Vektor             */
DOUBLE  *c;         /* (n)-Vektor             */
DOUBLE  *c2;        /* (n+m)-Vektor           */
DOUBLE  *c0;        /* Zeiger auf c0          */
DOUBLE  c0start;    /* Korr.wert f. Zielfktsw.*/
USHORT  flags;      /* VERBOSE                */
DOUBLE  *upper;     /* (n+m)-Vektor           */
DOUBLE  *x;         /* (n+m)-Vektor           */
DOUBLE  *cq;        /* (n)-Vektor: (n-m)+m    */
DOUBLE  *pi;        /* (m)-Vektor             */
DOUBLE  *dq;        /* (m)-Vektor             */
SHORT   *Nminus;    /* (n)-Vektor: (n-m)+m    */
DOUBLE  *help;      /* (m)-Vektor             */
ULONG   *iter;      /* Anzahl Iterationen     */

{                         /*         T  */
  DOUBLE  bsum;           /* bsum = 1 b */
  DOUBLE  sum, *ptr1;
  SHORT   mm = *m, nn = *n, i, j, pivots, no_of_bad;
  USHORT  result;
  VOID    PrepareData(), Calc1();
  USHORT  TryPivot(), Calc2();


  PrepareData(mm,nn,B,Nq,A,AB1,b,b2q,&bsum,c2,c0,upper);


  /* Das Hilfsprogramm hat eine optimale Lösung (theoretisches Ergebnis) */

  result = PhaseII(mm,mm+nn,B,Nq,A,AB1,b,b2q,c2,c0,0.0,PHASE_I | flags,upper,
                   NULL,x,cq,pi,dq,Nminus,help,iter);

  if(result == UNBOUNDED) {
    puts("\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    puts("!! Hilfsprogramm von Phase I unbeschränkt !!");
    puts("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
    return(UNBOUNDED);
  }

  /*                        T      T      */
  /* P(A,b) == EMPTY, wenn 1 Ax < 1 b ist */

  if(bsum-(*c0) > EPS_NULL) return(EMPTY);

  /* Versuch, künstliche Variablen aus der Basis zu werfen */

  do {
    pivots = no_of_bad = 0;
    for(i=1; i<=mm; ++i) {
      if(B[i-1]>nn) {         /* help als eta-Vektor */
        if(TryPivot(i,mm,nn,B,Nq,A,AB1,help,dq) == PIVOT) ++pivots;
        else                                              ++no_of_bad;
      }
    }
  } while(pivots>0);


  if(no_of_bad == 0) {  /* keine künstlichen Variablen in der Basis (Yeah!) */
    if(nn == mm) {      /* trivialer Fall */
      Mult(AB1,b,help,mm,mm,1);
      for(i=0; i<mm; ++i) x[B[i]-1] = help[i]; /* umsortieren */
      sum = c0start;                           /* c0 berechnen */
      ptr1 = help;
      for(i=0; i<mm; ++i) sum += c[B[i]-1]*(*ptr1++);
      for(i=0; i<nn; ++i) {
        if(Nq[i]<0) {
          j = ABS(Nq[i])-1;
          sum += c[j]*upper[j];
        }
      }
      *c0 = sum;
      return(CLEAR_CUT);
    }
    else {              /* nn > mm */
      Calc1(mm,nn,B,Nq,b2q,c,c0,c0start,upper,Nminus);
      return(OPTIMAL);
    }
  }

  else {                /* künstl. Variablen in der Basis */
    if(Calc2(m,nn,B,A,AB1,b,b2q,help) == NOT_INVERTABLE) return(NOT_INVERTABLE);
    mm = *m;
    if(nn == mm) {      /* trivialer Fall */
      Mult(AB1,b,help,mm,mm,1);
      for(i=0; i<mm; ++i) x[B[i]-1] = help[i]; /* umsortieren */
      sum = c0start;                           /* c0 berechnen */
      ptr1 = help;
      for(i=0; i<mm; ++i) sum += c[B[i]-1]*(*ptr1++);
      for(i=0; i<nn; ++i) {
        if(Nq[i]<0) {
          j = ABS(Nq[i])-1;
          sum += c[j]*upper[j];
        }
      }
      *c0 = sum;
      return(CLEAR_CUT);
    }
    else {
      Calc1(mm,nn,B,Nq,b2q,c,c0,c0start,upper,Nminus);
      return(OPTIMAL);
    }
  }
}



/*****************************************************************************
 * VOID PrepareData()                                                        *
 * bereitet die Daten für den Aufruf von PhaseII() mit dem Hilfsprogramm vor *
 *****************************************************************************/

VOID    PrepareData(mm,nn,B,Nq,A,AB1,b,b2q,bsum,c2,c0,upper)

SHORT   mm, nn, *B, *Nq;
DOUBLE  *A, *AB1, *b, *b2q, *bsum, *c2, *c0, *upper;

{
  REGISTER DOUBLE sum, *ptr1, *ptr2;
  REGISTER SHORT  j, i, *ptr;


  *c0 = 0.0;

  /*                     T          */
  /* Neue Zielfunktion: 1 A  ->  c2 */

  ptr1 = c2;
  for(i=0; i<nn; ++i) {
    ptr2 = A+i;
    for(sum=0.0,j=0; j<mm; ++j) {
      sum += *ptr2;
      ptr2 += nn;
    }
    *ptr1++ = sum;
  }
  for(i=0; i<mm; ++i) *ptr1++ = 0.0;

  /*         T  */
  /* bsum = 1 b */

  ptr1 = b;
  for(sum=0.0,i=0; i<mm; ++i) sum += *ptr1++;
  *bsum = sum;

  /* Für das Hilfsprogramm gilt b[] == b2q[] */

  ptr1 = b;
  ptr2 = b2q;
  for(i=0; i<mm; ++i) *ptr2++ = *ptr1++;

  /* AB1 ist die Einheitsmatrix (nur Diagonale setzen, wegen MEMF_CLEAR) */

  ptr1 = AB1;
  for(i=0; i<mm; ++i) {
    *ptr1++ = 1.0;
    ptr1 += mm;
  }

  /* Basis B = { n+1, ... , n+m } */

  ptr = B;
  for(i=0,j=nn; i<mm; ++i) *ptr++ = ++j;

  /* Nichtbasis Nq = { 1, ... , n } */

  ptr = Nq;
  for(i=1; i<=nn; ++i) *ptr++ = i;

}



/*****************************************************************************
 * USHORT TryPivot() -> PIVOT/NO_PIVOT                                       *
 * Versuch, die künstliche Variable B[pos-1] aus der Basis zu entfernen      *
 *****************************************************************************/

USHORT  TryPivot(pos,mm,nn,B,Nq,A,AB1,eta,dq)

SHORT   pos, mm, nn, *B, *Nq;
DOUBLE  *A, *AB1, *eta, *dq;

{
  REGISTER SHORT  j, k, i;
  REGISTER DOUBLE *ptr1, *ptr2, sum;
  SHORT           nandm = nn+mm, column;
  LONG            offset = (LONG)(--pos)*(LONG)mm; /* pos: 0,...,m-1 */

  for(i=0; i<nandm; ++i) {

    if((column = Nq[i]-1) >= 0 && column < nn) {  /* mögl. Kandidat gefunden */

      ptr1 = AB1+offset;      /* dq[pos] Pivotelement berechnen */
      ptr2 = A+column;        /* column : 0,...,n-1 */
      for(sum=0.0,j=0; j<mm; ++j,ptr2+=nn) sum += (*ptr1++)*(*ptr2);

      if(fabs(sum) > EPS_NULL) {   /* Pivotelement != 0.0 => Pivotschritt */

        dq[pos] = sum;            /* dq[] jetzt ganz berechnen */
        ptr1 = AB1;
        for(k=0; k<mm; ++k) {
          if(k != pos) {
            ptr2 = A+column;
            for(sum=0.0,j=0; j<mm; ++j,ptr2+=nn) sum += (*ptr1++)*(*ptr2);
            dq[k] = sum;
          }
          else ptr1 += mm;
        }

        ptr1 = eta;                 /* eta-Vektor berechnen */
        ptr2 = dq;
        sum = 1.0/dq[pos];
        for(j=0; j<mm; ++j,++ptr2) *ptr1++ = j == pos ? sum : -(*ptr2)*sum;

        ptr1 = AB1;                 /* AB1 update */
        for(k=0; k<mm; ++k) {
          ptr2 = AB1+offset;
          if(k==pos) ptr1 += mm; /* pos.te Zeile überspringen */
          else {
            if((sum = eta[k]) != 0.0) {
              for(j=0; j<mm; ++j) *ptr1++ += sum*(*ptr2++);
            }
            else ptr1 += mm;
          }
        }
        ptr2 = AB1+offset;
        sum = 1.0/dq[pos];
        for(j=0; j<mm; ++j) *ptr2++ *= sum;  /* pos.te Spalte * 1/dq[pos] */


        k = B[pos];
        B[pos] = Nq[i];   /* nach obiger Abfrage > 0 */
        Nq[i] = k;

        return(PIVOT);

      }    /* if(Pivotelement > 0.0) */
    }      /* if(möglicher Kandidat) */
  }        /* for(i= ...             */

  return(NO_PIVOT);
}



/*****************************************************************************
 * VOID Calc1()                                                              *
 * berechnet c0 für die gefundene Basislösung und entfernt die künstlichen   *
 * Variablen aus Nq                                                          *
 *****************************************************************************/

VOID    Calc1(mm,nn,B,Nq,b2q,c,c0,c0start,upper,Nminus)

SHORT   mm, nn, *B, *Nq;
DOUBLE  *b2q, *c, *c0, c0start, *upper;
SHORT   *Nminus;

{
  REGISTER DOUBLE dummy, *ptr1;
  REGISTER SHORT  j, i, *ptr2;
  SHORT           nm = nn-mm, anzneg = 0;

  /* künstliche Variable aus Nq entfernen und gleichzeitig Nminus erstellen */

  for(ptr2=Nq,i=0; i<nn && *ptr2<=nn; ++i,++ptr2) {
    if(*ptr2 < 0) Nminus[anzneg++] = ABS(*ptr2);
  }

  for(j=i+1; j<nn; ++j) {
    if(Nq[j]<=nn) {
      Nq[i++] = Nq[j];
      if(Nq[j] < 0) Nminus[anzneg++] = ABS(Nq[j]);
    }
  }

  /* fehlt noch c0 für die gefundene zulässige Basis */
  /*        T       T              */
  /* c0 = cB xB + cN_uN_ + c0start */

  ptr1 = b2q;
  for(dummy=0.0,i=0; i<mm; ++i) dummy += c[B[i]-1]*(*ptr1++);
  ptr2 = Nminus;
  for(i=0; i<anzneg; ++i) {
    j = (*ptr2++)-1;
    dummy += c[j]*upper[j];
  }
  *c0 = dummy + c0start;

}



/*****************************************************************************
 * USHORT Calc2() ->INVERTABLE/NOT_INVERTABLE                                *
 * je nachdem, ob AB invertierbar ist (sollte es) oder nicht                 *
 * Streichen der überflüssigen Zeilen von A, Entfernen der künstlichen       *
 * Variablen aus B, Update von *m und Neuberechnung von AB1                  *
 *****************************************************************************/

USHORT  Calc2(m,nn,B,A,AB1,b,b2q,help)

SHORT   *m, nn, *B;
DOUBLE  *A, *AB1, *b, *b2q, *help;

{
  REGISTER SHORT  j, i, mm = *m, *ptr;
  DOUBLE          *dest, *src;
  SHORT           column;
  LONG            length = (LONG)nn*(LONG)sizeof(DOUBLE);

  for(ptr=B,i=0; i<mm && (*ptr++)<=nn; ++i);

  if(i<mm) {

    dest = A+(LONG)i*(LONG)nn;
    src = dest + nn;

    for(j=i+1; j<mm; ++j,src+=nn) {
      if(B[j]<=nn) {
        CopyMemQuick(src,dest,length);
        dest += nn;
        b2q[i] = b2q[j];    /* Auch Elemente aus b2q */
        b[i] = b[j];        /* und b streichen       */
        B[i++] = B[j];
      }
    }

    *m = mm = i;

    /* AB1 neu berechnen */

    for(ptr=B,j=0; j<mm; ++j) {
      src = A+((*ptr++)-1);        /* B[j].te Spalte von A */
      dest = AB1+j;
      for(i=0; i<mm; ++i,dest+=mm,src+=nn) *dest = *src;
    }
    /* help als (SHORT *)-Permutationsvektor !! */
    if(Invers(AB1,mm,(SHORT *)help) == NOT_INVERTABLE) return(NOT_INVERTABLE);
  }

  return(INVERTABLE);

}
