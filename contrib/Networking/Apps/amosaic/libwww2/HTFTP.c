/*			File Transfer Protocol (FTP) Client
**			for a WorldWideWeb browser
**			===================================
**
**	A cache of control connections is kept (NOT ANYMORE -- IT WAS BROKEN)
**
** Note: Port allocation
**
**	It is essential that the port is allocated by the system, rather
**	than chosen in rotation by us (POLL_PORTS), or the following
**	problem occurs.
**
**	It seems that an attempt by the server to connect to a port which has
**	been used recently by a listen on the same socket, or by another
**	socket this or another process causes a hangup of (almost exactly)
**	one minute. Therefore, we have to use a rotating port number.
**	The problem remains that if the application is run twice in quick
**	succession, it will hang for what remains of a minute.
**
** Authors
**	TBL	Tim Berners-lee <timbl@info.cern.ch>
**	DD	Denis DeLaRoca 310 825-4580 <CSP1DWD@mvs.oac.ucla.edu>
**	MWM	Mike Meyer
** History:
**	 2 May 91	Written TBL, as a part of the WorldWideWeb project.
**	15 Jan 92	Bug fix: close() was used for NETCLOSE for control soc
**	10 Feb 92	Retry if cached connection times out or breaks
**	 8 Dec 92	Bug fix 921208 TBL after DD
**	17 Dec 92	Anon FTP password now just WWWuser@ suggested by DD
**			fails on princeton.edu!
**	18 Oct 93	Allow ftp past firewall sometimes
**
*/

/* SOCKS mods by:
 * Ying-Da Lee, <ylee@syl.dl.nec.com>
 * NEC Systems Laboratory
 * C&C Software Technology Center
 */

#include "HTFTP.h"      /* Implemented here */

#define LINE_LENGTH 1024

#include "HTParse.h"
#include "HTUtils.h"
#include "tcp.h"
#include "HTTCP.h"
#include "HTAnchor.h"
#include "HTFile.h"
#include "HTChunk.h"
#include "HTSort.h"
#include "HText.h"
#include "HTAlert.h"

#ifndef _DNET

#ifndef IPPORT_FTP
#define IPPORT_FTP	21
#endif

#ifdef __STDC__
#include <stdlib.h>
#endif

/* #define TRACE 1 */

#ifndef NIL
#define NIL 0
#endif

/*		Hypertext object building machinery
*/
#include "HTML.h"

#define PUTC(c) (*targetClass.put_character)(target, c)
#define PUTS(s) (*targetClass.put_string)(target, s)
#define START(e) (*targetClass.start_element)(target, e, 0, 0)
#define END(e) (*targetClass.end_element)(target, e)
#define END_TARGET (*targetClass.end_document)(target)
#define FREE_TARGET (*targetClass.free)(target)
struct _HTStructured {
	CONST HTStructuredClass *	isa;
	/* ... */
};

struct _HTStream {
      CONST HTStreamClass*	isa;
      /* ... */
};


#ifdef SOCKS
extern struct in_addr SOCKS_ftpsrv; /* in HTFTP.c */
#endif


/*	Module-Wide Variables
**	---------------------
*/
PRIVATE char	response_text[LINE_LENGTH+1];/* Last response from NewsHost */
PRIVATE int control = -1;		/* Current connection */
PRIVATE int	data_soc = -1;		/* Socket for data transfer =invalid */

PRIVATE int	master_socket = -1;	/* Listening socket = invalid	*/
PRIVATE char	port_command[255];	/* Command for setting the port */
PRIVATE BOOL	passive;		/* we have a passive socket */


#define DATA_BUFFER_SIZE 2048
PRIVATE char data_buffer[DATA_BUFFER_SIZE];		/* Input data buffer */
PRIVATE char * data_read_pointer;
PRIVATE char * data_write_pointer;


/*	Procedure: Read a character from the data connection
**	----------------------------------------------------
*/
PRIVATE int interrupted_in_next_data_char = 0;
PRIVATE char next_data_char NOARGS
{
  int status;
  interrupted_in_next_data_char = 0;
  if (data_read_pointer >= data_write_pointer)
    {
      status = NETREAD(data_soc, data_buffer, DATA_BUFFER_SIZE);
      if (status == HT_INTERRUPTED)
	interrupted_in_next_data_char = 1;
      if (status <= 0)
	return (char)-1;
      data_write_pointer = data_buffer + status;
      data_read_pointer = data_buffer;
    }
  return *data_read_pointer++;
}


/*	Execute Command and get Response
**	--------------------------------
**
**	See the state machine illustrated in RFC959, p57. This implements
**	one command/reply sequence.  It also interprets lines which are to
**	be continued, which are marked with a "-" immediately after the
**	status code.
**
**	Continuation then goes on until a line with a matching reply code
**	an a space after it.
**
** On entry,
**	con	points to the connection which is established.
**	cmd	points to a command, or is NIL to just get the response.
**
**	The command is terminated with the CRLF pair.
**
** On exit,
**	returns:  The first digit of the reply type,
**		  or negative for communication failure.
*/
#ifdef __STDC__
PRIVATE int response (char * cmd)
#else
PRIVATE int response (cmd)
    char * cmd;
#endif
{
  int result;				/* Three-digit decimal code */
#ifdef OLD
  int continuation_response = -1;
#endif
  int status;
  char	continuation;
  int multiline_response = 0;

  if (!control ){
      if(TRACE) fprintf(stderr, " BAD ERROR MESSAGE FTP: No control connection set up!!\n");
      if(TRACE) fprintf(stderr, " MJW FTP: actually control == 0 "
		    " WHICH IS PERFECTLY OK!!!! "
		    " NOT DOING return -99;");
  }
  if ( control == -1)
    {
      if(TRACE)
	fprintf(stderr, "FTP: No control connection set up!!\n");
      return -99;
    }

  if (cmd)
    {
      if (TRACE)
	fprintf(stderr, "  Tx: %s", cmd);

      status = NETWRITE(control, cmd, (int)strlen(cmd));
      if (status<0)
	{
	  if (TRACE) fprintf(stderr,
			     "FTP: Error %d sending command: closing socket %d\n",
			     status, control);
	  NETCLOSE(control);
	  control = -1;
	  return status;
	}
    }

  /* Patch to be generally compatible with RFC 959 servers  -spok@cs.cmu.edu  */
  /* Multiline responses start with a number and a hyphen;
     end with same number and a space.	When it ends, the number must
     be flush left. */
  do
    {
      char *p = response_text;
      /* If nonzero, it's set to initial code of multiline response */
      for (;;)
	{
	  int foo;
	  /* This is set to 0 at the start of HTGetCharacter. */
	  extern int interrupted_in_htgetcharacter;

	  foo = (*p++ = HTGetCharacter ());
	  if (interrupted_in_htgetcharacter)
	    {
	      if (TRACE)
		fprintf (stderr, "FTP: Interrupted in HTGetCharacter, apparently.\n");
	      NETCLOSE (control);
	      control = -1;
	      return HT_INTERRUPTED;
	    }

	  if (foo == LF ||
       /* if (((*p++=NEXT_CHAR) == '\n') || */
	      (p == &response_text[LINE_LENGTH]))
	    {
	      *p++=0;		      /* Terminate the string */
	      if (TRACE)
		fprintf(stderr, "    Rx: %s", response_text);
	      sscanf(response_text, "%d%c", &result, &continuation);
	      if (continuation == '-' && !multiline_response)
		{
		  multiline_response = result;
		}
	      else if (multiline_response && continuation == ' ' &&
		       multiline_response == result &&
		       isdigit(response_text[0]))
		{
		  /* End of response (number must be flush on left) */
		  multiline_response = 0;
		}
	      break;
	    } /* if end of line */

	  if (*(p-1) < 0)
	    {
	      if (TRACE)
		fprintf(stderr, "Error on rx: closing socket %d\n",
			control);
	      strcpy (response_text, "000 *** TCP read error on response\n");
	      NETCLOSE(control);
	      control = -1;
	      return -1;	/* End of file on response */
	    }
	} /* Loop over characters */
    }
  while (multiline_response);

#ifdef OLD
  do
    {
      char *p = response_text;
      for(;;)
	{
	  int foo;
	  /* This is set to 0 at the start of HTGetCharacter. */
	  extern int interrupted_in_htgetcharacter;

	  foo = (*p++ = HTGetCharacter ());
	  if (interrupted_in_htgetcharacter)
	    {
	      if (TRACE)
		fprintf (stderr, "FTP: Interrupted in HTGetCharacter, apparently.\n");
	      NETCLOSE (control);
	      control = -1;
	      return HT_INTERRUPTED;
	    }

	  if (foo == LF ||
	      p == &response_text[LINE_LENGTH])
	    {
	      char continuation;
	      int rv;

	      *p++=0;			/* Terminate the string */
	      if (TRACE)
		fprintf(stderr, "    Rx: %s", response_text);
	      /* Clear out result ahead of time to see if we couldn't
		 read a real value. */
	      result = -1;
	      rv = sscanf(response_text, "%d%c", &result, &continuation);
	      /* Try just continuing if we couldn't pull out
		 a value for result and the response_text starts with
		 whitespace. */
	      if (rv < 2 && result == -1 &&
		  (*response_text == ' ' || *response_text == '\t'))
		{
		  /* Dunno what to do here -- the code isn't really
		     set up to deal with continuation lines starting
		     with whitespace.  Testcase is
		     reports.adm.cs.cmu.edu. */
		}
	      else if (continuation_response == -1)
		{
		  if (continuation == '-')  /* start continuation */
		    continuation_response = result;
		}
	      else
		{	/* continuing */
		  if (continuation_response == result && continuation == ' ')
		    continuation_response = -1; /* ended */
		}
	      break;
	    } /* if end of line */

	  if (*(p-1) == EOF)
	    {
	      if (TRACE)
		fprintf(stderr, "Error on rx: closing socket %d\n",
			control);
	      strcpy (response_text, "000 *** TCP read error on response\n");
	      NETCLOSE(control);
	      control = -1;
	      return -1;	/* End of file on response */
	    }
	} /* Loop over characters */

    }
  while (continuation_response != -1);
#endif

  if (result == 421)
    {
      if(TRACE)
	fprintf(stderr, "FTP: They close so we close socket %d\n",
		control);
      NETCLOSE(control);
      return -1;
    }
  return result/100;
}


/*	Get a valid connection to the host
**	----------------------------------
**
** On entry,
**	arg	points to the name of the host in a hypertext address
** On exit,
**	returns <0 if error
**		socket number if success
**
**	This routine takes care of managing timed-out connections, and
**	limiting the number of connections in use at any one time.
**
**	It ensures that all connections are logged in if they exist.
**	It ensures they have the port number transferred.
*/
PRIVATE int get_connection ARGS1 (char *,arg)
{
  int status, con;

  char *username = 0;
  char *password = 0;
  char dummy[MAXHOSTNAMELEN+32];

  if (!arg)
    return -1;		/* Bad if no name sepcified	*/
  if (!*arg)
    return -1;		/* Bad if name had zero length	*/

  if (TRACE)
    fprintf(stderr, "FTP: Looking for %s\n", arg);

  {
    char *p1 = HTParse(arg, "", PARSE_HOST);
    char *p2 = strrchr(p1, '@');        /* user? */
    char * pw;
    if (p2)
      {
	username = p1;
	*p2=0;				  /* terminate */
	p1 = p2+1;			  /* point to host */
	pw = strchr(username, ':');
	if (pw)
	  {
	    *pw++ = 0;
	    password = pw;
	  }
      }

    /* copy hostname into dummy URL, since username:password@
       might have been part of original */
    sprintf(dummy, "ftp://%s", p1);

    if (!username)
      free(p1);
  }

  con = -1;
  status = HTDoConnect (dummy, "FTP", IPPORT_FTP, &con);

  if (status < 0)
    {
      if (TRACE)
	{
	  if (status == HT_INTERRUPTED)
	    fprintf (stderr,
		     "FTP: Interrupted on connect\n");
	  else
	    fprintf(stderr,
		    "FTP: Unable to connect to remote host for `%s'.\n",
		    arg);
	}
      if (status == HT_INTERRUPTED)
	HTProgress ("Connection interrupted.");
      if (con != -1)
	{
	  NETCLOSE(con);
	  con = -1;
	}
      if (username)
	free(username);
      HTProgress ("Unable to connect to remote host.");
      return status;			/* Bad return */
    }

  if (TRACE)
    fprintf(stderr, "FTP connected, assigning control socket %d\n", con);
  control = con;			/* Current control connection */

  /* Initialise buffering for contron connection */
  HTInitInput (con);

  /* Now we log in; Look up username, prompt for pw. */
  {
    int status = response (NIL);	/* Get greeting */

    if (status == HT_INTERRUPTED)
      {
	if (TRACE)
	  fprintf (stderr,
		   "FTP: Interrupted at beginning of login.\n");
	HTProgress ("Connection interrupted.");
	NETCLOSE(control);
	control = -1;
	return HT_INTERRUPTED;
      }
    if (status == 2)
      { 	/* Send username */
	char * command;
	if (username)
	  {
	    command = (char*)malloc(10+strlen(username)+2+1);
	    sprintf(command, "USER %s%c%c", username, CR, LF);
	  }
	else
	  {
	    command = (char*)malloc(25);
	    sprintf(command, "USER anonymous%c%c", CR, LF);
	  }
	status = response (command);
	free(command);
	if (status == HT_INTERRUPTED)
	  {
	    if (TRACE)
	      fprintf (stderr,
		       "FTP: Interrupted while sending username.\n");
	    HTProgress ("Connection interrupted.");
	    NETCLOSE(control);
	    control = -1;
	    return HT_INTERRUPTED;
	  }
      }
    if (status == 3)
      { 	/* Send password */
	char * command;
	if (password)
	  {
	    command = (char*)malloc(10+strlen(password)+2+1);
	    sprintf(command, "PASS %s%c%c", password, CR, LF);
	  }
	else
	  {
	    char * user = getenv("USER");
	    extern char *machine_with_domain;
	    char *host = machine_with_domain;
	    if (!user)
	      user = "WWWuser";
	    /* If not fully qualified, suppress it as ftp.uu.net
	       prefers a blank to a bad name */ /* check for null first MJW*/
	    if (!host || !strchr(host, '.')) host = "";

	    command = (char*)malloc(20+strlen(host)+2+1);
	    sprintf(command,
		    "PASS %s@%s%c%c", user ? user : "WWWuser",
		    host, CR, LF); /*@@*/
	  }
	status = response (command);
	free(command);
	if (status == HT_INTERRUPTED)
	  {
	    if (TRACE)
	      fprintf (stderr,
		       "FTP: Interrupted while sending password.\n");
	    HTProgress ("Connection interrupted.");
	    NETCLOSE(control);
	    control = -1;
	    return HT_INTERRUPTED;
	  }
      }
    if (username)
      free(username);

    if (status == 3)
      {
	char temp[80];
	sprintf (temp, "ACCT noaccount%c%c", CR, LF);
	status = response (temp);
	if (status == HT_INTERRUPTED)
	  {
	    if (TRACE)
	      fprintf (stderr,
		       "FTP: Interrupted while sending ACCT.\n");
	    HTProgress ("Connection interrupted.");
	    NETCLOSE(control);
	    control = -1;
	    return HT_INTERRUPTED;
	  }
      }
    if (status != 2)
      {
	if (TRACE)
	  fprintf(stderr, "FTP: Login fail: %s", response_text);
	NETCLOSE(control);
	control = -1;
	return -1;		/* Bad return */
      }
    if (TRACE)
      fprintf(stderr, "FTP: Logged in.\n");
  }

  return con;			/* Good return */
} /* Scope of con */



/*	Close Master (listening) socket
**	-------------------------------
**
**
*/
#ifdef __STDC__
PRIVATE void close_master_socket(void)
#else
PRIVATE void close_master_socket()
#endif
{
  if (TRACE)
    fprintf(stderr, "FTP: Closing master socket %d\n", master_socket);
  if (master_socket >= 0) NETCLOSE(master_socket);
  master_socket = -1;

  return;
}


  /*	MWM   (mwm@contessa.phone.net) FIREWALL Code
   **	 THE END OF THIS SECTION IS EXPLICITLY MARKED BELOW
  **	Open a passive master socket for data
  **	-------------------------------------
  **
  **	With the server in passive mode, we connect to the server to get the
  **	data, rather than waiting for it to send us the data. This allows
  **	FTP to work through a firewall that allows outgoing but not incoming
  **	TCP connections.
  **
  ** On entry,
  **	master_socket	Must be negative if not set up already.
  ** On exit,
  **	Returns 	socket number if good,
  **			less than zero if error.
  **	data_soc	is socket number if good, else negative.
  **	passive 	is TRUE if remote server is passive, else FALSE.
  */
  #ifdef __STDC__
  PRIVATE int get_passive_socket(void)
  #else
  PRIVATE int get_passive_socket()
  #endif
  {
      int status;
      struct sockaddr_in soc_address;		/* Binary network address */
      char pasv_command[6];

      passive = FALSE;
      sprintf(pasv_command, "PASV%c%c", CR, LF);
      status = response(pasv_command);
      CTRACE(stderr, "FTP: PASV command returned %d\n", status);
      if (2 != status) return -status ;

  /*	The other end sent us a port number; get the host and port number
  */
      {
	char host_addr[16];
	int host_port;
	char *resp;
	int h1, h2, h3, h4;	/* Host number */
	int p1, p2;		/* Port number */

	resp = index(response_text, '(');
	if (!resp) {	/* Sigh */
	    resp = response_text + strlen(response_text) - 1 ;
	    /* Back over any trailing whitespace */
	    while (resp > response_text && isspace(*resp))
		resp -= 1 ;
	    /* Now, back over the port numbers */
	    while (resp > response_text && !isspace(*resp))
		resp -= 1 ;
	}
	sscanf(resp + 1, "%d,%d,%d,%d,%d,%d", &h1, &h2, &h3, &h4, &p1, &p2);
	host_port = (p1 << 8) + p2;
	sprintf(host_addr, "%d.%d.%d.%d", h1, h2, h3, h4);
	CTRACE(stderr, "FTP: Passive port at %d, IP address %s\n",
		host_port, host_addr);

	soc_address.sin_family = AF_INET;
	soc_address.sin_port = htons(host_port);
	soc_address.sin_addr.s_addr = inet_addr(host_addr);
      }

  /*	let's get a socket set up from the server.
  */
      {
	int new_socket;

	status = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (status < 0) {
	    HTInetStatus("socket");
	    return status;
	}
	new_socket = status;

	status = connect(new_socket, (struct sockaddr *)&soc_address,
	    sizeof(soc_address));
	if (status < 0) {
	    HTInetStatus("connect");
	    CTRACE(stderr,
		"FTP: Unable to connect to remote host via passive socket.\n");
	    NETCLOSE(new_socket);
	    return status;		/* Bad return! Bad! Bad! */
	}
	CTRACE(stderr, "FTP: connected to pasive datasocket %d\n", new_socket);

	if (data_soc >= 0) {
	    NETCLOSE(data_soc);
	    CTRACE(stderr, "FTP: Closing data socket %d\n", data_soc);
	}
	data_soc = new_socket;
	passive = TRUE;
	return new_socket;
      }
  }	/* get_data_socket */

/* END OF MWM (mwm@contessa.phone.net) FIREWALL CODE */

/*	Open a master socket for listening on
**	-------------------------------------
**
**	When data is transferred, we open a port, and wait for the server to
**	connect with the data.
**
** On entry,
**	master_socket	Must be negative if not set up already.
** On exit,
**	Returns 	socket number if good
**			less than zero if error.
**	master_socket	is socket number if good, else negative.
**	port_number	is valid if good.
*/
#ifdef __STDC__
PRIVATE int get_listen_socket(void)
#else
PRIVATE int get_listen_socket()
#endif
{
  struct sockaddr_in soc_address;	/* Binary network address */
  struct sockaddr_in *sin = &soc_address;
  int new_socket;			/* Will be master_socket */

  /* Create internet socket */
  new_socket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (new_socket < 0)
    return -1;

  if (TRACE)
    fprintf(stderr, "FTP: Opened master socket number %d\n", new_socket);

  /* Search for a free port. */
  sin->sin_family = AF_INET;	    /* Family = internet, host order  */
  sin->sin_addr.s_addr = INADDR_ANY; /* Any peer address */
  {
    int status;
    int address_length = sizeof(soc_address);
#ifdef SOCKS
    status = Rgetsockname(control,
#else
    status = getsockname(control,
#endif
			 (struct sockaddr *)&soc_address,
			 &address_length);
    if (status<0)
      return -1;
    CTRACE(tfp, "FTP: This host is %s\n",
	   HTInetString(sin));

    soc_address.sin_port = 0; /* Unspecified: please allocate */
#ifdef SOCKS
    status=Rbind(new_socket,
#else
    status=bind(new_socket,
#endif
	      (struct sockaddr*)&soc_address,
		/* Cast to generic sockaddr */
#ifdef SOCKS
	      sizeof(soc_address), SOCKS_ftpsrv.s_addr);
#else
	      sizeof(soc_address));
#endif
      if (status<0)
	return -1;

      address_length = sizeof(soc_address);
#ifdef SOCKS
    status = Rgetsockname(new_socket,
#else
    status = getsockname(new_socket,
#endif
			   (struct sockaddr*)&soc_address,
			   &address_length);
    if (status<0)
      return -1;
  }

  CTRACE(tfp, "FTP: bound to port %d on %s\n",
	 (int)ntohs(sin->sin_port),
	 HTInetString(sin));

  if (master_socket >= 0)
    close_master_socket ();

  master_socket = new_socket;

  /* Now we must find out who we are to tell the other guy */
  (void)HTHostName();	/* Make address valid - doesn't work*/
  sprintf(port_command, "PORT %d,%d,%d,%d,%d,%d%c%c",
	  (int)*((unsigned char *)(&sin->sin_addr)+0),
	  (int)*((unsigned char *)(&sin->sin_addr)+1),
	  (int)*((unsigned char *)(&sin->sin_addr)+2),
	  (int)*((unsigned char *)(&sin->sin_addr)+3),
	  (int)*((unsigned char *)(&sin->sin_port)+0),
	  (int)*((unsigned char *)(&sin->sin_port)+1),
	  CR, LF);

  /* Inform TCP that we will accept connections */
#ifdef SOCKS
  if (Rlisten (master_socket, 1) < 0)
#else
  if (listen (master_socket, 1) < 0)
#endif
    {
      close_master_socket ();
      return -1;
    }
  CTRACE(tfp, "FTP: Master socket(), bind() and listen() all OK\n");

  return master_socket; 	/* Good */
} /* get_listen_socket */



/*	Read a directory into an hypertext object from the data socket
**	--------------------------------------------------------------
**
** On entry,
**	anchor		Parent anchor to link the this node to
**	address 	Address of the directory
** On exit,
**	returns 	HT_LOADED if OK
**			<0 if error.
**
** Author: Charles Henrich (henrich@crh.cl.msu.edu)  October 2, 1993
**
** Completely re-wrote this chunk of code to present FTP directory information
** in a much more useful manner.  Also included the use of icons. -Crh
*/
PRIVATE int read_directory
ARGS4 (
  HTParentAnchor *,		parent,
  CONST char *, 		address,
  HTFormat,			format_out,
  HTStream *,			sink )
{
  HText *HT = HText_new ();
  HTFormat format;
  HTAtom *pencoding;
  char *filename = HTParse(address, "", PARSE_PATH + PARSE_PUNCTUATION);
  char buffer[BUFSIZ];
  char itemtype;
  char itemname[BUFSIZ];
  char itemsize[BUFSIZ];
  char *full_ftp_name, *ptr;
  int count, ret, cmpr, c, rv;
  extern char *HTgeticonname(HTFormat, char *);

  HText_beginAppend (HT);

  HText_appendText (HT, "<H1>FTP Directory ");
  HText_appendText (HT, filename);
  HText_appendText (HT, "</H1>\n");
  HText_appendText (HT, "<DL>\n");
  data_read_pointer = data_write_pointer = data_buffer;

  /* If this isnt the root level, spit out a parent directory entry */

  if(strcmp(filename,"/") != 0)
    {
      HText_appendText(HT,"<DD>");

      HText_appendText(HT,"<A HREF=\"");

      strcpy(buffer,filename);
      ptr = strrchr(buffer,'/');

      if(ptr != NULL) *ptr='\0';

      if(buffer[0] == '\0')
	HText_appendText(HT,"/");
      else
	HText_appendText(HT, buffer);

      HText_appendText(HT,"\"><IMG SRC=\"");
      HText_appendText(HT, HTgeticonname(NULL, "directory"));
      HText_appendText(HT,"\"> Parent Directory</a>");
    }

  /* Loop until we hit EOF */
  while(1)
    {
      /* Read in a line of data */
      for(count = 0; count < BUFSIZ; count++)
	{
	  c = next_data_char ();
	  if (interrupted_in_next_data_char)
	    {
	      if (TRACE)
		fprintf (stderr, "FTP: Picked up interrupted_in_next_data_char\n");
	      return HT_INTERRUPTED;
	    }

	  if (c == '\r')
	    {
	      c = next_data_char ();
	      if (interrupted_in_next_data_char)
		{
		  if (TRACE)
		    fprintf
		      (stderr, "FTP: Picked up interrupted_in_next_data_char\n");
		  return HT_INTERRUPTED;
		}

	      if (c != '\n')
		break;
	    }

	  if (c == '\n' || c == (char)EOF)
	    break;

	  buffer[count] = c;
	}

      if(c == (char)EOF)
	break;

      buffer[count] = 0;

      /* Parse the input buffer, extract the item type, and the item size */

#if 0
      ret=sscanf(buffer,"%c%*9s%*d %*s %*s %s", &itemtype, itemsize);

      if (ret != 2)
	continue;
#endif
      /* Retain whole string -- we don't use it at the moment, but we will. */
      full_ftp_name = strdup (buffer);
      /* Read but disregard itemsize -- this effectively guarantees we will know
	 what we should display and what we shouldn't -- don't ask. */
      ret=sscanf(buffer,"%c%*9s%*d %*s %*s %s", &itemtype, itemsize);

      if (ret != 2)
	{
	  free (full_ftp_name);
	  continue;
	}

      /* Due to the various time stamp formats, its "safer" to retrieve the        */
      /* filename by taking it from the right side of the string, we do that here. */
      ptr = strrchr(buffer,' ');

      if(ptr == NULL)
	continue;

      strcpy(itemname,ptr+1);

      HText_appendText (HT, "<DD>");
      /* Spit out the anchor refrence, and continue on... */

      HText_appendText (HT, "<A HREF=\"");
      /* Assuming it's a relative reference... */
      if (itemname && itemname[0] != '/')
	{
	  HText_appendText (HT, filename);
	  if (filename[strlen(filename)-1] != '/')
	    HText_appendText (HT, "/");
	}
      HText_appendText (HT, itemname);
      HText_appendText (HT, "\"> ");

      /* There are 3 "types", directory, link and file.  If its a directory we     */
      /* just spit out the name with a directory icon.	If its a link, we go	   */
      /* retrieve the proper name (i.e. the input looks like bob -> ../../../bob   */
      /* so we want to hop past the -> and just grab bob.  The link case falls	   */
      /* through to the filetype case.	The filetype shows name and filesize, and  */
      /* then attempts to select the correct icon based on file extension.	   */
      switch(itemtype)
	{
	case 'd':
	  {
	    sprintf(buffer,"%s",itemname);
	    HText_appendText(HT, "<IMG SRC=\"");
	    HText_appendText(HT, HTgeticonname(NULL, "directory"));
	    HText_appendText(HT, "\"> ");
	    break;
	  }

	case 'l':
	  {
	    ptr = strrchr(buffer,' ');
	    if(ptr != NULL)
	      {
		*ptr = '\0';
		ptr = strrchr(buffer,' ');
	      }

	    if(ptr != NULL)
	      {
		*ptr = '\0';
		ptr = strrchr(buffer,' ');
	      }

	    if(ptr != NULL) strcpy(itemname,ptr+1);
	  }

	case '-':
	  {

	    /* If this is a link type, and the bytes are small,
	       its probably a directory so lets not show the byte
	       count */
#if 0
	    if(itemtype == 'l' && atoi(itemsize) < 128)
	      {
		sprintf(buffer,"%s",itemname);
	      }
	    else
	      {
		sprintf(buffer,"%s (%s bytes)",itemname,itemsize);
	      }
#endif

#if 0
	    if(itemtype == 'l')
	      {
#endif
		sprintf(buffer,"%s",itemname);
#if 0
	      }
	    else
	      {
		/* code doesn't work for this, and neither does pre. */
		sprintf(buffer,"<code>%s</code>",full_ftp_name);
	      }
#endif

	    format = HTFileFormat(itemname, &pencoding, WWW_SOURCE, &cmpr);

	    if (1)
	      {
		HText_appendText(HT, "<IMG SRC=\"");

		/* If this is a link, and we can't figure out what
		   kind of file it is by extension, throw up the unknown
		   icon; however, if it isn't a link and we can't figure
		   out what it is, throw up the text icon...

		   Unless it's compressed. */
		if(itemtype == 'l' && cmpr == COMPRESSED_NOT)
		  {
		    /* If it's unknown, let's call it a menu (since symlinks
		       are most commonly used on FTP servers to point to
		       directories, IMHO... -marc */
		    HText_appendText(HT, HTgeticonname(format, "directory") );
		  }
		else
		  {
		    HText_appendText(HT, HTgeticonname(format, "text"));
		  }

		HText_appendText(HT, "\"> ");
	      }
	    else
	      {
		HText_appendText(HT, "<IMG SRC=\"");
		HText_appendText(HT, HTgeticonname(format, "application"));
		HText_appendText(HT, "\"> ");
	      }

	    break;
	    }

	  default:
	    {
	      HText_appendText(HT, "<IMG SRC=\"");
	      HText_appendText(HT, HTgeticonname(NULL, "unknown"));
	      HText_appendText(HT, "\"> ");
	      break;
	    }
	  }

      HText_appendText (HT, buffer);
      HText_appendText (HT, "</A>\n");

      free (full_ftp_name);
    }

  HText_appendText (HT, "</DL>\n");
#endif
  HText_endAppend (HT);

  rv = response (NIL);
  if (rv == HT_INTERRUPTED)
    return rv;
  return rv == 2 ? HT_LOADED : -1;
}


/*	Retrieve File from Server
**	-------------------------
**
** On entry,
**	name		WWW address of a file: document, including hostname
** On exit,
**	returns 	Socket number for file if good.
**			<0 if bad.
*/
#ifndef _DNET
PUBLIC int HTFTPLoad
ARGS4 (
  char *,			name,
  HTParentAnchor *,		anchor,
  HTFormat,			format_out,
  HTStream *,			sink
)
{
  BOOL isDirectory = NO;
  int status;
  int retry;			/* How many times tried? */
  HTFormat format;
  int compressed = 0;

  for (retry = 0; retry < 2; retry++)
    {
      if (TRACE)
	fprintf (stderr, "FTP: TRYING in HTFTPLoad, attempt %d\n", retry);
      status = get_connection(name);
      if (status < 0)
	{
	  NETCLOSE (control);
	  control = -1;
	  /* HT_INTERRUPTED will fall through. */
	  return status;
	}

       /* MORE MWM PASSIVE SUPPORT - Nov 4 1993 */
      status = get_passive_socket();
      if (status < 0)
       /* END MWM PASSIVE SUPPORT  */
       status = get_listen_socket();
      if (status < 0)
	{
	  NETCLOSE (control);
	  control = -1;
	  close_master_socket ();
	  /* HT_INTERRUPTED would fall through, if we could interrupt
	     somehow in the middle of it, which we currently can't. */
	  return status;
	}

      /* Inform the server of the port number we will listen on */
      if (!passive)
      {
	status = response (port_command);
	if (status == HT_INTERRUPTED)
	  {
	    if (TRACE)
	      fprintf (stderr, "FTP: Interrupted in response (port_command)\n");
	    HTProgress ("Connection interrupted.");
	    NETCLOSE (control);
	    control = -1;
	    close_master_socket ();
	    return HT_INTERRUPTED;
	  }
	if (status !=2)
	  {		/* Could have timed out */
	    if (status < 0)
	      {
		NETCLOSE (control);
		control = -1;
		close_master_socket ();
		continue;		/* try again - net error*/
	      }

	    NETCLOSE (control);
	    control = -1;
	    close_master_socket ();
	    return HT_NOT_LOADED;			/* bad reply */
	  }
	if (TRACE)
	  fprintf(stderr, "FTP: Port defined.\n");
      }
      status = 0;
      break;	/* No more retries */
    } /* for retries */

  if (status < 0)
    {
      close_master_socket ();
      NETCLOSE (control);
      control = -1;
      return status;	/* Failed with this code */
    }

  /* Ask for the file: */
  {
    char *filename = HTParse(name, "", PARSE_PATH + PARSE_PUNCTUATION);
    char command[LINE_LENGTH+1];
    HTAtom *encoding;

    if (!(*filename))
      StrAllocCopy(filename, "/");
    format = HTFileFormat (filename, &encoding, WWW_PLAINTEXT, &compressed);

    sprintf(command, "TYPE %s%c%c", "I", CR, LF);
    status = response (command);
    if (status != 2)
      {
	if (status == HT_INTERRUPTED)
	  HTProgress ("Connection interrupted.");
	close_master_socket ();
	NETCLOSE (control);
	control = -1;
	free (filename);
	return (status == HT_INTERRUPTED) ? HT_INTERRUPTED : -1;
      }

    sprintf(command, "RETR %s%c%c", filename, CR, LF);
    status = response (command);
    if (status == HT_INTERRUPTED)
      {
	if (TRACE)
	  fprintf (stderr, "FTP: Interrupted while sending RETR\n");
	HTProgress ("Connection interrupted.");
	NETCLOSE (control);
	control = -1;
	close_master_socket ();
	free (filename);
	return HT_INTERRUPTED;
      }
    if (status != 1)
      {  /* Failed : try to CWD to it */
	sprintf(command, "CWD %s%c%c", filename, CR, LF);
	status = response (command);
	if (status == HT_INTERRUPTED)
	  {
	    if (TRACE)
	      fprintf (stderr, "FTP: Interrupted while sending CWD\n");
	    HTProgress ("Connection interrupted.");
	    NETCLOSE (control);
	    control = -1;
	    close_master_socket ();
	    free (filename);
	    return HT_INTERRUPTED;
	  }
	/* Now pick up status == 5 for reports.adm.cs.cmu.edu, which says:

	   Tx: CWD /
	   Rx: 530-Access not allowed for guest users for path "/".
	   Rx:	   Here is a hint... If you know the full name of the
	   Rx:	   path or directory that you want, try to cd there in
	   Rx:	   one step rather than taking little steps in between.
	   Rx:	   Those intermediate directories are sometimes protected.
	   Rx:	   Or perhaps you are already in the appropriate directory.
	   Rx: 530 Use the pwd command to get the current directory.

	   This may break something.  I dunno...
	if (status == 2 || status == 5)
	   Yep it broke things... */
	if (status == 2)
	  {
	    /* Successed : let's NAME LIST it */
	    isDirectory = YES;
	    sprintf(command, "LIST%c%c", CR, LF);
	    status = response (command);
	    if (status == HT_INTERRUPTED)
	      {
		if (TRACE)
		  fprintf (stderr, "FTP: Interrupted while sending LIST\n");
		HTProgress ("Connection interrupted.");
		NETCLOSE (control);
		control = -1;
		close_master_socket ();
		free (filename);
		return HT_INTERRUPTED;
	      }
	  }
      }
    free(filename);
    if (status != 1)
      {
	NETCLOSE (control);
	control = -1;
	close_master_socket ();
	return HT_NOT_LOADED; /* Action not started */
      }
  }

  /* Wait for the connection */
  if (!passive)
  {
    struct sockaddr_in soc_address;

    int soc_addrlen = sizeof(soc_address);
#ifdef SOCKS
    status = Raccept(master_socket,
#else
    status = accept(master_socket,
#endif
		    (struct sockaddr *)&soc_address,
		    &soc_addrlen);

    if (status < 0)
      {
	NETCLOSE (control);
	control = -1;
	close_master_socket ();
	/* We can't interrupt out of an accept. */
	return HT_NOT_LOADED;
      }

    CTRACE(tfp, "FTP: Accepted new socket %d\n", status);
    data_soc = status;
  }

  if (isDirectory)
    {
      int s = read_directory (anchor, name, format_out, sink);

      NETCLOSE (control);
      control = -1;
      close_master_socket ();
      NETCLOSE (data_soc);
      data_soc = -1;

      if (TRACE)
	fprintf (stderr, "FTP: Returning %d after doing read_directory\n", s);
      /* HT_INTERRUPTED should fall right through. */
      return s;
    }
  else
    {
      /* We reproduce ParseSocket below because of socket/child process
	 problem. */
      HTStream * stream;
      HTStreamClass targetClass;
      int rv;

      stream = HTStreamStack(format,
			     format_out,
			     compressed,
			     sink, anchor);

      if (!stream)
	{
	  char buffer[1024];	/* @@@@@@@@ */
	  sprintf(buffer, "Sorry, can't convert from %s to %s.",
		  HTAtom_name(format), HTAtom_name(format_out));
	  HTProgress (buffer);
	  if (TRACE)
	    fprintf(stderr, "FTP: %s\n", buffer);
	  return HT_NOT_LOADED;
	}

      targetClass = *(stream->isa);	/* Copy pointers to procedures */
      rv = HTCopy(data_soc, stream, 0);
      if (rv == -1)
	{
	  rv = HT_INTERRUPTED;
	}
      else
	{
	  (*targetClass.end_document)(stream);
	  /* Do NOT call *targetClass.free yet -- sockets aren't closed. */
	  rv = HT_LOADED;
	}

      if (TRACE)
	fprintf (stderr, "FTP: Got back %d from our local equivalent of ParseSocket\n", rv);

      /* Reset buffering to control connection -- probably
	 no longer necessary, since we don't use a connection
	 more than once. */
      HTInitInput(control);

      if (TRACE)
	fprintf (stderr, "FTP: Closing data socket %d\n", data_soc);
      NETCLOSE (data_soc);
      data_soc = -1;

      /* Unfortunately, picking up the final reply sometimes causes
	 serious problems.  It *probably* isn't safe not to do this,
	 as there is the possibility that FTP servers could die if they
	 try to send it and we're not listening.

	 Testcase for problems (10/30/93): uxc.cso.uiuc.edu,
	 AnswerGarden COPYRIGHT in X11R5 contrib clients.

	 Of course, we may already be triggering hostile actions
	 by allowing client-side interrupts as follows... */
      if (rv != HT_INTERRUPTED)
	{
	  if (TRACE)
	    fprintf (stderr, "FTP: Picking up final reply...\n");
#ifdef OLD
	  status = response (NIL);		/* Pick up final reply */
	  if (status == HT_INTERRUPTED)
	    {
	      if (TRACE)
		fprintf (stderr, "FTP: Interrupted picking up final reply.\n");
	      HTProgress ("Connection interrupted.");

	      NETCLOSE (control);
	      control = -1;
	      close_master_socket ();

	      (*targetClass.handle_interrupt)(stream);

	      return HT_INTERRUPTED;
	    }
	  if (status != 2)
	    {
	      NETCLOSE (control);
	      control = -1;
	      close_master_socket ();
	      return HT_NOT_LOADED;
	    }
#else
	  if (TRACE)
	    fprintf (stderr, "FTP: Aw hell, we don't care about the final reply!\n");
#endif
	}

      if (TRACE)
	fprintf (stderr, "FTP: Closing control socket %d\n", control);
      NETCLOSE(control);
      control = -1;
      close_master_socket ();

      if (rv != HT_INTERRUPTED)
	{
#else
    return -1;
#endif
	  /* WAIT until all sockets have been closed. */
	  if (TRACE)
	    fprintf (stderr, "FTP: Calling free method, finally.\n");
	  (*targetClass.free)(stream);
	}

      return rv == HT_INTERRUPTED ? HT_INTERRUPTED : HT_LOADED;
    }
} /* open_file_read */
