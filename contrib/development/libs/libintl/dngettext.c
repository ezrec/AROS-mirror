/* Implementation of the dngettext(3) function.
   Copyright (C) 1995-1997, 2000, 2001, 2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <locale.h>

#include "gettextP.h"
#include <libintl.h>

/* @@ end of prolog @@ */

#define DNGETTEXT dngettext
#define DCNGETTEXT dcngettext

/* Look up MSGID in the DOMAINNAME message catalog of the current
   LC_MESSAGES locale and skip message according to the plural form.  */
char *
DNGETTEXT (domainname, msgid1, msgid2, n)
     const char *domainname;
     const char *msgid1;
     const char *msgid2;
     unsigned long int n;
{
  return DCNGETTEXT (domainname, msgid1, msgid2, n, LC_MESSAGES);
}

