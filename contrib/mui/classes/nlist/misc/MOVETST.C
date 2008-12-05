
/* attention :  le nombre d'octets doit etre multiple de 4 !
 * et que ca soit efficace src et dest doivent etre multiples de 4 aussi.
 *
 * extern void  NL_Move(void *dest,void *src,LONG len);
 *
 */

extern void  NL_Move(long *dest,long *src,long len);


extern void  NL_MoveD(long *dest,long *src,long len);

void main()
{
  long tab1[32];
  long tab2[32];
  NL_Move(tab2,tab1,32);
}

