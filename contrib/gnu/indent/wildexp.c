/* Wildcard expansion for WIN32 version
 *
 * Copyright (c) 2000 John Bridges.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Please send bug reports about this file to jbridges@netcom.com
 */

#ifdef _WIN32
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <direct.h>
#include <malloc.h>
#include <ctype.h>
#include <io.h>

#define MAX_PATH 1024

struct arginfo
{
  int nargc;
  int argpos;
  char *argbuf;
  int bufsize;
};

static void
myabort (char *str)
{
  puts ("");
  puts (str);
  exit (1);
}

void fparms (struct arginfo *arg, char *fname);
void chkparm (struct arginfo *arg, char *str);

static void
addarg (struct arginfo *arg, char *str)
{
  int cnt;

  if (*str == '@')
    {
      fparms (arg, str + 1);
    }
  else
    {
      cnt = strlen (str) + 1;
      if (arg->argpos + cnt >= arg->bufsize)
	{
	  arg->bufsize += 8192;
	  arg->argbuf = realloc (arg->argbuf, arg->bufsize);
	}
      strcpy (arg->argbuf + arg->argpos, str);
      arg->argpos += cnt;
      ++arg->nargc;
    }
}

static void
fparms (struct arginfo *arg, char *fname)
{
  char *buf;
  char *cp, *ecp;
  FILE *fs;
  unsigned int cnt;
  short quoted = 0;

  fs = fopen (fname, "rb");
  if (!fs)
    myabort ("CANNOT FIND '@' FILE!");

  fseek (fs, 0l, 2);
  cnt = ftell (fs);
  fseek (fs, 0l, 0);
  buf = malloc (cnt + 1);
  fread (buf, 1, cnt, fs);
  fclose (fs);
  *(ecp = buf + cnt) = 0;
  cp = buf;
  while (cp < ecp)
    {
      if (*cp == ';')
	{
	  while (cp < ecp && *cp != '\r' && *cp != '\n')
	    *cp++ = 0;
	}
      if (*cp < ' ')
	*cp = 0;

      if (*cp == '"')
	{
	  *cp = 0;
	  quoted = !quoted;
	}
      if (!quoted)
	{
	  if (*cp == ' ')
	    *cp = 0;		/* set spaces to null */
	  if (*cp >= 'A' && *cp <= 'Z')
	    *cp += 32;
	}
      ++cp;
    }
  cp = buf;
  while (cp < ecp)
    {
      if (*cp)
	{
	  chkparm (arg, cp);
	  cp += strlen (cp);
	}
      ++cp;
    }
  free (buf);
}

static int
wildmatch (char *s1, char *s2)
{
  if (s1 == NULL)
    s1 = "";
  if (s2 == NULL)
    s2 = "";

  while (*s1 && (*s2 || *s1 == '*'))
    {
      if (tolower (*s1) != tolower (*s2) && *s1 != '?' && *s1 != '*')
	return 0;
      if (*s2)
	{
	  if (*s1 == '*')
	    {
	      ++s1;
	      if (!*s1)
		return 1;

	      while (*s2)
		{
		  if (wildmatch (s1, s2))
		    return 1;
		  ++s2;
		}
	      return 0;
	    }
	  ++s2;
	}
      ++s1;
    }
  if (*s1 || *s2)
    return 0;
  else
    return 1;
}


static short
removearg (struct arginfo *arg, char *str)
{
  char *cp;
  int i;
  int cnt;

  i = arg->nargc;
  cp = arg->argbuf;
  while (i-- > 0)
    {
      cnt = strlen (cp) + 1;
      if (wildmatch (str, cp))
	{
	  arg->argpos -= cnt;
	  memcpy (cp, cp + cnt, arg->argpos - (cp - arg->argbuf));
	  --arg->nargc;
	}
      else
	{
	  cp += cnt;
	}
    }
  return 0;
}


static void
chkparm (struct arginfo *arg, char *str)
{
  char tmpstr[MAX_PATH + 1];
  char *cp;
  char *pnt2;
  short remarg;
  long hFile;
  struct _finddata_t c_file;

  remarg = 0;
  if (*str == '!')
    {
      remarg = 1;
      ++str;
    }
  if (strchr (str, '?') || strchr (str, '*'))
    {
      if (remarg)
	{
	  removearg (arg, str);
	}
      else
	{
	  strcpy (tmpstr, str);
	  if (!(cp = strrchr (tmpstr, '\\')))
	    if (!(cp = strrchr (tmpstr, '/')))
	      if (!(cp = strrchr (tmpstr, ':')))
		cp = tmpstr - 1;
	  pnt2 = cp + 1;
	  hFile = _findfirst (str, &c_file);
	  while (hFile > 0)
	    {
	      if (!(c_file.attrib & _A_SUBDIR))
		{
		  cp = pnt2;
		  strcpy (pnt2, c_file.name);
		  while (*cp)
		    {
		      if (islower (*cp))
			{
			  strcpy (pnt2, c_file.name);
			  break;
			}
		      *cp = tolower (*cp);
		      ++cp;
		    }
		  addarg (arg, tmpstr);
		}
	      if (_findnext (hFile, &c_file))
		{
		  _findclose (hFile);
		  hFile = 0;
		}
	    }
	}
    }
  else
    {
      if (remarg)
	removearg (arg, str);
      else
	addarg (arg, str);
    }
}

void
wildexp (int *argc, char ***argv)
{
  char *cp;
  char **pnt;
  char **nargv;
  struct arginfo arg;
  int i;

  arg.bufsize = 8192;
  arg.argpos = 0;
  arg.argbuf = malloc (arg.bufsize);
  arg.nargc = 0;
  i = *argc;
  pnt = *argv;

  while (i--)
    chkparm (&arg, *pnt++);

  arg.argbuf = realloc (arg.argbuf, arg.argpos);
  cp = arg.argbuf;
  i = *argc = arg.nargc;
  if (arg.nargc < 32)
    arg.nargc = 32;
  *argv = nargv = malloc (arg.nargc * sizeof (nargv[0]));
  while (i--)
    {
      *nargv++ = cp;
      cp += strlen (cp) + 1;
    }
}

#endif
