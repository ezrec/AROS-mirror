/* Handle list of needed message catalogs
   Copyright (C) 1995-1999, 2000, 2001, 2002, 2004, 2006, 2007
   Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Written by Ulrich Drepper <drepper@gnu.org>, 1995.

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

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#if defined HAVE_UNISTD_H || defined _LIBC
# include <unistd.h>
#endif

#include "gettextP.h"
# include <libintl.h>

/* @@ end of prolog @@ */
/* List of already loaded domains.  */
static struct loaded_l10nfile *_nl_loaded_domains;


/* Return a data structure describing the message catalog described by
   the DOMAINNAME and CATEGORY parameters with respect to the currently
   established bindings.  */
struct loaded_l10nfile *
internal_function
_nl_find_domain (dirname, locale, domainname, domainbinding)
     const char *dirname;
     char *locale;
     const char *domainname;
     struct binding *domainbinding;
{
  struct loaded_l10nfile *retval;
  const char *language;
  const char *modifier;
  const char *territory;
  const char *codeset;
  const char *normalized_codeset;
  const char *alias_value;
  int mask;

  /* LOCALE can consist of up to four recognized parts for the XPG syntax:

		language[_territory[.codeset]][@modifier]

     Beside the first part all of them are allowed to be missing.  If
     the full specified locale is not found, the less specific one are
     looked for.  The various parts will be stripped off according to
     the following order:
		(1) codeset
		(2) normalized codeset
		(3) territory
		(4) modifier
   */

  /* We need to protect modifying the _NL_LOADED_DOMAINS data.  */
  __libc_rwlock_define_initialized (static, lock);
  __libc_rwlock_rdlock (lock);

  /* If we have already tested for this locale entry there has to
     be one data set in the list of loaded domains.  */
  retval = _nl_make_l10nflist (&_nl_loaded_domains, dirname,
			       strlen (dirname) + 1, 0, locale, NULL, NULL,
			       NULL, NULL, domainname, 0);
  __libc_rwlock_unlock (lock);

  if (retval != NULL)
    {
      /* We know something about this locale.  */
      int cnt;

      if (retval->decided <= 0)
	_nl_load_domain (retval, domainbinding);

      if (retval->data != NULL)
	return retval;

      for (cnt = 0; retval->successor[cnt] != NULL; ++cnt)
	{
	  if (retval->successor[cnt]->decided <= 0)
	    _nl_load_domain (retval->successor[cnt], domainbinding);

	  if (retval->successor[cnt]->data != NULL)
	    break;
	}

      return retval;
      /* NOTREACHED */
    }

  /* See whether the locale value is an alias.  If yes its value
     *overwrites* the alias name.  No test for the original value is
     done.  */
  alias_value = NULL;
  if (alias_value != NULL)
    locale = strdupa ((char *)alias_value);

  /* Now we determine the single parts of the locale name.  First
     look for the language.  Termination symbols are `_' and `@' if
     we use XPG4 style, and `_', `+', and `,' if we use CEN syntax.  */
  mask = _nl_explode_name (locale, &language, &modifier, &territory,
			   &codeset, &normalized_codeset);
  if (mask == -1)
    /* This means we are out of core.  */
    return NULL;

  /* We need to protect modifying the _NL_LOADED_DOMAINS data.  */
  __libc_rwlock_wrlock (lock);

  /* Create all possible locale entries which might be interested in
     generalization.  */
  retval = _nl_make_l10nflist (&_nl_loaded_domains, dirname,
			       strlen (dirname) + 1, mask, language, territory,
			       codeset, normalized_codeset, modifier,
			       domainname, 1);
  __libc_rwlock_unlock (lock);

  if (retval == NULL)
    /* This means we are out of core.  */
    goto out;

  if (retval->decided <= 0)
    _nl_load_domain (retval, domainbinding);
  if (retval->data == NULL)
    {
      int cnt;
      for (cnt = 0; retval->successor[cnt] != NULL; ++cnt)
	{
	  if (retval->successor[cnt]->decided <= 0)
	    _nl_load_domain (retval->successor[cnt], domainbinding);
	  if (retval->successor[cnt]->data != NULL)
	    break;
	}
    }

out:
  /* The space for normalized_codeset is dynamically allocated.  Free it.  */
  if (mask & XPG_NORM_CODESET)
    free ((void *) normalized_codeset);

  return retval;
}

