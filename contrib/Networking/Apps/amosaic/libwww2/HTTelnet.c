/*		Telnet Acees, Roligin, etc			HTAccess.c
**		==========================
**
** Authors
**	TBL	Tim Berners-Lee timbl@info.cern.ch
**	JFG	Jean-Francois Groff jgh@next.com
**	DD	Denis DeLaRoca (310) 825-4580  <CSP1DWD@mvs.oac.ucla.edu>
** History
**	 8 Jun 92 Telnet hopping prohibited as telnet is not secure (TBL)
**	26 Jun 92 When over DECnet, suppressed FTP, Gopher and News. (JFG)
**	 6 Oct 92 Moved HTClientHost and logfile into here. (TBL)
**	17 Dec 92 Tn3270 added, bug fix. (DD)
**	 2 Feb 93 Split from HTAccess.c. Registration.(TBL)
*/

/* Implements:
*/
#include "HTTelnet.h"

#include "HTParse.h"
#include "HTUtils.h"
#include "HTAnchor.h"
#include "HTTP.h"
#include "HTFile.h"
#include <errno.h>
#include <stdio.h>

#include "tcp.h"
#include "HText.h"

#include "HTAccess.h"
#include "HTAlert.h"

#if defined(_AMIGA) || defined(__AROS__)
#include <dos/dos.h>
#include <dos/dostags.h>
#include <proto/dos.h>
#endif
extern void application_user_feedback (char *);


#if !defined(_AMIGA) && !defined(__AROS__)
/*	make a string secure for passage to the
**	system() command.  Make it contain only alphanumneric
**	characters, or the characters '.', '-', '_', '+'.
**	Also remove leading '-' or '+'.
**	-----------------------------------------------------
*/
PRIVATE void make_system_secure ARGS1(char *, str)
{
	char *ptr1, *ptr2;

	if ((str == NULL)||(*str == '\0'))
	{
		return;
	}

	/*
	 * remove leading '-' or '+' by making it into whitespace that
	 * will be stripped later.
	 */
	if ((*str == '-')||(*str == '+'))
	{
		*str = ' ';
	}

	ptr1 = ptr2 = str;

	while (*ptr1 != '\0')
	{
		if ((!isalpha((int)*ptr1))&&(!isdigit((int)*ptr1))&&
			(*ptr1 != '.')&&(*ptr1 != '_')&&
			(*ptr1 != '+')&&(*ptr1 != '-'))
		{
			ptr1++;
		}
		else
		{
			*ptr2 = *ptr1;
			ptr2++;
			ptr1++;
		}
	}
	*ptr2 = *ptr1;
}

PRIVATE void run_a_command ARGS1(char *, command)
{
	char **argv;
	int argc;
	char *str;
	int alen;

	alen = 10;
	argv = (char **)malloc(10 * sizeof(char *));
	if (argv == NULL)
	{
		return;
	}
	argc = 0;

	str = strtok(command, " \t\n");
	while (str != NULL)
	{
		argv[argc] = strdup(str);
		argc++;
		if (argc >= alen)
		{
			int i;
			char **tmp_av;

			tmp_av = (char **)malloc((alen + 10) * sizeof(char *));
			if (tmp_av == NULL)
			{
				return;
			}
			for (i=0; i<alen; i++)
			{
				tmp_av[i] = argv[i];
			}
			alen += 10;
			free((char *)argv);
			argv = tmp_av;
		}
		str = strtok(NULL, " \t\n");
	}
	argv[argc] = NULL;

	if (fork() == 0)
	{
		execvp(argv[0], argv);
	}
	else
	{
		int i;

		/*
		 * The signal handler in main.c will clean this child
		 * up when it exits.
		 */

		for (i=0; i<argc; i++)
		{
			if (argv[i] != NULL)
			{
				free(argv[i]);
			}
		}
		free((char *)argv);
	}
}
#endif

/*	Telnet or "rlogin" access
**	-------------------------
*/
PRIVATE int remote_session ARGS2(char *, access, char *, host)
{
  char *user, *hostname, *port;
  int portnum;
  char command[256];
  char *xterm_str;
  enum _login_protocol { telnet, rlogin, tn3270 } login_protocol;
#if defined(_AMIGA) || defined(__AROS__)
  /* FIXME: remote_session doesn't do tn3270 properly for Amiga */
  BPTR seglist;
  extern char *amiga_telnet;
  extern char *amiga_rlogin;
  extern void application_user_feedback (char *);
#else
  extern char *global_xterm_str;
#endif

  if (!access || !host)
    {
      application_user_feedback
	("Cannot open remote session, because\nURL is malformed.\0");
      return HT_NO_DATA;
    }

  login_protocol =
    strcmp(access, "rlogin") == 0 ? rlogin :
      strcmp(access, "tn3270") == 0 ? tn3270 :
	telnet;

  /* Make sure we won't overrun the size of command with a huge host string */
  if (strlen(host) > 200)
    {
	host[200] = '\0';
    }

  user = host;
  hostname = strchr(host, '@');
  port = strchr(host, ':');

  if (hostname)
    {
      *hostname++ = 0;	/* Split */
    }
  else
    {
      hostname = host;
      user = 0; 	/* No user specified */
    }
  if (port)
    {
      *port++ = 0;	/* Split */
      portnum = atoi(port);
    }

#if !defined(_AMIGA) && !defined(__AROS__)
  /*
   * Make user and hostname secure by removing leading '-' or '+'.
   * and allowing only alphanumeric, '.', '_', '+', and '-'.
   */
  make_system_secure(user);
  make_system_secure(hostname);

  xterm_str = global_xterm_str;

  if (login_protocol == rlogin)
    {
      /* For rlogin, we should use -l user. */
      if ((port)&&(portnum > 0)&&(portnum < 63336))
	{
	  sprintf(command, "%s -e %s %s %d %s %s", xterm_str, access,
	      hostname,
	      portnum,
	      user ? "-l" : "",
	      user ? user : "");
	}
      else
	{
	  sprintf(command, "%s -e %s %s %s %s", xterm_str, access,
	      hostname,
	      user ? "-l" : "",
	      user ? user : "");
	}
    }
  else
    {
      /* For telnet, -l isn't safe to use at all -- most platforms
	 don't understand it. */
      if ((port)&&(portnum > 0)&&(portnum < 63336))
	{
	  sprintf(command, "%s -e %s %s %d", xterm_str, access,
	      hostname, portnum);
	}
      else
	{
	  sprintf(command, "%s -e %s %s", xterm_str, access,
	      hostname);
	}
    }

  if (TRACE) fprintf(stderr, "HTaccess: Command is: %s\n", command);
  run_a_command(command);
#else /* Amiga */
#ifdef _AS225
  if (port && (portnum <= 0 || portnum > 63336))
	port = NULL;

  if (login_protocol == rlogin)
  {
	if (amiga_rlogin && *amiga_rlogin)
	    xterm_str = amiga_rlogin;
	else
	    xterm_str = "inet:c/rlogin";
	/* doesn't allow port specification right now... */
	sprintf (command, "%s %s HOST %s",
		user ? "user" : "",
		user ?	user  : "",
		hostname);
  }
  else
  {
	if (amiga_telnet && *amiga_telnet)
	    xterm_str = amiga_telnet;
	else
	    xterm_str = "inet:c/telnet";
	sprintf (command, "%s %s",
		hostname,
		port ? port : "");
  }

  if (TRACE)
  {
	fprintf (stderr, "HTTelnet: command is '%s', arguments are '%s'\n", xterm_str, command);
  }

  if (seglist = NewLoadSegTags (xterm_str, TAG_DONE))
  {
	struct Process
		*proc;

	if (TRACE)
	{
		fprintf (stderr, "HTTelnet: got seglist for %s\n", xterm_str);
	}
	proc =	CreateNewProcTags (
			   NP_Seglist,	   seglist,
			   NP_FreeSeglist, TRUE,
			   NP_Name,	   "Mosaic Remote Session",
			   NP_Cli,	   TRUE,
			   NP_StackSize,   32000,
			   NP_CommandName, xterm_str,
			   NP_Arguments,   command,
			   TAG_DONE);
	if (TRACE)
	{
		if (proc)
			fprintf (stderr, "HTTelnet: new process is 0x%lx\n", (long) proc);
		else
			fprintf (stderr, "HTTelnet: CreateNewProc() failed. IoErr %ld\n", IoErr ());
	}

	if (!proc)
	{
		static const char
		    msg_fmt [] = "Could not start '%s %s'";
		char
		    *buf = malloc (strlen (msg_fmt) + strlen (xterm_str) + strlen (command) + 8);

		if (buf)
		{
			sprintf (buf, msg_fmt, xterm_str, command);
			application_user_feedback (buf);
			free (buf);
		}
	}
  }
  else
  {
	static const char
	    msg_fmt [] = "Could not load '%s'";
	char
	    *buf = malloc (strlen (msg_fmt) + strlen (xterm_str) + 8);

	if (buf)
	{
		sprintf (buf, msg_fmt, xterm_str);
		application_user_feedback (buf);
		free (buf);
	}
  }
#else /* AmiTCP */
  if (port && (portnum < 0 || 63336 < portnum))
	port = NULL;

  xterm_str = "amitcp:bin/napsaterm";
  command [0] = 0;

  if (login_protocol == rlogin)
  {
	if (amiga_rlogin && *amiga_rlogin)
	{
	    xterm_str = amiga_rlogin;
	    sprintf (command, "%s %s HOST %s",
		    user ? "user" : "",
		    user ?  user  : "",
		    hostname);
	}
  }
  else
  {
	if (amiga_telnet && *amiga_telnet)
	{
	    xterm_str = amiga_telnet;
	    sprintf (command, "%s %s",
		    hostname,
		    port ? port : "");
	}
  }

  if (command [0] == 0)
  {
	/* Arguments only in command, to avoid security problems */
	sprintf (command, "-S Mosaic -d %s %s %s %s %s %s\n",
	    login_protocol == rlogin ? "NET" : "TELNET",
	    user ? "-r" : "",
	    user ? user : "",
	    port ? "-s" : "",
	    port ? port : "",
	    hostname);
  }

  seglist = NewLoadSegTags (xterm_str, TAG_DONE) ;
  if (seglist)
  {
	struct Process
	    *proc;

	proc = CreateNewProcTags (NP_Seglist, seglist,
				 NP_FreeSeglist, TRUE,
				 NP_Name, "Mosaic Remote Session",
				 NP_Cli, TRUE,
				 NP_CommandName, xterm_str,
				 NP_Arguments, command,
				 TAG_DONE);
	if (!proc)
	{
		static const char
		    msg_fmt [] = "Could not start '%s %s'";
		char
		    *buf = malloc (strlen (msg_fmt) + strlen (xterm_str) + strlen (command) + 8);

		if (buf)
		{
			sprintf (buf, msg_fmt, xterm_str, command);
			application_user_feedback (buf);
			free (buf);
		}
	}
  }
  else
  {
	static const char
	    msg_fmt [] = "Could not load '%s'";
	char
	    *buf = malloc (strlen (msg_fmt) + strlen (xterm_str) + 8);

	if (buf)
	{
		sprintf (buf, msg_fmt, xterm_str);
		application_user_feedback (buf);
		free (buf);
	}
  }
#endif /* AmiTCP */
#endif /* Amiga */

  /* No need for application feedback if we're rlogging directly
     in... */
  if (user && login_protocol != rlogin)
    {
      char str[200];
      /* Sleep to let the xterm get up first.
	 Otherwise, the popup will get buried. */
      sleep (2);
      sprintf (str, "When you are connected, log in as '%s'.\0", user);
      application_user_feedback (str);
    }

  return HT_NO_DATA;		/* Ok - it was done but no data */
}

/*	"Load a document" -- establishes a session
**	------------------------------------------
**
** On entry,
**	addr		must point to the fully qualified hypertext reference.
**
** On exit,
**	returns 	<0	Error has occured.
**			>=0	Value of file descriptor or socket to be used
**				 to read data.
**	*pFormat	Set to the format of the file, if known.
**			(See WWW.h)
**
*/
PRIVATE int HTLoadTelnet
ARGS4
(
 CONST char *,		addr,
 HTParentAnchor *,	anchor,
 HTFormat,		format_out,
 HTStream *,		sink			/* Ignored */
)
{
    char * access;

    char * host;
    int status;

    if (sink)
      {
	HTAlert("Can't output a live session -- it has to be interactive");
	return HT_NO_ACCESS;
      }
    access =  HTParse(addr, "file:", PARSE_ACCESS);

    host = HTParse(addr, "", PARSE_HOST);
    status = remote_session(access, host);

    free(host);
    free(access);
    return status;
}


PUBLIC HTProtocol HTTelnet = { "telnet", HTLoadTelnet, NULL };
PUBLIC HTProtocol HTRlogin = { "rlogin", HTLoadTelnet, NULL };
PUBLIC HTProtocol HTTn3270 = { "tn3270", HTLoadTelnet, NULL };
