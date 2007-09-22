/* 
 *     XaoS, a fast portable realtime fractal zoomer 
 *                  Copyright (C) 1996,1997 by
 *
 *      Jan Hubicka          (hubicka@paru.cas.cz)
 *      Thomas Marsh         (tmarsh@austin.ibm.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifdef _plan9_
#include <u.h>
#include <libc.h>
#else
#include <string.h>
#include <stdlib.h>
#endif
#include <config.h>
#include <ui.h>
#include <param.h>
/*extern struct params *global_params; */
extern int testik;
extern struct params global_params[];
extern struct ui_driver *drivers[];
extern int ndrivers;

int 
params_parser (int argc, char **argv)
{
  int i, p = 0, d;
  struct params *par = NULL;
  int error = 0;
  int found;
  for (i = 1; i < argc && !error; i++)
    {
      found = 0;
      for (d = -1; d < ndrivers; d++)
	{
	  if (d == -1)
	    par = global_params;
	  else
	    par = drivers[d]->params;
	  for (p = 0; par[p].name != NULL && !error; p++)
	    {
	      if (!strcmp (par[p].name, argv[i]))
		{
		  found = 1;
		  switch (par[p].type)
		    {
		    case P_SWITCH:
		      *((int *) par[p].value) = 1;
		      break;
		    case P_NUMBER:
		      {
			int n;
			if (i == argc - 1)
			  {
			    fprintf (stderr, "parameter %s requires numeric value\n", argv[i]);
			    error = 1;
			    break;
			  }
			if (sscanf (argv[i + 1], "%i", &n) != 1)
			  {
			    fprintf (stderr, "parameter for %s is not number \n", argv[i]);
			    error = 1;
			    break;
			  }
			*((int *) par[p].value) = n;
			i++;
		      }
		      break;
		    case P_FLOAT:
		      {
			float n;
			if (i == argc - 1)
			  {
			    fprintf (stderr, "parameter %s requires floating point numeric value\n", argv[i]);
			    error = 1;
			    break;
			  }
			if (sscanf (argv[i + 1], "%f", &n) != 1)
			  {
			    fprintf (stderr, "parameter for %s is not floating point number \n", argv[i]);
			    error = 1;
			    break;
			  }
			*((float *) par[p].value) = n;
			i++;
		      }
		      break;
		    case P_STRING:
		      {
			if (i == argc - 1)
			  {
			    fprintf (stderr, "parameter %s requires string value\n", argv[i]);
			    error = 1;
			    break;
			  }
			i++;
			*((char **) par[p].value) = *(argv + i);
		      }
		    }
		}
	    }
	  if (par[p].name != NULL)
	    break;
	}
      if (d == ndrivers && !found)
	{
	  fprintf (stderr, "option %s is unknown\n", argv[i]);
	  error = 1;
	  break;
	}
    }
  if (error)
    {
      const char *name[] =
      {
	"",
	"number",
	"string",
	"f.point"
      };
      printf ("                 XaoS help text\n");
      printf (" (This help is genereated automagically. I am sorry for all inconvencies)\n");
      printf ("option string  value   description\n");
      for (d = -1; d < ndrivers; d++)
	{
	  if (d == -1)
	    par = global_params;
	  else
	    {
	      par = drivers[d]->params;
	      printf ("\n%s driver options:\n\n", drivers[d]->name);
	    }
	  for (p = 0; par[p].name != NULL; p++)
	    {
	      if (par[p].type == P_HELP)
		printf ("\n%s\n\n", par[p].help);
	      else
		printf (" %-15s%-8s%s\n", par[p].name, name[par[p].type], par[p].help);
	    }
	  if (p == 0)
	    printf (" No options avaiable for now\n");
	}
      exit (-1);

    }
  return (1);
}
