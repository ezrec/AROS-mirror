/* Copyright (c) 1994, Joseph Arceneaux */

/* Copy LEN bytes starting at SRCADDR to DESTADDR.  Result undefined
   if the source overlaps with the destination.
   Return DESTADDR. */

#ifdef USG
char *
memcpy (destaddr, srcaddr, len)
     char *destaddr;
     char *srcaddr;
     int len;
{
  char *dest = destaddr;

  while (len-- > 0)
    *destaddr++ = *srcaddr++;
  return dest;
}
#endif
