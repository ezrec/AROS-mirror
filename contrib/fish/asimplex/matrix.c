/*****************************************************************************
 * Modul                : matrix.c                                           *
 * Zweck                : Funktionen für Matrixinversion, Matrizenmultipli-  *
 *                        kation und Matrizenzugriff                         *
 * Autor                : Stefan Förster                                     *
 *                                                                           *
 * Datum      | Version | Bemerkung                                          *
 * -----------|---------|--------------------------------------------------- *
 * 21.12.1988 |         | M()                                                *
 *            |         | SetM()                                             *
 *            |         | Mult()                                             *
 * 05.01.1989 |         | Invers()                                           *
 * 18.01.1989 |         | Mult() ohne M() und SetM()                         *
 * 30.01.1989 |         | Invers() ohne Verwendung von LRSubst(),LRPart())   *
 *            |         | Bug in M(): Typumwandlung (LONG) wegen Overflow    *
 *            |         | Bug in SetM(): Typumwandlung (LONG) wegen Overflow *
 * 06.02.1989 | 0.0     | Invers() ohne M() u. SetM(); Anpassung an ASimplex *
 *            |         | Mult() angepaßt an ASimplex                        *
 *            |         | M() angepaßt an ASimplex                           *
 *            |         | SetM() angepaßt an ASimplex                        *
 * 02.03.1989 | 0.1     | Bug in M(): if(flag==PHASE_I) statt if(PHASE_I)    *
 * 07.03.1989 | 1.0     | Bug in Invers(): fabs() statt ABS() (Typ SHORT !!) *
 *****************************************************************************/




/*****************************************************************************
 * USHORT Invers(inv,n,p)                                                    *
 * Zweck     : Berechnung der inversen Matrix mit Hilfe des Gauß-Algorithmus *
 * Input     : inv    - Matrix, von der die inverse Matrix berechnet werden  *
 *                      soll                                                 *
 *             n      - Größe der (n,n)-Matrix                               *
 * Output    : inv    - inverse Matrix                                       *
 *             p      - n-Permutationsvektor                                 *
 * Ergebnis  : INVERTABLE/NOT_INVERTABLE                                     *
 *****************************************************************************/

#include <aros/oldprograms.h>
#include "simplex.h"

USHORT Invers(inv,n,p)
DOUBLE *inv;
SHORT  n,*p;
{
  REGISTER DOUBLE *ptr1, *ptr2, *ptr3, s;
  REGISTER SHORT  i,j,k;
  REGISTER DOUBLE max;

  for(k=1; k<=n; ++k) {
    max = 0.0;
    ptr2 = inv + ((LONG)(k-1)*(LONG)(n+1));
    for(i=k; i<=n; ++i) {
      s = 0;
      ptr1 = inv + ((LONG)(i-1)*(LONG)n+(LONG)(k-1));
      for(j=k; j<=n; ++j) {
        s += fabs(*ptr1);
        ++ptr1;
      }
      if(s==0.0) return(NOT_INVERTABLE);
      s = fabs(*ptr2)/s;
      ptr2 += n;
      if(s>max) {
        max = s;
        p[k-1] = i;
      }
    }
    if(max<EPS_INV) return(NOT_INVERTABLE);
    if((i=p[k-1]) != k) {
      ptr1 = inv+(i-1)*n;
      ptr2 = inv+(k-1)*n;
      for(j=0; j<n; ++j) {
        s = *ptr1;
        *ptr1++ = *ptr2;
        *ptr2++ = s;
      }
    }
    s = *(inv + ((LONG)(k-1)*(LONG)(n+1)));
    ptr1 = inv + ((LONG)(k-1)*(LONG)n);
    for(j=1; j<=n; ++j) {
      if(j!=k) {
        *ptr1 = -(*ptr1)/s;
        ptr2 = inv + (j-1);
        ptr3 = inv + (k-1);
        for(i=1; i<=n; ++i) {
          if(i!=k) *ptr2 += (*ptr3)*(*ptr1);
          ptr2 += n;
          ptr3 += n;
        }
      }
      ++ptr1;
    }
    ptr1 = inv + (k-1);
    for(i=1; i<=n; ++i) {
      if(i!=k) *ptr1 /= s;
      else     *ptr1 = 1.0/s;
      ptr1 += n;
    }
  }


  for(k=n-1; k>0; --k) {
    if((j=p[k-1]) != k) {
      ptr1 = inv + (k-1);
      ptr2 = inv + (j-1);
      for(i=1; i<=n; ++i) {
        s = *ptr1;
        *ptr1 = *ptr2;
        *ptr2 = s;
        ptr1 += n;
        ptr2 += n;
      }
    }
  }

return(INVERTABLE);
}



/*****************************************************************************
 * VOID Mult(A,B,erg,n,m,k)                                                  *
 * Zweck     : Matrizenmultiplikation erg = A*B                              *
 * Input     : A      - (n,m)-Matrix                                         *
 *             B      - (m,k)-Matrix                                         *
 *             n,m,k  - Zeilen- und Spaltenanzahlen                          *
 * Output    : erg    - (n,k)-Matrix A*B                                     *
 *****************************************************************************/

VOID    Mult(A,B,erg,n,m,k)
DOUBLE  *A,*B,*erg;
SHORT   n,m,k;
{
  REGISTER DOUBLE *Aptr,*Bptr,s;
  REGISTER SHORT  h,j,i;

  for(i=0; i<n; ++i) {          /* Zeilen von erg  */
    for(j=0; j<k; ++j) {        /* Spalten von erg */
      Aptr = A;
      Bptr = B;
      for(s=0.0,h=0; h<m; ++h) {
        s += (*Aptr++)*(*Bptr);
        Bptr += k;
      }
      *erg++ = s;
      ++B;
    }
    A += m;
    B -= k;
  }
}



/*****************************************************************************
 * DOUBLE M(matrix,n,i,j,flag)                                               *
 * Zweck     : M() liest die Komponente a[i,j] einer Matrix                  *
 * Input     : matrix - Zeiger auf die Matrix                                *
 *             n      - Anzahl Spalten                                       *
 *             i      - Zeilenindex                                          *
 *             j      - Spaltenindex                                         *
 *             flag   - PHASE_I/PHASE_II                                     *
 * Bemerkung : Falls j>n und flag==PHASE_I, gibt M() Werte der angehängten   *
 *             Einheitsmatrix aus (0 oder 1)                                 *
 *****************************************************************************/

DOUBLE M(matrix,n,i,j,flag)
DOUBLE *matrix;
SHORT  n,i,j;
USHORT flag;
{
  if(flag==PHASE_I && j>n) return(i==j-n ? 1.0 : 0.0);
  else                     return(*(matrix+((LONG)(i-1)*(LONG)n+(LONG)(j-1))));
}



/*****************************************************************************
 * VOID SetM(matrix,n,i,j,value)                                             *
 * Zweck     : SetM() setzt die Komponente a[i,j] einer Matrix auf 'value'   *
 * Input     : matrix - Zeiger auf die Matrix                                *
 *             n      - Anzahl Spalten                                       *
 *             i      - Zeilenindex                                          *
 *             j      - Spaltenindex                                         *
 *             value  - a[i,j] = value                                       *
 *****************************************************************************/

VOID    SetM(matrix,n,i,j,value)
DOUBLE  *matrix;
SHORT   n,i,j;
DOUBLE  value;
{
  *(matrix+((LONG)(i-1)*(LONG)n+(LONG)(j-1))) = value;
}
