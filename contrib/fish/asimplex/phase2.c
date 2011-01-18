/*****************************************************************************
 * Modul                : phase2.c                                           *
 * Zweck                : Phase II des Simplexalgorithmus                    *
 * Autor                : Stefan Förster                                     *
 *                                                                           *
 * Datum      | Version | Bemerkung                                          *
 * -----------|---------|--------------------------------------------------- *
 * 06.02.1989 | 0.0     |                                                    *
 * 07.02.1989 | 0.1     | Anpassung an PHASE_I                               *
 * 25.02.1989 | 0.2     | CHUZR(), WRETA(), 1. Testlauf erfolgreich          *
 * 26.02.1989 | 0.3     | PRICE() verbessert                                 *
 * 26.02.1989 | 0.4     | VERBOSE-Option, Reinvertierung von AB1             *
 * 02.03.1989 | 0.5     | Bug in CHUZR(): dqi<-EPS_NULL statt dqi<EPS_NULL   *
 *            |         | Bug in PRICE(): sum-=pi[qs-nm] statt sum-=pi[qs]   *
 *            |         | Bug in PRICE(): Bei PHASE_I hat A phys. nm Spalten *
 *            |         | Bug in FTRAN(): Bei PHASE_I hat A phys. nm Spalten *
 *            |         | Bug in WRETA(): M(A,phase==PHASE_I?nm:n,i,j,phase) *
 * 03.03.1989 |         | Bug in CHUZR(): Abfrage if(lambdaX==INFINITE||...) *
 *            | 1.0     | Bug in FTRAN(): ptr1 += m  statt  *ptr1 += m  !!!! *
 *            |         | Abfrage auf Invertierbarkeit von AB                *
 * 04.03.1989 | 1.1     | Bug in WRETA(): c0-Änderung, wenn Nq[s-1]<0 ist    *
 * 06.03.1989 | 1.2     | Bug in WRETA(): else ptr1+=m; bei AB1-Update       *
 *            |         | Neustrukturierung von WRETA()                      *
 * 10.03.1989 | 1.3     | c0start, lower zusätzlich übergeben                *
 *****************************************************************************/

#include <aros/oldprograms.h>
#include "simplex.h"


IMPORT VOID   Mult();
IMPORT USHORT Invers();
IMPORT DOUBLE M();
IMPORT VOID   SetM();

GLOBAL DOUBLE INFINITE;
GLOBAL BOOL   minimize;
GLOBAL FILE   *file[2];


/*****************************************************************************
 * USHORT PhaseII()                                                          *
 * - OPTIMAL, falls optimale Lösung gefunden                                 *
 * - UNBOUNDED, falls das Problem unbeschränkt ist                           *
 * - NOT_INVERTABLE, falls (unzulässigerweise!) AB nicht invertierbar ist    *
 *                                                                           *
 * Input:     B, Nq,                                                         *
 *            A, AB1,                                                        *
 *            b, b2q,                                                        *
 *            c, c0, c0start,                                                *
 *            flag,                                                          *
 *            upper, lower bzw. NULL (für Phase1())                          *
 * Output:    x, c0, B, Nq                                                   *
 *****************************************************************************/

USHORT PhaseII(m,n,B,Nq,A,AB1,b,b2q,c,c0,c0start,flags,upper,lower,x,cq,pi,dq,
               Nminus,help,iter)

SHORT   m,n;          /* m = Zeilenzahl, n = Spaltenzahl des Problems        */
SHORT   *B;           /* (m)-Vektor     - Basis                              */
SHORT   *Nq;          /* (n-m)-Vektor   - E-Nichtbasis                       */
DOUBLE  *A;           /* (m,n)-Matrix   - Matrix A                           */
DOUBLE  *AB1;         /* (m,m)-Matrix   - Inverse von AB                     */
DOUBLE  *b;           /* (m)-Vektor     - RHS (rechte Seite)                 */
DOUBLE  *b2q;         /* (m)-Vektor     - bq-AB1*AN*uN_                      */
DOUBLE  *c;           /* (n)-Vektor     - Vektor c                           */
DOUBLE  *c0;          /* Zahl (Zeiger!) - Zielfunktionswert                  */
DOUBLE  c0start;      /* Korrekturwert für Zielfunktionswert                 */
USHORT  flags;        /* Flags          - PHASE_I/PHASE_II,VERBOSE           */
DOUBLE  *upper;       /* (n)-Vektor     - Obergrenzen für x                  */
DOUBLE  *lower;       /* (n)-Vektor     - Untergrenzen für x                 */
DOUBLE  *x;           /* (n)-Vektor     - Lösung (falls existiert)           */
DOUBLE  *cq;          /* (n-m)-Vektor   - reduzierte Kosten                  */
DOUBLE  *pi;          /* (m)-Vektor     - pi = cB*AB1                        */
DOUBLE  *dq;          /* (m)-Vektor     - dq = AB1*A.qs                      */
SHORT   *Nminus;      /* (n-m)-Vektor   - Negativer Anteil von Nq            */
DOUBLE  *help;        /* (m)-Vektor     - Hilfsvektor                        */
ULONG   *iter;        /* Zeiger auf "Anzahl Iterationen"                     */

{
  USHORT  method = STEEPEST_ASCENT, rowflag = LAMBDA_0;
  USHORT  phase = flags & (PHASE_I | PHASE_II), verbose = flags & VERBOSE;
  SHORT   r ,s ,nm = n-m, i, qs, lastpos = nm-1; /* lastpos: 0,...,(n-m)-1 */
  DOUBLE  c0_old;
  VOID    BTRAN(), FTRAN();
  USHORT  PRICE(), CHUZR(), WRETA();
  ULONG   count = 1L;


  for( ; ; count++) {

    c0_old = *c0;

    if(rowflag!=LAMBDA_2) BTRAN(m,AB1,B,c,pi,x);

    if(PRICE(m,n,A,Nq,c,cq,pi,&s,method|phase,&lastpos)==OPTIMAL) {
      for(i=0; i<nm; ++i) {
        qs = ABS(Nq[i])-1;
        if(lower) x[qs] = Nq[i]>0 ? lower[qs] : upper[qs]+lower[qs];
        else      x[qs] = Nq[i]>0 ? 0.0 : upper[qs];
      }
      for(i=0; i<m; ++i) {
        if(lower) x[B[i]-1] = b2q[i]+lower[qs];
        else      x[B[i]-1] = b2q[i];
      }
      *iter = count;
      return(OPTIMAL);
    }

    FTRAN(m,n,AB1,A,Nq[s-1],dq,phase);

    if(CHUZR(m,dq,b2q,upper,B,Nq[s-1],s,&r,&rowflag)==UNBOUNDED) {
      *iter = count;
      return(UNBOUNDED);
    }

    /* x als eta-Vektor mißbrauchen */
    if(WRETA(m,n,B,Nq,A,AB1,b,b2q,c,cq,c0,c0start,dq,upper,r,s,x,help,
       Nminus,rowflag|phase|verbose,count) == NOT_INVERTABLE)
      return(NOT_INVERTABLE);

    if(verbose) {
      printf("@@ ");
      if(phase == PHASE_I) printf("phase I : ");
      else                 printf("phase II: ");
      printf("iter# %5ld : s =%4d, r =",count,s);
      if(rowflag == LAMBDA_2) printf(" np , ");
      else                    printf("%4d, ",r);
      printf("c0 = %16.10lg\n",minimize ? -(*c0) : *c0);
      if(method==STEEPEST_ASCENT) puts("   steepest-ascent-rule");
      else                        puts("   cyclic smallest-index-rule");
      if(file[1]) {
        fprintf(file[1],"@@ ");
        if(phase == PHASE_I) fprintf(file[1],"phase I : ");
        else                 fprintf(file[1],"phase II: ");
        fprintf(file[1],"iter# %5ld : s =%4d, r =",count,s);
        if(rowflag == LAMBDA_2) fprintf(file[1]," np , ");
        else                    fprintf(file[1],"%4d, ",r);
        fprintf(file[1],"c0 = %16.10lg\n",minimize ? -(*c0) : *c0);
        if(method==STEEPEST_ASCENT) fputs("   steepest-ascent-rule\n",file[1]);
        else fputs("   cyclic smallest-index-rule\n",file[1]);
      }
    }

    if(fabs(c0_old)>EPS_NULL)
      method = (*c0-c0_old)/fabs(c0_old) <= PERCENT ? STEEPEST_ASCENT :
                                                      SMALLEST_INDEX;
  }
}



/*****************************************************************************
 * VOID BTRAN()                                                              *
 * Backward Transformation                                                   *
 *****************************************************************************/

VOID BTRAN(m,AB1,B,c,pi,dummy)

SHORT   m;
DOUBLE  *AB1;
SHORT   *B;
DOUBLE  *c, *pi, *dummy; /* dummy : (m)-Hilfsvektor */

{
  DOUBLE *ptr=dummy;
  SHORT  i;

  for(i=0; i<m; ++i) *ptr++ = c[B[i]-1];
  Mult(dummy,AB1,pi,1,m,m);

}



/*****************************************************************************
 * USHORT PRICE() -> OPTIMAL/NOT_OPTIMAL                                     *
 * method & STEEPEST_ASCENT => Steilster-Anstieg-Regel                       *
 * method & SMALLEST_INDEX  => (zyklische) Kleinster-Index-Regel             *
 *****************************************************************************/

USHORT PRICE(m,n,A,Nq,c,cq,pi,s,flags,lastpos)

SHORT   m, n;
DOUBLE  *A;
SHORT   *Nq;
DOUBLE  *c, *cq, *pi;
SHORT   *s;
USHORT  flags;       /* PHASE_I/PHASE_II, SMALLEST_INDEX/STEEPEST_ASCENT */
SHORT   *lastpos;

{
  REGISTER DOUBLE *ptr1, *ptr2, sum;
  REGISTER SHORT  j, i;
  SHORT           qs, nm = n-m, pos;
  USHORT          flag = OPTIMAL;
  USHORT          phase = flags & (PHASE_I | PHASE_II);
  USHORT          method = flags & (SMALLEST_INDEX | STEEPEST_ASCENT);
  SHORT           columns = phase==PHASE_I ? nm : n;
  DOUBLE          dummy;

  if(method == STEEPEST_ASCENT) {
    for(i=0; i<nm; ++i) {
      ptr1 = pi;
      qs = ABS(Nq[i])-1;
      sum = c[qs];
      if(phase == PHASE_I && qs>=nm) sum -= pi[qs-nm];
      else {
        ptr2 = A+qs;
        for(j=0; j<m; ++j) {
          sum -= (*ptr1++)*(*ptr2);
          ptr2 += columns;
        }
      }
      cq[i] = sum;
    }
    for(i=0; i<nm; ++i) {
      if((dummy=fabs(cq[i])) > EPS_NULL && cq[i]*Nq[i] > 0.0) {
        if(flag==OPTIMAL) {
          flag = NOT_OPTIMAL;
          *s = i+1;           /* Index s : 1,...,n-m */
          sum = dummy;
        }
        else {
          if(dummy>sum) {
            *s = i+1;
            sum = dummy;
          }
        }
      }
    }
  }

  else {                              /* method == SMALLEST_INDEX */
    pos = ((*lastpos)+1) % nm;        /* lastpos+1 modulo nm */
    for(i=0; i<nm; ++i) {
      ptr1 = pi;
      qs = ABS(Nq[pos])-1;
      sum = c[qs];
      if(phase == PHASE_I && qs>=nm) sum -= pi[qs-nm];
      else {
        ptr2 = A+qs;
        for(j=0; j<m; ++j) {
          sum -= (*ptr1++)*(*ptr2);
          ptr2 += columns;
        }
      }
      if(fabs(sum)>EPS_NULL && sum*Nq[pos]>0.0) {
        cq[pos] = sum;
        *s = pos+1;                   /* s: Index 1,...,nm */
        *lastpos = pos;
        return(NOT_OPTIMAL);
      }
      ++pos;
      pos %= nm;
    }
  }

return(flag);
}



/*****************************************************************************
 * VOID FTRAN()                                                              *
 * Forward Transformation                                                    *
 *****************************************************************************/

VOID FTRAN(m,n,AB1,A,qs,dq,phase)

SHORT   m, n;
DOUBLE  *AB1, *A;
SHORT   qs;
DOUBLE  *dq;
USHORT  phase;

{
  REGISTER DOUBLE *ptr1, sum;
  REGISTER SHORT  j, i;
  SHORT           nm = n-m, columns = phase==PHASE_I ? nm : n;

  qs = ABS(qs)-1;
  if(phase == PHASE_I && qs>=nm) {
    ptr1 = AB1 + (qs-nm);
    for(i=0; i<m; ++i) {
      dq[i] = *ptr1;
      ptr1 += m;
    }
  }
  else {
    for(i=0; i<m; ++i) {
      ptr1 = A+qs;    
      sum = 0.0;
      for(j=0; j<m; ++j) {
        sum += (*AB1++)*(*ptr1);
        ptr1 += columns;
      }
      dq[i] = sum;
    }
  }

}



/*****************************************************************************
 * USHORT CHUZR() -> UNBOUNDED/NOT_UNBOUNDED                                 *
 * Ergebnis: Zeile r, rowflag (Art des Pivotschritts)                        *
 *****************************************************************************/

USHORT  CHUZR(m,dq,b2q,upper,B,qs,s,r,rowflag)

SHORT   m;
DOUBLE  *dq, *b2q, *upper;
SHORT   *B, qs, s, *r;      /* qs: 0,...,n-1   s: 1,...,n-m   r: 1,...,m  */
USHORT  *rowflag;           /* LAMBDA_0, LAMBDA_1 oder LAMBDA_2 */

{
  REGISTER DOUBLE lambda0 = INFINITE, lambda1 = INFINITE;
  REGISTER SHORT  i;
  DOUBLE          lambda2 = upper[ABS(qs)-1];
  REGISTER DOUBLE dqi, dummy, min = lambda2;
  SHORT           sigma = SGN(qs), min_i;

  *rowflag = LAMBDA_2;

  for(i=0; i<m; ++i) {
    dqi = sigma*dq[i];
    if(dqi > EPS_NULL) {
      dummy = b2q[i]/dqi;
      if(lambda0 == INFINITE || dummy<lambda0) lambda0 = dummy;
      if(min==INFINITE || dummy<min || (dummy==min &&
         fabs(dqi)>fabs(dq[min_i-1]))) {
        lambda0 = min = dummy;
        min_i = i+1;
        *rowflag = LAMBDA_0;
      }
    }
    else if(dqi < -EPS_NULL && (dummy = upper[B[i]-1])!=INFINITE) {
      dummy = (b2q[i]-dummy)/dqi;
      if(lambda1 == INFINITE || dummy<lambda1) lambda1 = dummy;
      if(min==INFINITE || dummy<min || (dummy==min &&
         fabs(dqi)>fabs(dq[min_i-1]))) {
        lambda1 = min = dummy;
        min_i = i+1;
        *rowflag = LAMBDA_1;
      }
    }
  }

  *r = min_i;

  if(lambda0==INFINITE && lambda1==INFINITE && lambda2==INFINITE)
    return(UNBOUNDED);

  return(NOT_UNBOUNDED);
}



/*****************************************************************************
 * USHORT WRETA() -> INVERTABLE/NOT_INVERTABLE                               *
 * je nachdem, ob AB invertierbar ist (sollte es zumindest) oder nicht       *
 *****************************************************************************/

USHORT  WRETA(m,n,B,Nq,A,AB1,b,b2q,c,cq,c0,c0start,dq,upper,r,s,eta,
              help,Nminus,flags,iter)

SHORT   m, n;
SHORT   *B, *Nq;
DOUBLE  *A, *AB1, *b, *b2q, *c, *cq, *c0, c0start, *dq, *upper;
SHORT   r, s;
DOUBLE  *eta, *help;         /* eta : (n)-Vektor (=x), als (m)-Vektor verw. */
SHORT   *Nminus;             /* Nminus (n-m)-Vektor, negativer Anteil von Nq */
USHORT  flags;               /* PHASE_I/PHASE_II, rowflag, verbose */
ULONG   iter;                /* Zahl der bewältigten Iterationen */

{
  REGISTER DOUBLE *ptr1, *ptr2, eta_r = 1.0/dq[r-1], eta_dummy;
  REGISTER SHORT  j, i;
  LONG            offset = (LONG)(r-1)*(LONG)m;
  SHORT           anzneg = 0; /* Anzahl negativer Elemente in Nq */
  SHORT           nm = n-m, swap;
  USHORT          phase = flags & (PHASE_I | PHASE_II);
  USHORT          rowflag = flags & (LAMBDA_0 | LAMBDA_1 | LAMBDA_2);
  SHORT           columns = phase==PHASE_I ? nm : n;


  iter %= INVERT_FREQUENCY;

  if(rowflag != LAMBDA_2) {

    /* Falls Nq[s-1]<0 ist, ändert sich c0 zusätzlich !!!! */
    /* Falls AB1 reinvertiert wird, wird c0 jedoch völlig neu berechnet */
    if(Nq[s-1]<0) *c0 -= cq[s-1]*upper[ABS(Nq[s-1])-1];

    swap = B[r-1];
    B[r-1] = ABS(Nq[s-1]);
    Nq[s-1] = rowflag == LAMBDA_0 ? swap : -swap;

    for(j=0; j<nm; ++j) {
      if(Nq[j]<0) Nminus[anzneg++] = ABS(Nq[j]);
    }

    if(!iter) {  /* AB1 reinvertieren */

      if(flags & VERBOSE) {
        printf("@@ ");
        if(phase == PHASE_I) printf("phase I : ");
        else                 printf("phase II: ");
        puts("reinversion");
        if(file[1]) {
          fprintf(file[1],"@@ ");
          if(phase == PHASE_I) fprintf(file[1],"phase I : ");
          else                 fprintf(file[1],"phase II: ");
          fputs("reinversion\n",file[1]);
        }
      }

      for(j=1; j<=m; ++j) {
        swap = B[j-1];        /* B[j].te Spalte von A */
        for(i=1; i<=m; ++i) SetM(AB1,m,i,j,M(A,columns,i,swap,phase));
      }
      /* x==eta als (SHORT *)-Permutationsvektor !! */
      if(Invers(AB1,m,(SHORT *)eta) == NOT_INVERTABLE) return(NOT_INVERTABLE);
    }

    else {  /* AB1 nicht reinvertieren, sondern nur mit eta mult. */

      ptr1 = eta;
      ptr2 = dq;

      for(j=1; j<=m; ++j) {
        *ptr1++ = j==r ? eta_r : -(*ptr2)*eta_r;
        ++ptr2;
      }

      ptr1 = AB1;

      for(i=0; i<m; ++i) {
        ptr2 = AB1+offset;
        if(i+1==r) ptr1 += m; /* r.te Zeile überspringen */
        else {
          if((eta_dummy = eta[i]) != 0.0) {
            for(j=0; j<m; ++j) *ptr1++ += eta_dummy*(*ptr2++);
          }
          else ptr1 += m;
        }
      }

      ptr2 = AB1+offset;

      for(j=0; j<m; ++j) *ptr2++ *= eta_r;  /* r.te Spalte mit eta_r mult. */
    }


    for(i=0; i<m; ++i) {
      eta_dummy = b[i];
      for(j=0; j<anzneg; ++j)
        eta_dummy -= M(A,columns,i+1,Nminus[j],phase)*upper[Nminus[j]-1];
      eta[i] = eta_dummy;
    }

    Mult(AB1,eta,b2q,m,m,1); /* eta-Vektor als Zwischenspeicher */

    if(iter) *c0 += cq[s-1]*b2q[r-1];
    else {
      eta_dummy = 0.0;
      ptr1 = b2q;
      for(i=0; i<m; ++i) eta_dummy += c[B[i]-1]*(*ptr1++);
      for(i=0; i<anzneg; ++i) {
        j = Nminus[i]-1;
        eta_dummy += c[j]*upper[j];
      }
      *c0 = eta_dummy+c0start;
    }

  }



  else {   /* rowflag == LAMBDA_2 */

    if(!iter) {  /* AB1 reinvertieren */

      if(flags & VERBOSE) {
        printf("@@ ");
        if(phase == PHASE_I) printf("phase I : ");
        else                 printf("phase II: ");
        puts("reinversion");
        if(file[1]) {
          fprintf(file[1],"@@ ");
          if(phase == PHASE_I) fprintf(file[1],"phase I : ");
          else                 fprintf(file[1],"phase II: ");
          fputs("reinversion\n",file[1]);
        }
      }

      for(j=1; j<=m; ++j) {
        swap = B[j-1];        /* B[j].te Spalte von A */
        for(i=1; i<=m; ++i) SetM(AB1,m,i,j,M(A,columns,i,swap,phase));
      }
      /* x==eta als (SHORT *)-Permutationsvektor !! */
      if(Invers(AB1,m,(SHORT *)eta) == NOT_INVERTABLE) return(NOT_INVERTABLE);
    }

    swap = Nq[s-1];               /* swap      : qs_quer */
    anzneg = ABS(swap);           /* anzneg    : qs      */
    eta_dummy = upper[anzneg-1];  /* eta_dummy : u(qs)   */

    ptr1 = eta;
    for(i=1; i<=m; ++i) *ptr1++ = M(A,columns,i,anzneg,phase)*eta_dummy;

    Mult(AB1,eta,help,m,m,1);

    ptr1 = b2q;
    ptr2 = help;

    if(swap>0) {    /* wenn also das alte qs_quer positiv war */
      for(i=0; i<m; ++i) *ptr1++ -= *ptr2++;
      *c0 += cq[s-1]*upper[anzneg-1];
    }
    else {
      for(i=0; i<m; ++i) *ptr1++ += *ptr2++;
      *c0 -= cq[s-1]*upper[anzneg-1];
    }

    Nq[s-1] *= -1;  /* qs_quer auf neuen Stand bringen */
  }

  return(INVERTABLE);
}
